#include <iostream>
#include <vector>
#include "AlohaMarkovChain.h"


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

  for (int i = 1; i <= resolution; i++)
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
  for (int i = 1; i <= resolution; i++)
  {
    double const lambda = i * 1.0 / resolution;
    std::cout << lambda << ": " << relation.avg_subscriber_count[i] << "\t" << relation.avg_delay[i] << '\n';
  }

  return 0;
}
