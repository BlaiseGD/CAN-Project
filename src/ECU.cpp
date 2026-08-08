#include "ECU.hpp"
#include <iostream>
#include <atomic>
using namespace std::literals::chrono_literals;


constexpr uint32_t ECU_CAN_ID = 0x120;
constexpr uint8_t ECU_DATA_LENGTH = 8;

EngineECU::EngineECU(const std::string& iface, int r, float t, float c) : rpm(r), throttle(t), coolantTemp(c), can_(iface) {}

void EngineECU::tick(int r, float t, float c) {
    // run every tick
    this->rpm = r;
    this->throttle = t;
    this->coolantTemp = c;
}

void EngineECU::encodeMessage() {
   //encode the rpm info, coolant info, and throttle info
   //get the most significant byte and then the next byte
    lastMsg.id = ECU_CAN_ID;
    lastMsg.length = ECU_DATA_LENGTH;
    //filling with all 0s by default
    lastMsg.data.fill(0);

    //big endian for payload
    lastMsg.data[0] = static_cast<uint8_t>((rpm >> 8) & 0xFF);
    lastMsg.data[1] = static_cast<uint8_t>(rpm & 0xFF);

    //need 16 bits for throttle and coolant and then split them
    uint16_t throttleBig = static_cast<uint16_t>(throttle * 100.0f);

    lastMsg.data[2] = static_cast<uint8_t>((throttleBig >> 8) & 0xFF);
    lastMsg.data[3] = static_cast<uint8_t>(throttleBig & 0xFF);

    uint16_t coolantBig = static_cast<uint16_t>(coolantTemp * 100.0f);
    
    lastMsg.data[4] = static_cast<uint8_t>((coolantBig >> 8) & 0xFF);
    lastMsg.data[5] = static_cast<uint8_t>(coolantBig & 0xFF);

}

void EngineECU::stop(){ isRunning = false; }

void EngineECU::start() {
    this->isRunning = true;
    const auto interval = 10ms;
    auto timeUpdate = std::chrono::high_resolution_clock::now() + interval;

    //normally read from physical hardware, don't have it so virtual
    int r = 5000;
    float t = 10.0f;
    float c = 10.0f;

    while(isRunning){
        // tick
        tick(r, t, c);
        encodeMessage();
        //rpm limit
        if(r < 8000) r += 50;

        can_.send(lastMsg);

        timeUpdate += interval;
        //sleep for 10 ms between ticks
        //need sleep_until to not have the execution time + delay (sleep_for does this)
        std::this_thread::sleep_until(timeUpdate);
    }
}

void EngineECU::start(int iterations) {
    this->isRunning = true;
    const auto interval = 10ms;
    auto timeUpdate = std::chrono::high_resolution_clock::now() + interval;

    //normally read from physical hardware, don't have it so virtual
    int r = 5000;
    float t = 10.0f;
    float c = 10.0f;

    for(int i = 0; i < iterations; i++){
        // tick
        tick(r, t, c);
        encodeMessage();
        //rpm limit
        if(r < 8000) r += 50;

        can_.send(lastMsg);

        timeUpdate += interval;
        //sleep for 10 ms between ticks
        //need sleep_until to not have the execution time + delay (sleep_for does this)
        std::this_thread::sleep_until(timeUpdate);
    }
}

CANMessage EngineECU::receive() { return can_.receive(); }

void EngineECU::decodeMessage(const CANMessage& message){
	
	//rx  id=0x120 len=8 data=13 88 3 E8 23 28 0 0
	std::cout << "rx\t id=" << std::hex << message.id << " ";
	std::cout << "len=" << std::dec << static_cast<int>(message.length) << " data=";
	for(int i = 0; i < message.length; i++){
		std::cout << std::hex << static_cast<int>(message.data[i]) << " ";
	}
	std::cout << "\n";
}

