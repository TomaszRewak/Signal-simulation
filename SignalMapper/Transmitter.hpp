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

struct Reciver
{
	AntenaGain antenaGain;

	Reciver(AntenaGain antenaGain) :
		antenaGain(antenaGain)
	{ }
};