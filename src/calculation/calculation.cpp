#include <iostream>
#include <vector>
#include <cmath>
#include <numbers>
#include <Eigen/Dense>


int binCoefficient(int n, int k)
{
  if (k > n / 2)
  {
    k = n - k;
  }

  int result = 1;

  for (int i = n - k + 1; i <= n; i++)
  {
    result *= i;
  }

  for (int i = 1; i <= k; i++)
  {
    result /= i;
  }

  return result;
}


double binominalExperimentProb(int total, int success, double prob)
{
  if (success > total)
    return 0;

  double result = binCoefficient(total, success);
  if (success != 0)
    result *= std::pow(prob, success);
  if (success != total)
    result *= std::pow(1 - prob, total - success);

  return result;
}


class AlohaMarkovChain
{
private:
  int subscriber_count_;
  int prob_count_;

public:
  AlohaMarkovChain(int subscriber_count_, int prob_count_)
    : subscriber_count_(subscriber_count_), prob_count_(prob_count_)
  {}

  Eigen::MatrixXd getTransitionMatrix(double lambda) const
  {
    size_t const state_count = stateCount();
    Eigen::MatrixXd matrix = Eigen::MatrixXd::Zero(state_count, state_count);

    double const inactive = std::pow(std::numbers::e, -lambda / subscriber_count_);

    for (int s_t = 0; s_t <= prob_count_; s_t++)
      for (int n_t = 0; n_t <= subscriber_count_; n_t++)
      {
        double const aloha_prob = alohaProb(s_t);
        int const state_id = stateNumber(s_t, n_t);
        auto row = matrix.row(state_id);

        for (int v_t = 0; v_t <= subscriber_count_ - n_t; v_t++)
        {
          double const activation_prob
            = binominalExperimentProb(subscriber_count_ - n_t, v_t, 1 - inactive);

          /* Empty */
          double const zero_transmission_prob
            = binominalExperimentProb(n_t, 0, aloha_prob);

          row(stateNumber(std::max(0, s_t - 1), n_t + v_t))
            += zero_transmission_prob * activation_prob;

          /* Success */
          double const one_transmission_prob
            = binominalExperimentProb(n_t, 1, aloha_prob);

          row(stateNumber(s_t, std::max(0, n_t + v_t - 1)))
            += one_transmission_prob * activation_prob;

          /* Fail */
          double const multiple_transmission
            = 1
            - zero_transmission_prob
            - one_transmission_prob;

          row(stateNumber(std::min(prob_count_, s_t + 1), n_t + v_t))
            += multiple_transmission * activation_prob;
        }
      }

    return matrix;
  }

  int stateCount() const
  {
    return (subscriber_count_ + 1) * (prob_count_ + 1);
  }

  int stateNumber(int s_t, int n_t) const
  {
    return (s_t * (1 + subscriber_count_)) + n_t;
  }

  std::pair<int, int> stateName(int state_id) const
  {
    int const l = subscriber_count_ + 1;

    return { state_id / l, state_id % l };
  }

  static double alohaProb(int s_t)
  {
    return 1.0 / (1 << s_t);
  }
};


Eigen::VectorXd getStationaryDistribution(Eigen::MatrixXd matrix)
{
  int size = matrix.innerSize();
  matrix.transposeInPlace();
  matrix -= Eigen::MatrixXd::Identity(size, size);
  for (auto & x : matrix.row(size - 1))
  {
    x = 1;
  }

  Eigen::VectorXd b = Eigen::VectorXd::Zero(size);
  b[size - 1] = 1;

  return matrix.colPivHouseholderQr().solve(b);
}


struct LambdaRelation
{
  std::vector<double> avg_subscriber_count;
  std::vector<double> avg_delay;
};


LambdaRelation calculateLambdaRelation(int const subscriber_count, int const prob_count, int const resolution = 10)
{
  AlohaMarkovChain chain(subscriber_count, prob_count);
  std::vector<double> avg_subscriber_count_map(resolution + 1);
  std::vector<double> avg_delay_map(resolution + 1);

  for (int i = 0; i <= resolution; i++)
  {
    double lambda = i * 1.0 / resolution;
    auto matrix = chain.getTransitionMatrix(lambda);
    Eigen::VectorXd distribution = getStationaryDistribution(std::move(matrix));
    
    double avg_subscriber_count = 0;
    double avg_success_count = 0;
    for (int s_t = 0; s_t <= prob_count; s_t++)
      for (int n_t = 0; n_t <= subscriber_count; n_t++)
      {
        int const state_id = chain.stateNumber(s_t, n_t);

        avg_subscriber_count 
          += n_t * distribution[state_id];
        avg_success_count
          += binominalExperimentProb(n_t, 1, AlohaMarkovChain::alohaProb(s_t))
          * distribution[state_id];
      }

    avg_subscriber_count_map[i] = avg_subscriber_count;
    avg_delay_map[i] = avg_subscriber_count / avg_success_count + 0.5;
  }

  return { avg_subscriber_count_map, avg_delay_map };
}


int main()
{
  int resolution = 10;
  int subscriber_count;
  int prob_count;
  std::cin >> subscriber_count >> prob_count;
  
  LambdaRelation relation = calculateLambdaRelation(subscriber_count, prob_count, resolution);

  std::cout << "Average amount of subscribers, delay:\n";
  for (int i = 0; i <= resolution; i++)
  {
    double const lambda = i * 1.0 / resolution;
    std::cout << lambda << ": " << relation.avg_subscriber_count[i] << "\t" << relation.avg_delay[i] << '\n';
  }

  return 0;
}
