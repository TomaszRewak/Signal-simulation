#pragma once

struct Transmitter
{
	double frequency;
	double signalStrength;

	Transmitter(double frequency, double signalStrength):
		frequency(frequency), signalStrength(signalStrength)
	{ }
};