#pragma once

#include <chrono>
#include <thread>
#include <cstdint>
#include <atomic>
#include "CANSocket.hpp"

class EngineECU {
public:
    //interface, rpm, throttle, coolantTemp
    EngineECU(const std::string&, int, float, float);

    void start();
    //iterations
    void start(int);
    void stop();

    const CANMessage& getLastTransmitted() const { return lastMsg; }
    void decodeMessage(const CANMessage&);

    CANMessage receive();

private:
    int rpm;
    float throttle;
    float coolantTemp;

    std::atomic<bool> isRunning{false};

    CANSocket can_;
    CANMessage lastMsg{};

    //rpm, throttle, coolantTemp
    void tick(int, float, float);
    void encodeMessage();
};
