#include <cerrno>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <cstring>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "CANSocket.hpp"

CANSocket::CANSocket(const std::string& interface){
    sockaddr_can addr{};
    // using CAN, give me the raw CAN frames
    fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(fd_ < 0) {
        throw std::runtime_error("CANSocket: socket() failed: " + std::string(std::strerror(errno)));
    }


    //let the socket read its transmitted frames
    int recv_own_msgs = 1;
    //added error checks for socket options setting
    if(setsockopt(fd_, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs)) < 0){
	    throw std::runtime_error("CANSocket: setsockopt() failed: " + std::string(strerror(errno)));
    }

    //getting kernel's interface index for vcan0
    unsigned int ifindex = if_nametoindex(interface.c_str());
    if(ifindex == 0){
	    throw std::runtime_error("CANSocket: if_nametoindex() failed: " + std::string(strerror(errno)));
    }
    //associate the interface with the socket
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifindex;

    //bind the socket
    if(bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0){
	    throw std::runtime_error("CANSocket: bind() failed: " +std::string(std::strerror(errno)));
    }
}

CANSocket::~CANSocket(){
    if(fd_ >= 0) close(fd_);
}

void CANSocket::send(const CANMessage& message){
    can_frame frame{};
    frame.can_id = message.id;
    frame.len = message.length;

    for(int i = 0; i < message.length; ++i){
        frame.data[i] = message.data[i];
    }
    if(write(fd_, &frame, sizeof(frame)) < 0){
		throw std::runtime_error("CANSocket: failed to write " + std::string(std::strerror(errno)) + "\n");
		return;
    }

}

CANMessage CANSocket::receive(){
    can_frame frame{};

    if(read(fd_, &frame, sizeof(frame)) < 0){
	    throw std::runtime_error("CANSocket: failed to read" + std::string(std::strerror(errno)) + "\n");
	    return CANMessage{};
    }

    // put it into message format
    CANMessage message{};
    message.id = frame.can_id;
    message.length = frame.len;

    for(int i = 0; i < message.length; ++i){
        message.data[i] = frame.data[i];
    }

    return message;
}
