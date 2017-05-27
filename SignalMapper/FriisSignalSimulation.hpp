#pragma once

#include "SignalSimulation.hpp"

#include <vector>
#include <algorithm>

struct FriisSignalSimulationParameters {
	Transmitter bestTransmitter;
	Receiver bestReceiver;
	Power minimumPower;

	FriisSignalSimulationParameters(
		Transmitter bestTransmitter,
		Receiver bestReceiver,
		Power minimumPower
	) :
		bestTransmitter(bestTransmitter),
		bestReceiver(bestReceiver),
		minimumPower(minimumPower)
	{ }
};

struct FriisSignalSimulationDistortion
{
	AbsorptionCoefficient absorption;

	FriisSignalSimulationDistortion()
	{}
};

class FriisSignalSimulation : public SignalSimulation
{
private:
	struct Ray
	{
		DiscretePoint position;

		Position source;
		Distance distance;
		Distance previousDistance;

		FreeVector normalVector;
		FreeVector offset;

		int reflections = 0;

		PowerCoefficient powerCoefficient;

		Ray(Position source, DiscretePoint position, FreeVector normalVector, int reflections) :
			source(source),
			position(position),
			normalVector(normalVector.normalized()),
			reflections(reflections),
			powerCoefficient(PowerCoefficient::in<PowerCoefficient::Unit::coefficient>(1))
		{ }
	};

	const Frequency frequency;
	const FriisSignalSimulationParameters simulationParameters;
	const SignalSimulationSpaceDefinitionPtr simulationSpaceDefinition;

public:
	FriisSignalSimulation(SignalSimulationSpaceDefinitionPtr simulationSpaceDefinition, Frequency frequency, FriisSignalSimulationParameters simulationParameters) :
		simulationSpaceDefinition(simulationSpaceDefinition),
		frequency(frequency),
		simulationParameters(simulationParameters)
	{ }

	virtual SignalMapPtr simulate(Position transmitterPosition) const
	{
		Point p = transmitterPosition.get<Distance::Unit::m>();
		p.x += 0.0001;
		p.y += 0.0002;
		transmitterPosition = Position::in<Distance::Unit::m>(p);

		auto signalMap = std::make_shared<SignalMap>(simulationSpaceDefinition->spaceSize, simulationSpaceDefinition->precision);
		auto minimumCoefficient =
			simulationParameters.minimumPower /
			(simulationParameters.bestTransmitter.power *
				simulationParameters.bestTransmitter.antenaGain *
				simulationParameters.bestReceiver.antenaGain);

		for (int x = 0; x < signalMap->resolution.width; x++)
		{
			for (int y = 0; y < signalMap->resolution.height; y++)
			{
				DiscretePoint discretePosition(x, y);
				Position position = signalMap->getPosition(discretePosition);
				Distance distance = transmitterPosition.distanceTo(position);

				PowerCoefficient powerCoefficient = PowerCoefficient::in<PowerCoefficient::Unit::coefficient>(1);

				for (const auto& obstacle : simulationSpaceDefinition->obstacles)
				{
					powerCoefficient = powerCoefficient * obstacle->absorption(transmitterPosition, position, frequency).get<AbsorptionCoefficient::Unit::coefficient>(distance);
				}

				signalMap->getElement(discretePosition) = powerCoefficient * std::pow(frequency / (distance * 4 * 3.141592653589793238463), 2);
			}
		}

		return signalMap;
	}
};