#pragma once

#include "UniformFiniteElementsSpace.hpp"
#include "SignalSimulationSpace.hpp"
#include "Transmitter.hpp"

#include <vector>

class SignalSimulation;

struct SignalMapElement
{
	double signalStrangth = 0;
};

class SignalMap : protected UniformFiniteElementsSpace<SignalMapElement>
{
public:
	SignalMap(SignalSimulationSpacePtr simulationSpace) :
		UniformFiniteElementsSpace<SignalMapElement>(simulationSpace->bounds, simulationSpace->elementsDistance)
	{ }

	Power getSignalStrength(const Transmitter& transmitter, const Reciver& reciver, Point p) const
	{
		if (!inRange(p))
			return Power();

		double
			signalStrength = transmitter.power.get(PowerUnit::W),
			transmitterGain = transmitter.antenaGain.get(AntenaGainUnit::coefficient),
			reciverGain = reciver.antenaGain.get(AntenaGainUnit::coefficient);


		double power = signalStrength * transmitterGain * reciverGain * getElement(p).signalStrangth;

		return Power(power, PowerUnit::W);
	}

	friend SignalSimulation;
};
using SignalMapPtr = std::shared_ptr<const SignalMap>;