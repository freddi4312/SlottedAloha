#include <optional>
#include <random>


class Request
{
private:
  double creation_time_;

public:
  explicit Request(double creation_time) :
    creation_time_(creation_time)
  {}

  double timeSinceCreation(int current_time) const
  {
    return current_time - creation_time_;
  }
};


class Subscriber
{
private:
  std::optional<Request> request_;
  bool is_transmitting_;
  std::mt19937 & generator_;
  std::uniform_real_distribution<> uniform01_;
  std::poisson_distribution<> poisson_;
  int transmission_count_;
  double overall_delay_;
  int active_duration_;

public:
  Subscriber(double mean, std::mt19937 & generator);

  bool transmit(double prob);
  void generateNewRequest(int current_time);
  void handleTransmissionResult(int transmitted_count, int current_time);

  int transmissionCount() const
  {
    return transmission_count_;
  }

  int activeDuration() const
  {
    return active_duration_;
  }

  double overallDelay() const
  {
    return overall_delay_;
  }

private:
  bool isLucky(double prob);
};
