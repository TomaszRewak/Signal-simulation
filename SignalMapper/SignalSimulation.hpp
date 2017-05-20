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
	Power minimalSignalStrength;

	SignalSimulationParameters(int raysCount = 100, int reflectionCount = 5, Power minimalSignalStrength = Power()) :
		raysCount(raysCount),
		reflectionCount(reflectionCount),
		minimalSignalStrength(minimalSignalStrength)
	{ }
};

class SignalSimulation
{
private:
	struct Ray
	{
		Point source;
		DiscretePoint position;
		FreeVector normalVector;
		FreeVector offset;

		int reflections = 0;
		int lastObstacle = -1;

		double absorption = 0;
		double distance = 0;

		Ray(Point source, DiscretePoint position, FreeVector normalVector, int reflections) :
			source(source),
			position(position),
			normalVector(normalVector.normalized()),
			reflections(reflections)
		{ }
	};

	const SignalSimulationSpacePtr simulationSpace;
	const SignalSimulationParameters simulationParameters;

public:
	SignalSimulation(SignalSimulationSpacePtr simulationSpace, SignalSimulationParameters simulationParameters = SignalSimulationParameters()) :
		simulationSpace(simulationSpace),
		simulationParameters(simulationParameters)
	{ }

	SignalMapPtr simulate(Point transmitterPosition) const
	{
		auto signalMap = std::make_shared<SignalMap>(simulationSpace);
		double minimalSignalStrength = simulationParameters.minimalSignalStrength.get(Power::Unit::W);
		double frequency = simulationSpace->frequency.get(Frequency::Unit::m);

		std::vector<Ray> rays;

		for (int i = 0; i < simulationParameters.raysCount; i++)
		{
			double alpha = 0.123 + std::atan(1.) * 8 * i / simulationParameters.raysCount;

			Ray ray(
				transmitterPosition,
				signalMap->getDiscretePosition(transmitterPosition),
				FreeVector(std::sin(alpha), std::cos(alpha)),
				simulationParameters.reflectionCount
			);

			rays.push_back(ray);
		}

		while (rays.size() > 0)
		{
			Ray ray = *rays.rbegin();
			rays.pop_back();

			if (!simulationSpace->inRange(ray.position))
				continue;

			double distance = ray.distance + FreeVector(ray.source, simulationSpace->getPosition(ray.position)).d();
			double strength = std::min(frequency / std::pow(4 * 3.141592653589793238463 * distance, 2), 1.) * std::pow(2.71828182846, -ray.absorption);

			if (strength < minimalSignalStrength)
				continue;

			auto& mapElement = signalMap->getElement(ray.position);
			mapElement.signalStrangth = std::max(mapElement.signalStrangth, strength);

			auto& simulationElement = simulationSpace->getElement(ray.position);

			FreeVector newOffset = ray.offset + ray.normalVector;
			DiscreteDirection direction = newOffset;

			auto& connection = simulationElement.connections[direction.getIndex()];

			if (connection.distortion.coefficient > 0)
				continue;

			ray.position = ray.position + newOffset;
			ray.offset = newOffset - direction;

			rays.push_back(ray);
		}

		return signalMap;
	}
};