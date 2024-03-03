#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase{
    green, red
};


template <class T>
class MessageQueue
{
public:
    void send(T && msg);
    T receive();

private:
    std::deque<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;
};


class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    // getters / setters
    TrafficLightPhase getCurrentPhase();
    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods

    void cycleThroughPhases();

    TrafficLightPhase _currentPhase;

    MessageQueue<TrafficLightPhase> _queue;

    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif