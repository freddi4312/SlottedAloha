#include <iostream>
#include "AlohaSimulator.h"


struct LambdaRelation
{
  std::vector<double> avg_subscriber_count;
  std::vector<double> avg_delay;
};


LambdaRelation estimateLambdaRelation(int subscriber_count, int prob_count, int resolution, int tick_count, uint32_t seed)
{
  std::vector<double> avg_subscriber_count(resolution + 1);
  std::vector<double> avg_delay(resolution + 1);
  AlohaSimulator simulator(subscriber_count, prob_count, seed);

  for (int i = 1; i <= resolution; i++)
  {
    double lambda = i * 1.0 / resolution;
    AlohaSimulator::SimulationResult result = simulator.simulate(lambda, tick_count);
    avg_subscriber_count[i] = result.avg_subscriber_count;
    avg_delay[i] = result.avg_delay;
  }

  return { avg_subscriber_count, avg_delay };
}


int main()
{
  uint32_t seed = 13;
  int tick_count = 1000000;
  int resolution = 10;
  int subscriber_count;
  int prob_count;
  std::cin >> subscriber_count >> prob_count;

  LambdaRelation relation = estimateLambdaRelation(subscriber_count, prob_count, resolution, tick_count, seed);

  std::cout << "Average amount of subscribers, delay:\n";
  for (int i = 1; i <= resolution; i++)
  {
    double const lambda = i * 1.0 / resolution;
    std::cout << lambda << ": " << relation.avg_subscriber_count[i] << "\t" << relation.avg_delay[i] << '\n';
  }

  return 0;
}
