#include <vector>
#include "Subscriber.h"


class AlohaSimulator
{
private:
  int subscriber_count_;
  int prob_count_;
  std::mt19937 generator_;

public:
  struct SimulationResult
  {
    double avg_subscriber_count;
    double avg_delay;
  };

  AlohaSimulator(int subscriber_count, int prob_count, uint32_t seed = std::random_device()()) :
    subscriber_count_(subscriber_count),
    prob_count_(prob_count),
    generator_(seed)
  {}

  SimulationResult simulate(double lambda, int tick_count);

private:
  double averageActiveCount(std::vector<Subscriber> const & subscribers, int duration) const;
  double averageDelay(std::vector<Subscriber> const & subscribers) const;

  static double alohaProb(int s_t);
};
