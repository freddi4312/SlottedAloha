#include <cmath>
#include <numbers>
#include "AlohaMarkovChain.h"


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


Eigen::MatrixXd AlohaMarkovChain::getTransitionMatrix(double lambda) const
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

        /* Collision */
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


int AlohaMarkovChain::stateCount() const
{
  return (subscriber_count_ + 1) * (prob_count_ + 1);
}


int AlohaMarkovChain::stateNumber(int s_t, int n_t) const
{
  return (s_t * (1 + subscriber_count_)) + n_t;
}


std::pair<int, int> AlohaMarkovChain::stateName(int state_id) const
{
  int const l = subscriber_count_ + 1;

  return { state_id / l, state_id % l };
}


double AlohaMarkovChain::alohaProb(int s_t)
{
  return 1.0 / (1 << s_t);
}


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
