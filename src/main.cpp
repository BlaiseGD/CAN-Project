#include <iostream>
#include <thread>
#include <chrono>
#include "CANMessage.hpp"
#include "CANSocket.hpp"
#include "ECU.hpp"
using namespace std::literals::chrono_literals;


int main() {
    int iterations = 0;
    //frame receive format
    //rx  id=0x120 len=8 data=13 88 3 E8 23 28 0 0
    EngineECU testEngine{"vcan0", 5000, 80.0f, 100.0f};
    std::cout << "How many iterations do you want? (0 for indefinite): \n";
    std::cin >> iterations;
    std::cout << '\n';

    //dealing with overloaded start function with lambda
    //thread will use initialized engine object to do the start function
    if(iterations == 0){
        std::thread engineThread(
            [&testEngine](){
                testEngine.start();
            }
        );
        std::this_thread::sleep_for(5s);
        testEngine.stop();
        //join = wait until thread finishes
        engineThread.join();
    }
    else{
        std::thread engineThread(
            [&testEngine, iterations](){
                testEngine.start(iterations);
            }
        );
        //join = wait until thread finishes
        engineThread.join();
    }
   
    

}