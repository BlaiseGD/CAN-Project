#pragma once

#include <string>
#include "CANMessage.hpp"

class CANSocket {
public:
    explicit CANSocket(const std::string& ifname);

    ~CANSocket();

    void send(const CANMessage&);
    CANMessage receive();

private:
    int fd_{-1};
};