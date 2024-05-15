#include "AlohaSimulator.h"


auto AlohaSimulator::simulate(double lambda, int tick_count) -> SimulationResult
{
  int s_t = 0;
  double mean = lambda / subscriber_count_;
  std::vector<Subscriber> subscribers(subscriber_count_, Subscriber(mean, generator_));

  for (int tick = 0; tick < tick_count; tick++)
  {
    int transmitted_count = 0;
    double aloha_prob = alohaProb(s_t);
    for (Subscriber & subscriber : subscribers)
    {
      if (subscriber.transmit(aloha_prob))
      {
        transmitted_count++;
      }
    }

    for (Subscriber & subscriber : subscribers)
    {
      subscriber.generateNewRequest(tick);
    }

    for (Subscriber & subscriber : subscribers)
    {
      subscriber.handleTransmissionResult(transmitted_count, tick + 1);
    }

    if (transmitted_count == 0)
    {
      s_t = std::max(0, s_t - 1);
    }
    else if (transmitted_count > 1)
    {
      s_t = std::min(prob_count_, s_t + 1);
    }
  }

  return { averageActiveCount(subscribers, tick_count), averageDelay(subscribers) };
}


double AlohaSimulator::averageActiveCount(std::vector<Subscriber> const & subscribers, int duration) const
{
  int total_active_time = 0;

  for (Subscriber const & subscriber : subscribers)
  {
    total_active_time += subscriber.activeDuration();
  }

  return total_active_time * 1.0 / duration;
}


double AlohaSimulator::averageDelay(std::vector<Subscriber> const & subscribers) const
{
  double weighted_sum = 0;
  int transmission_count = 0;

  for (Subscriber const & subscriber : subscribers)
  {
    transmission_count += subscriber.transmissionCount();
    weighted_sum += subscriber.overallDelay();
  }

  return weighted_sum / transmission_count;
}


double AlohaSimulator::alohaProb(int s_t)
{
  return 1.0 / (1 << s_t);
}
