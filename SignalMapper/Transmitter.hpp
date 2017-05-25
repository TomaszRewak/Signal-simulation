#pragma once

#include "Physics.hpp"

struct Transmitter
{
	Power power;
	AntenaGain antenaGain;

	Transmitter(Power power, AntenaGain antenaGain):
		power(power), antenaGain(antenaGain)
	{ }
};

struct Receiver
{
	AntenaGain antenaGain;

	Receiver(AntenaGain antenaGain) :
		antenaGain(antenaGain)
	{ }
};