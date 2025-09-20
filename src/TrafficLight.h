#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


// FP.3 Define a class âMessageQueueâ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:
    // Receive blocks until a message is available and returns it
    T receive();

    // Send a message to the queue using move semantics
    void send(T&& msg);

private:
    std::deque<T> _queue;               // Message queue
    std::mutex _mutex;                  // Mutex for thread-safe access
    std::condition_variable _cond;      // Condition variable for notification
};

// FP.1 : Define a class âTrafficLightâ which is a child class of TrafficObject. 
// The class shall have the public methods âvoid waitForGreen()â and âvoid simulate()â 
// as well as âTrafficLightPhase getCurrentPhase()â, where TrafficLightPhase is an enum that 
// can be either âredâ or âgreenâ. Also, add the private method âvoid cycleThroughPhases()â. 
// Furthermore, there shall be the private member _currentPhase which can take âredâ or âgreenâ as its value. 
enum class TrafficLightPhase { red, green };

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();
    // getters / setters

    // typical behaviour methods

private:
    // typical behaviour methods
    void cycleThroughPhases();
    
    TrafficLightPhase _currentPhase;
    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.

    std::condition_variable _condition;
    std::mutex _mutex;
    MessageQueue<TrafficLightPhase> _messageQueue;  
};

#endif