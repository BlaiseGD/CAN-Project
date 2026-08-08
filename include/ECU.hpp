#pragma once

#include <chrono>
#include <thread>
#include <cstdint>
#include <atomic>
#include "CANSocket.hpp"

class EngineECU {
public:
    // can change this to 32 if need above ~65K RPS
    uint16_t targetRPS = 100;
    void start(int);
    void start();
    void stop();
    EngineECU(const std::string&, int, float, float);

    //getters to read ecu info and last message
    int getRPM() const { return rpm; }
    float getThrottle() const { return throttle; }
    float getCoolantTemp() const { return coolantTemp; }
    const CANMessage& getLastTransmitted() const { return lastMsg; }
    void decodeMessage(const CANMessage&);
    void decodeMessage(const CANMessage&, int);
    CANMessage receive();

private:
    int rpm;
    float throttle;
    float coolantTemp;
    std::atomic <bool> isRunning{false};
    CANSocket can_;
    CANMessage lastMsg{};
    void tick(int, float, float);
    void encodeMessage();
};
