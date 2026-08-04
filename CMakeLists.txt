cmake_minimum_required(VERSION 3.16)
project(CANProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(can_demo
    src/main.cpp
    src/CANSocket.cpp
    src/ECU.cpp
)

target_include_directories(can_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Linux only: CAN sockets use Linux kernel headers
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "This project requires Linux (CAN sockets are not available on ${CMAKE_SYSTEM_NAME})")
endif()

# Threads for std::thread
find_package(Threads REQUIRED)
target_link_libraries(can_demo PRIVATE Threads::Threads)