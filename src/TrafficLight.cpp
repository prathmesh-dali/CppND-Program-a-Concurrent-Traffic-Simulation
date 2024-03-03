#include "TrafficLight.h"

#include <iostream>
#include <random>

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive() {
  std::unique_lock<std::mutex> uLck(_mutex);
  _queue.clear();
  _cond.wait(uLck, [this] { return !_queue.empty(); });

  T msg = std::move(_queue.back());
  _queue.pop_back();
  return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg) {
  std::lock_guard<std::mutex> lck(_mutex);
  _queue.emplace_back(std::move(msg));
  _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

void TrafficLight::waitForGreen() {
  while (true) {
    if (_queue.receive() == TrafficLightPhase::green) {
      return;
    }
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
  auto previousUpdateCycle = std::chrono::system_clock::now();
  std::random_device randomFromDevice;
  std::mt19937 generator(randomFromDevice());
  std::uniform_real_distribution<> distribution(4000.0, 6000.0);
  float timeThreshold = distribution(generator);
  while (true) {
    auto currentCycleTime = std::chrono::system_clock::now();
    long timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
                        currentCycleTime - previousUpdateCycle)
                        .count();
    if (timeDiff >= timeThreshold) {
      _currentPhase = _currentPhase == TrafficLightPhase::green
                          ? TrafficLightPhase::red
                          : TrafficLightPhase::green;
      _queue.send(std::move(_currentPhase));
      previousUpdateCycle = currentCycleTime;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
