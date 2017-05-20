#pragma once

#include "UniformFiniteElementsSpace.hpp"
#include "SignalSimulationSpace.hpp"
#include "Transmitter.hpp"
#include "SimulationSpace.hpp"

#include <vector>

class SignalSimulation;

class BuildingMap : protected SimulationUniformFiniteElementsSpace<int>
{
public:
	BuildingMap(SimulationSpacePtr simulationSpace) :
		SimulationUniformFiniteElementsSpace(simulationSpace)
	{
		for (int x = 0; x < resolution.width; x++)
			for (int y = 0; y < resolution.height; y++)
				getElement(DiscretePoint(x, y)) = 0;

		for (const auto& obstacle : simulationSpace->obstacles)
		{
			for (int x = 0; x < resolution.width; x++)
			{
				for (int y = 0; y < resolution.height; y++)
				{
					DiscretePoint elementDiscretePosition(x, y);
					Position elementPosition = getPosition(elementDiscretePosition);

					if (obstacle->inside(elementPosition))
					{
						auto& element = getElement(elementDiscretePosition);
						element++;
					}
				}
			}
		}
	}

	bool hasObstacle(Position position) const
	{
		if (!inRange(position))
			return false;

		return getElement(position) > 0;
	}
};
using BuildingMapPtr = std::shared_ptr<const BuildingMap>;

class SignalMap : protected SimulationUniformFiniteElementsSpace<double>
{
public:
	SignalMap(SimulationSpacePtr simulationSpace) :
		SimulationUniformFiniteElementsSpace(simulationSpace)
	{
		for (int x = 0; x < resolution.width; x++)
			for (int y = 0; y < resolution.height; y++)
				getElement(DiscretePoint(x, y)) = 0;
	}

	Power getSignalStrength(Position position, const Transmitter& transmitter, const Reciver& reciver) const
	{
		if (!inRange(position))
			return Power();

		double
			signalStrength = transmitter.power.get(Power::Unit::mW),
			transmitterGain = transmitter.antenaGain.get(AntenaGain::Unit::coefficient),
			reciverGain = reciver.antenaGain.get(AntenaGain::Unit::coefficient);


		double power = signalStrength * transmitterGain * reciverGain * getElement(position);

		return Power(power, Power::Unit::mW);
	}

	friend SignalSimulation;
};
using SignalMapPtr = std::shared_ptr<const SignalMap>;