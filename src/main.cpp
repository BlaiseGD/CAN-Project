#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include "CANMessage.hpp"
#include "CANSocket.hpp"
#include "ECU.hpp"
using namespace std::literals::chrono_literals;


int main() {
    int iterations = 0;
    std::atomic <bool> receiveIsRunning{true};
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
    	//	std::cout << "this is running";
                testEngine.start();
            }
        );
	std::thread receiverThread(
		[&testEngine, &receiveIsRunning](){
			while(receiveIsRunning){
				CANMessage msg = testEngine.receive();
				testEngine.decodeMessage(msg);
			}
		}
	);

	//to make it go "indefinitely" for longer, change this sleep statement
	//could also make it user choice or actually indefinite.
        std::this_thread::sleep_for(5s);
        testEngine.stop();
	receiveIsRunning = false;
        //join = wait until thread finishes
        engineThread.join();
	//get this to run in background
	receiverThread.detach();
    }
    else{
	    std::thread engineThread([&testEngine, iterations](){
                testEngine.start(iterations);
            });

	    std::thread receiverThread([&testEngine, iterations](){
        	for (int i = 0; i < iterations; ++i){
            		CANMessage msg = testEngine.receive();
            		testEngine.decodeMessage(msg);
        	}
		});
	    engineThread.join();
	    receiverThread.join();
    }

	return 0;
}

