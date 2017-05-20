#pragma once

#include "UniformFiniteElementsSpace.hpp"
#include "Obstacle.hpp"

#include <algorithm>
#include <memory>

struct SignalSimulationDistortion {
	FreeVector normalVector;
	double distance;
	double coefficient = 0;

	SignalSimulationDistortion() :
		coefficient(0)
	{};

	SignalSimulationDistortion(FreeVector normalVector, double distance, double coefficient) :
		normalVector(normalVector), distance(distance), coefficient(coefficient)
	{ }

	SignalSimulationDistortion operator+(const SignalSimulationDistortion& second) const
	{
		double secondCoefficient = (1 - coefficient) * second.coefficient;

		return SignalSimulationDistortion(
			(normalVector * coefficient + second.normalVector * secondCoefficient).normalized(),
			(distance * coefficient + second.distance * secondCoefficient) / (coefficient + secondCoefficient),
			coefficient + secondCoefficient
		);
	}
};

struct SignalSimulationConnection
{
	SignalSimulationDistortion distortion;
};

struct SignalSimulationElement
{
	double absorption;
	std::array<SignalSimulationConnection, 4> connections;
};

class SignalSimulationSpace : public UniformFiniteElementsSpace<SignalSimulationElement>
{
public:
	const Frequency frequency;

	SignalSimulationSpace(const Frequency frequency, const std::vector<ObstaclePtr>& obstacles, Rectangle spaceSize, Distance elementsDistance) :
		UniformFiniteElementsSpace(spaceSize, elementsDistance.get(Distance::Unit::m)),
		frequency(frequency)
	{
		const auto directions = DiscreteDirection::baseDirections();

		for (const auto& obstacle : obstacles)
		{
			for (int x = 0; x < resolution.width; x++)
			{
				for (int y = 0; y < resolution.height; y++)
				{
					DiscretePoint elementPosition(x, y);

					auto& element = getElement(elementPosition);
					element.absorption = obstacle->absorption(getPosition(elementPosition), frequency).get(
						AbsorptionCoefficient::Unit::coefficient, 
						elementsDistance);

					for (auto direction : DiscreteDirection::baseDirections())
					{
						DiscretePoint connectedElementPosition = elementPosition + direction;
						Vector ray(
							getPosition(elementPosition),
							getPosition(connectedElementPosition)
						);

						auto& connection = getElement(elementPosition).connections[direction.getIndex()];
						auto distortions = obstacle->distortion(ray, frequency);

						std::sort(distortions.begin(), distortions.end());

						for (const auto& distortion : distortions)
						{
							SignalSimulationDistortion signalDistortion(
								distortion.normalVector,
								distortion.distance,
								distortion.coefficient.get(ReflectionCoefficient::Unit::coefficient)
							);

							connection.distortion = connection.distortion + signalDistortion;
						}
					}
				}
			}
		}
	}

	bool hasObstacle(Point point) const
	{
		return getElement(point).absorption < 1;
	}
};
using SignalSimulationSpacePtr = std::shared_ptr<const SignalSimulationSpace>;