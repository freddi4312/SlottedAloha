#include <iostream>
#include <vector>
#include <cmath>
#include <numbers>
#include <random>


class AlohaSimulator
{
private:
  int subscriber_count_;
  int prob_count_;
  std::mt19937 generator_;
  std::uniform_real_distribution<> distribution_;

  enum class Result
  {
    empty,
    success,
    fail
  };

public:
  struct SimulationResult
  {
    double avg_subscriber_count;
    double avg_transmission_count;
  };

  AlohaSimulator(int subscriber_count, int prob_count, uint32_t seed = std::random_device()()): 
    subscriber_count_(subscriber_count), 
    prob_count_(prob_count),
    generator_(seed),
    distribution_(0, 1)
  {}

  SimulationResult simulate(double lambda, int tick_count)
  {
    int overall_subscriber_count = 0;
    int successful_transmission_count = 0;

    int s_t = 0;
    int n_t = 0;
    int transmitted_count = 0;
    double const activation_prob = 1 - std::pow(std::numbers::e, -lambda / subscriber_count_);

    for (int t = 0; t < tick_count; t++)
    {
      overall_subscriber_count += n_t;

      switch (transmitted_count)
      {
      case 0:
        s_t = std::max(0, s_t - 1);
        break;

      case 1:
        successful_transmission_count++;
        break;

      default:
        s_t = std::min(prob_count_, s_t + 1);
      }

      int v_t = 0;
      for (int i = 0; i < subscriber_count_ - n_t; i++)
      {
        if (isLucky(activation_prob))
          v_t++;
      }
      n_t += v_t;

      if (transmitted_count == 1)
        n_t--;

      transmitted_count = 0;
      double aloha_prob = alohaProb(s_t);
      for (int i = 0; i < n_t; i++)
      {
        if (isLucky(aloha_prob))
          transmitted_count++;
      }
    }

    return {overall_subscriber_count * 1.0 / tick_count, successful_transmission_count * 1.0 / tick_count};
  }

private:
  bool isLucky(double prob)
  {
    double roll_result = distribution_(generator_);

    return roll_result < prob;
  }

public:
  static double alohaProb(int s_t)
  {
    return 1.0 / (1 << s_t);
  }
};


struct LambdaRelation
{
  std::vector<double> avg_subscriber_count;
  std::vector<double> avg_delay;
};


LambdaRelation estimateLambdaRelation(int subscriber_count, int prob_count, int resolution, int tick_count)
{
  uint32_t seed = 7;
  std::vector<double> avg_subscriber_count(resolution + 1);
  std::vector<double> avg_delay(resolution + 1);
  AlohaSimulator simulator(subscriber_count, prob_count, seed);

  for (int i = 0; i <= resolution; i++)
  {
    double lambda = i * 1.0 / resolution;
    AlohaSimulator::SimulationResult result = simulator.simulate(lambda, tick_count);
    avg_subscriber_count[i] = result.avg_subscriber_count;
    avg_delay[i] = result.avg_subscriber_count / result.avg_transmission_count + 0.5;
  }

  return { avg_subscriber_count, avg_delay };
}


int main()
{
  int tick_count = 1000000;
  int resolution = 10;
  int subscriber_count;
  int prob_count;
  std::cin >> subscriber_count >> prob_count;

  LambdaRelation relation = estimateLambdaRelation(subscriber_count, prob_count, resolution, tick_count);

  std::cout << "Average amount of subscribers, delay:\n";
  for (int i = 0; i <= resolution; i++)
  {
    double const lambda = i * 1.0 / resolution;
    std::cout << lambda << ": " << relation.avg_subscriber_count[i] << "\t" << relation.avg_delay[i] << '\n';
  }

  return 0;
}
