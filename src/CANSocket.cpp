#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <cstring>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "CANSocket.hpp"

CANSocket::CANSocket(const std::string& interface) {
    ifreq ifr {};
    sockaddr_can addr{};
    // using CAN, give me the raw CAN frames
    fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd_ < 0) {
        throw std::runtime_error("CANSocket: socket() failed: " + std::string(std::strerror(errno)));
    }

    // tells which interface we're using
    std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ioctl(fd_, SIOCGIFINDEX, &ifr)

    //associate the interface with the socket
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    //bind the socket
    bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
}

CANSocket::~CANSocket() {
    if (fd_ >= 0) close(fd_);
}

void CANSocket::send(const CANMessage& message) {
    can_frame frame{};
    frame.can_id = message.id;
    frame.len = message.length;

    for (int i = 0; i < message.length; ++i) {
        frame.data[i] = message.data[i];
    }
    write(fd_, &frame, sizeof(frame));
}

CANMessage CANSocket::receive() {
    can_frame frame{};

    read(fd_, &frame, sizeof(frame));

    // put it into message format
    CANMessage message{};
    message.id = frame.can_id;
    message.length = frame.len;

    for (int i = 0; i < message.length; ++i) {
        message.data[i] = frame.data[i];
    }

    return message;
}