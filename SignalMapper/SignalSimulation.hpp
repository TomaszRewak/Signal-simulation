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

		double absorptionCoefficient = 1;

		Ray(Position source, DiscretePoint position, FreeVector normalVector, int reflections) :
			source(source),
			position(position),
			normalVector(normalVector.normalized()),
			reflections(reflections)
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

			double distance = ray.distance.get(Distance::Unit::m) + FreeVector(ray.source.get(Distance::Unit::m), signalMap->getPosition(ray.position).get(Distance::Unit::m)).d();
			double strength = std::pow(frequency.get(Frequency::Unit::m) / (4 * 3.141592653589793238463 * distance), 2) * ray.absorptionCoefficient;

			strength = std::min(strength, 1.);

			//if (strength < simulationParameters.minimalSignalStrength.get(Power::Unit::W))
			//	continue;

			auto& mapElement = signalMap->getElement(ray.position);
			mapElement = std::max(mapElement, strength);

			auto& connections = distortionMap->getElement(ray.position);

			FreeVector newOffset = ray.offset + ray.normalVector;
			DiscreteDirection direction = newOffset;

			auto& connection = connections[direction.getIndex()];

			Distance distanceDiff(distance - ray.previousDistance.get(Distance::Unit::m), Distance::Unit::m);

			if (connection.absorption.get(AbsorptionCoefficient::Unit::coefficient, simulationSpace->precision) != 1)
			{
				ray.absorptionCoefficient *= connection.absorption.get(AbsorptionCoefficient::Unit::coefficient, distanceDiff);
			}

			ray.position = ray.position + newOffset;
			ray.offset = newOffset - direction;

			ray.previousDistance = Distance(distance, Distance::Unit::m);

			rays.push_back(ray);
		}

		return signalMap;
	}
};