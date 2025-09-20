#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <thread>
/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> lock(_mutex);
    
    // Wait until the queue is not empty
    _cond.wait(lock, [this] { return !_queue.empty(); });

    // Retrieve message from the back of the queue (FIFO)
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lock(_mutex);

    // Add message to the front of the queue
    _queue.push_front(std::move(msg));

    // Notify one waiting thread
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true) {
        TrafficLightPhase phase = _messageQueue.receive();
        if (phase == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method âcycleThroughPhasesâ should be started in a thread when the public method âsimulateâ is called. To do this, use the thread queue in the base class. 
    	threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
      // Random number generator for cycle duration between 4 and 6 seconds
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<int> distr(4000, 6000); // milliseconds

    // Start time
    auto lastUpdate = std::chrono::steady_clock::now();

    // Initial cycle duration
    int cycleDuration = distr(eng);

    while (true)
    {
        // Sleep to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Time since last phase change
        auto now = std::chrono::steady_clock::now();
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();

        if (timeSinceLastUpdate >= cycleDuration)
        {
            // Toggle current phase
            _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;

            // Send update to message queue (part of FP.2)
            _messageQueue.send(std::move(_currentPhase));

            // Reset time and get new random duration
            lastUpdate = std::chrono::steady_clock::now();
            cycleDuration = distr(eng);
        }
    }
}

