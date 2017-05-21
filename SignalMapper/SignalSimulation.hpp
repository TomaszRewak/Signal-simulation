#pragma once

#include "SignalSimulationSpace.hpp"
#include "SignalMap.hpp"
#include "Transmitter.hpp"
#include "UniformFiniteElementsSpace.hpp"

#include <vector>
#include <algorithm>

struct SignalSimulationParameters {
	int raysCount;
	int reflectionCount;

	SignalSimulationParameters(int raysCount = 100, int reflectionCount = 5) :
		raysCount(raysCount),
		reflectionCount(reflectionCount)
	{ }
};

class SignalSimulation
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
			powerCoefficient(1, PowerCoefficient::Unit::coefficient)
		{ }
	};

	const SimulationSpacePtr simulationSpace;
	const SignalSimulationParameters simulationParameters;

public:
	SignalSimulation(SimulationSpacePtr simulationSpace, SignalSimulationParameters simulationParameters = SignalSimulationParameters()) :
		simulationSpace(simulationSpace),
		simulationParameters(simulationParameters)
	{ }

	SignalMapPtr simulate(Frequency frequency, Position transmitterPosition) const
	{
		auto distortionMap = std::make_shared<SignalDistortionConnectionSpace>(frequency, simulationSpace);
		auto signalMap = std::make_shared<SignalMap>(simulationSpace);

		std::vector<Ray> rays;

		for (int i = 0; i < simulationParameters.raysCount; i++)
		{
			double alpha = 0.123 + std::atan(1.) * 8 * i / simulationParameters.raysCount;

			Ray ray(
				transmitterPosition,
				distortionMap->getDiscretePoint(transmitterPosition),
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

			auto& mapElement = signalMap->getElement(ray.position);
			if (mapElement < strength)
				mapElement = strength;

			auto& connections = distortionMap->getElement(ray.position);

			FreeVector newOffset = ray.offset + ray.normalVector;
			DiscreteDirection direction = newOffset;

			auto& connection = connections[direction.getIndex()];

			Distance distanceDiff = distance - ray.previousDistance;

			if (ray.reflections > 0 && connection.reflection.coefficient.get(PowerCoefficient::Unit::coefficient) != 0)
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

			if (connection.absorption.get(AbsorptionCoefficient::Unit::coefficient, simulationSpace->precision) != 1)
			{
				ray.powerCoefficient = ray.powerCoefficient * connection.absorption.get(AbsorptionCoefficient::Unit::coefficient, distanceDiff);
			}

			ray.position = ray.position + newOffset;
			ray.offset = newOffset - direction;

			ray.previousDistance = distance;

			rays.push_back(ray);
		}

		return signalMap;
	}
};