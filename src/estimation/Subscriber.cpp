#include "Subscriber.h"


Subscriber::Subscriber(double mean, std::mt19937 & generator) :
  request_(),
  is_transmitting_(false),
  generator_(generator),
  poisson_(mean),
  transmission_count_(0),
  overall_delay_(0),
  active_duration_(0)
{}


bool Subscriber::transmit(double prob)
{
  if (request_.has_value())
  {
    active_duration_++;

    if (isLucky(prob))
    {
      is_transmitting_ = true;
      return true;
    }
  }

  is_transmitting_ = false;
  return false;
}


void Subscriber::generateNewRequest(int current_time)
{
  if (request_.has_value() || poisson_(generator_) < 1)
  {
    return;
  }

  request_.emplace(current_time + uniform01_(generator_));
}


void Subscriber::handleTransmissionResult(int transmitted_count, int current_time)
{
  if (transmitted_count != 1 || !is_transmitting_)
  {
    return;
  }

  transmission_count_++;
  overall_delay_ += request_.value().timeSinceCreation(current_time);
  request_.reset();
}


bool Subscriber::isLucky(double prob)
{
  double roll_result = uniform01_(generator_);

  return roll_result < prob;
}
