#include <utility>
#include <Eigen/Dense>


int binCoefficient(int n, int k);

double binominalExperimentProb(int total, int success, double prob);


class AlohaMarkovChain
{
private:
  int subscriber_count_;
  int prob_count_;

public:
  AlohaMarkovChain(int subscriber_count_, int prob_count_)
    : subscriber_count_(subscriber_count_), prob_count_(prob_count_)
  {}

  Eigen::MatrixXd getTransitionMatrix(double lambda) const;
  int stateCount() const;
  int stateNumber(int s_t, int n_t) const;
  std::pair<int, int> stateName(int state_id) const;
  static double alohaProb(int s_t);
};


Eigen::VectorXd getStationaryDistribution(Eigen::MatrixXd matrix);
