#pragma once

#include "SignalSimulation.hpp"

#include <vector>
#include <algorithm>
#include <limits>

struct BFSSignalSimulationParameters {
	Transmitter bestTransmitter;
	Receiver bestReceiver;
	Power minimumPower;
	PowerCoefficient turnCoefficient;

	BFSSignalSimulationParameters(
		Transmitter bestTransmitter,
		Receiver bestReceiver,
		Power minimumPower,
		PowerCoefficient turnCoefficient
	) :
		bestTransmitter(bestTransmitter),
		bestReceiver(bestReceiver),
		minimumPower(minimumPower),
		turnCoefficient(turnCoefficient)
	{ }
};

class BFSSignalSimulation : public SignalSimulation
{
private:
	struct Distortion
	{
		AbsorptionCoefficient absorption;
		ObstacleDistortion reflection;
	};

	struct Connection
	{
		Distance distance;
		PowerCoefficient powerCoefficient;

		Connection() :
			distance(std::numeric_limits<double>::max()),
			powerCoefficient(PowerCoefficient::in<PowerCoefficient::Unit::coefficient>(0))
		{ }
	};

	struct Bot
	{
		DiscretePoint position;
		int direction;
		Distance distance;

		Bot(DiscretePoint position, int direction, Distance distance) :
			position(position),
			direction(direction),
			distance(distance)
		{ }
	};

	const Frequency frequency;
	const BFSSignalSimulationParameters simulationParameters;

	const std::array<DiscreteDirection, 16> baseDirections{
		{
			DiscreteDirection(-2, -1),
			DiscreteDirection(-2, 1),

			DiscreteDirection(-1, -2),
			DiscreteDirection(-1, -1),
			DiscreteDirection(-1, 0),
			DiscreteDirection(-1, 1),
			DiscreteDirection(-1, 2),

			DiscreteDirection(0, -1),
			DiscreteDirection(0, 1),

			DiscreteDirection(1, -2),
			DiscreteDirection(1, -1),
			DiscreteDirection(1, 0),
			DiscreteDirection(1, 1),
			DiscreteDirection(1, 2),

			DiscreteDirection(2, -1),
			DiscreteDirection(2, 1),
		}
	};

	DiscreteDirection toBaseDirection(const FreeVector& vector) const
	{
		FreeVector normalized = vector.normalized();

		int direction = 0;
		double bestDotProduct = 0;

		for (int i = 0; i < baseDirections.size(); i++) {
			double newDotProduct = baseDirections[i] * normalized;

			if (newDotProduct > bestDotProduct)
			{
				bestDotProduct = newDotProduct;
				direction = i;
			}
		}

		return baseDirections[direction];
	}

	int toBaseDirectionIndex(const DiscreteDirection& direction) const
	{
		for (int i = 0; i < baseDirections.size(); i++)
			if (direction.x == baseDirections[i].x && direction.y == baseDirections[i].y)
				return i;
	}

	SignalSimulationSpaceDefinitionPtr simulationSpaceDefinition;
	SimulationUniformFiniteElementsSpace<std::array<Distortion, 16>> simulationSpace;

public:
	BFSSignalSimulation(SignalSimulationSpaceDefinitionPtr simulationSpaceDefinition, Frequency frequency, BFSSignalSimulationParameters simulationParameters) :
		frequency(frequency),
		simulationParameters(simulationParameters),
		simulationSpace(simulationSpaceDefinition->spaceSize, simulationSpaceDefinition->precision),
		simulationSpaceDefinition(simulationSpaceDefinition)
	{
		for (const auto& obstacle : simulationSpaceDefinition->obstacles)
		{
			for (int x = 0; x < simulationSpace.resolution.width; x++)
			{
				for (int y = 0; y < simulationSpace.resolution.height; y++)
				{
					DiscretePoint firstDiscretePosition(x, y);
					Position firstPosition = simulationSpace.getPosition(firstDiscretePosition);

					auto& element = simulationSpace.getElement(firstDiscretePosition);

					for (int i = 0; i < baseDirections.size(); i++)
					{
						DiscretePoint secondDiscretePosition = firstDiscretePosition + baseDirections[i];
						Position secondPosition = simulationSpace.getPosition(secondDiscretePosition);

						auto& connection = element[i];

						auto absorption = obstacle->absorption(firstPosition, secondPosition, frequency);
						connection.absorption = connection.absorption + absorption;

						auto distortion = obstacle->distortion(firstPosition, secondPosition, frequency);
						connection.reflection = connection.reflection + distortion;
					}
				}
			}
		}
	}

