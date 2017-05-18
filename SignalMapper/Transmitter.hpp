#pragma once

#include "Physics.hpp"

struct Transmitter
{
	Frequency frequency;
	Power power;
	AntenaGain antenaGain;

	Transmitter(Frequency frequency, Power power, AntenaGain antenaGain):
		frequency(frequency), power(power), antenaGain(antenaGain)
	{ }
};

struct Reciver
{
	AntenaGain antenaGain;

	Reciver(AntenaGain antenaGain) :
		antenaGain(antenaGain)
	{ }
};