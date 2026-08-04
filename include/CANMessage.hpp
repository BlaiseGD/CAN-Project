#pragma once

#include <array>
#include <cstdint>

//defining the whole message so I can pass it as parameter conveniently
struct CANMessage{
	uint32_t id{};
	uint8_t length{};
	std::array<uint8_t,8> data{};
};