	virtual SignalMapPtr simulate(Position transmitterPosition) const
	{
		DiscretePoint transmitterDiscretePoint = simulationSpace.getDiscretePoint(transmitterPosition);

		auto signalMap = std::make_shared<SignalMap>(simulationSpace.surface, simulationSpace.precision);
		auto minimumCoefficient =
			simulationParameters.minimumPower /
			(simulationParameters.bestTransmitter.power *
				simulationParameters.bestTransmitter.antenaGain *
				simulationParameters.bestReceiver.antenaGain);

		std::vector<Bot> botsA;
		std::vector<Bot> botsB;

		SimulationUniformFiniteElementsSpace<std::array<Connection, 16>> connectionsMap(
			simulationSpace.surface,
			simulationSpace.precision
		);

		for (int x = 0; x < simulationSpace.resolution.width; x++)
		{
			for (int y = 0; y < simulationSpace.resolution.height; y++)
			{
				DiscretePoint inSightDiscretePosition(x, y);
				Position inSightPosition = simulationSpace.getPosition(inSightDiscretePosition);

				bool inSight = true;

				for (const auto& obstacle : simulationSpaceDefinition->obstacles)
				{
					inSight = inSight && obstacle->inSight(transmitterPosition, inSightPosition);
				}

				if (inSight)
				{
					DiscreteDirection direction = toBaseDirection(FreeVector(transmitterPosition.get<Distance::Unit::m>(), inSightPosition.get<Distance::Unit::m>()));
					int directionIndex = toBaseDirectionIndex(direction);

					Bot bot(
						inSightDiscretePosition,
						directionIndex,
						transmitterPosition.distanceTo(inSightPosition)
					);

					connectionsMap.getElement(inSightDiscretePosition)[directionIndex].powerCoefficient = PowerCoefficient::in<PowerCoefficient::Unit::coefficient>(1);

					botsA.push_back(bot);
				}
			}
		}

		while (botsA.size())
		{
			for (auto& bot : botsA)
			{
				DiscretePoint& botPosition = bot.position;
				Connection& botConnection = connectionsMap.getElement(botPosition)[bot.direction];

				PowerCoefficient powerCoefficient = botConnection.powerCoefficient * std::pow(frequency / (bot.distance * 4 * 3.141592653589793238463), 2);
				auto& signalMapElement = signalMap->getElement(botPosition);
				if (signalMapElement < powerCoefficient)
					signalMapElement = powerCoefficient;

				for (int i = 0; i < baseDirections.size(); i++)
				{
					const DiscreteDirection& direction = baseDirections[i];
					const DiscretePoint destinationPosition = botPosition + direction;

					if (!simulationSpace.inRange(destinationPosition))
						continue;

					Connection& destinationConnection = connectionsMap.getElement(destinationPosition)[i];

					double dotProduct = (FreeVector)direction * baseDirections[bot.direction];
					dotProduct += 1;
					dotProduct /= 2;
					dotProduct = 1 - dotProduct;

					PowerCoefficient turnCoefficient = PowerCoefficient::in<PowerCoefficient::Unit::dB>(
						simulationParameters.turnCoefficient.get<PowerCoefficient::Unit::dB>() * dotProduct
						);

					auto& connection = simulationSpace.getElement(bot.position)[i];

					Distance distance = simulationSpace.getPosition(botPosition).distanceTo(simulationSpace.getPosition(destinationPosition));
					PowerCoefficient newPowerCoefficient =
						botConnection.powerCoefficient *
						turnCoefficient *
						connection.absorption.get<AbsorptionCoefficient::Unit::coefficient>(distance);

					if (destinationConnection.powerCoefficient < newPowerCoefficient)
					{
						destinationConnection.powerCoefficient = newPowerCoefficient;

						botsB.push_back(Bot(
							destinationPosition,
							toBaseDirectionIndex(direction),
							bot.distance + distance
						));
					}
				}
			}

			botsA.clear();
			std::swap(botsA, botsB);
		}

		return signalMap;
	}
};