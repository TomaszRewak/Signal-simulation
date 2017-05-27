#pragma once

#include "SignalSimulation.hpp"

#include <vector>
#include <algorithm>

struct RaycastingSignalSimulationParameters {
	int raysCount;
	int reflectionCount;
	Transmitter bestTransmitter;
	Receiver bestReceiver;
	Power minimumPower;

	RaycastingSignalSimulationParameters(
		int raysCount,
		int reflectionCount,
		Transmitter bestTransmitter,
		Receiver bestReceiver,
		Power minimumPower
	) :
		raysCount(raysCount),
		reflectionCount(reflectionCount),
		bestTransmitter(bestTransmitter),
		bestReceiver(bestReceiver),
		minimumPower(minimumPower)
	{ }
};

class RaycastingSignalSimulation : public SignalSimulation
{
private:
	struct Distortion
	{
		AbsorptionCoefficient absorption;
		ObstacleDistortion reflection;
	};

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
	const RaycastingSignalSimulationParameters simulationParameters;

	const std::array<DiscreteDirection, 4> baseDirections{
		{
			DiscreteDirection(1, 0),
				DiscreteDirection(-1, 0),
				DiscreteDirection(0, 1),
				DiscreteDirection(0, -1)
		}
	};

	DiscreteDirection toBaseDirection(const FreeVector& vector) const
	{
		return DiscreteDirection(
			std::abs(vector.dx) > std::abs(vector.dy) ? vector.dx > 0 ? 1 : -1 : 0,
			std::abs(vector.dx) <= std::abs(vector.dy) ? vector.dy > 0 ? 1 : -1 : 0
		);
	}

	int toBaseDirectionIndex(const DiscreteDirection& direction) const
	{
		if (direction.x)
			return direction.x > 0 ? 0 : 1;
		else
			return direction.y > 0 ? 2 : 3;
	}

	SimulationUniformFiniteElementsSpace<std::array<Distortion, 4>> simulationSpace;

public:
	RaycastingSignalSimulation(SignalSimulationSpaceDefinitionPtr simulationSpaceDefinition, Frequency frequency, RaycastingSignalSimulationParameters simulationParameters) :
		frequency(frequency),
		simulationParameters(simulationParameters),
		simulationSpace(simulationSpaceDefinition->spaceSize, simulationSpaceDefinition->precision)
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
		auto signalMap = std::make_shared<SignalMap>(simulationSpace.surface, simulationSpace.precision);
		auto minimumCoefficient =
			simulationParameters.minimumPower /
			(simulationParameters.bestTransmitter.power *
				simulationParameters.bestTransmitter.antenaGain *
				simulationParameters.bestReceiver.antenaGain);

		std::vector<Ray> rays;

		for (int i = 0; i < simulationParameters.raysCount; i++)
		{
			double alpha = 0.123 + std::atan(1.) * 8 * i / simulationParameters.raysCount;

			Ray ray(
				transmitterPosition,
				simulationSpace.getDiscretePoint(transmitterPosition),
				FreeVector(std::sin(alpha), std::cos(alpha)),
				simulationParameters.reflectionCount
			);

			rays.push_back(ray);
		}

		while (rays.size() > 0)
		{
			Ray ray = *rays.rbegin();
			rays.pop_back();

			if (!signalMap->inRange(ray.position))
				continue;

			Distance distance = ray.distance + ray.source.distanceTo(signalMap->getPosition(ray.position));
			PowerCoefficient strength = ray.powerCoefficient * std::pow(frequency / (distance * 4 * 3.141592653589793238463), 2);

			if (strength < minimumCoefficient)
				continue;

			auto& mapElement = signalMap->getElement(ray.position);
			if (mapElement < strength)
				mapElement = strength;

			auto& connections = simulationSpace.getElement(ray.position);

			FreeVector newOffset = ray.offset + ray.normalVector;
			DiscreteDirection direction = toBaseDirection(newOffset);

			auto& connection = connections[toBaseDirectionIndex(direction)];

			Distance distanceDiff = distance - ray.previousDistance;

			if (ray.reflections > 0 && connection.reflection.coefficient.get<PowerCoefficient::Unit::coefficient>() != 0)
			{
				Ray reflectedRay = ray;
				reflectedRay.reflections--;
				reflectedRay.normalVector = ray.normalVector.reflectedBy(connection.reflection.normalVector).normalized();
				reflectedRay.offset = reflectedRay.offset.reflectedBy(connection.reflection.normalVector);
				reflectedRay.powerCoefficient = ray.powerCoefficient * connection.reflection.coefficient;
				reflectedRay.distance = reflectedRay.distance + distance;
				reflectedRay.previousDistance = Distance();
				reflectedRay.source = signalMap->getPosition(reflectedRay.position);
				rays.push_back(reflectedRay);
			}

			if (connection.absorption.get<AbsorptionCoefficient::Unit::coefficient>(distanceDiff) != 1)
			{
				ray.powerCoefficient = ray.powerCoefficient * connection.absorption.get<AbsorptionCoefficient::Unit::coefficient>(distanceDiff);
			}

			ray.position = ray.position + toBaseDirection(newOffset);
			ray.offset = newOffset - direction;

			ray.previousDistance = distance;

			rays.push_back(ray);
		}

		return signalMap;
	}
};