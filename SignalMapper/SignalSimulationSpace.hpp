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
	SignalSimulationSpace(Rectangle spaceSize, double elementsDistance, const std::vector<ObstaclePtr>& obstacles) :
		UniformFiniteElementsSpace(spaceSize, elementsDistance)
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
					element.absorption = obstacle->absorption(getPosition(elementPosition)).get(PowerCoefficientUnit::coefficient);

					for (auto direction : DiscreteDirection::baseDirections())
					{
						DiscretePoint connectedElementPosition = elementPosition + direction;
						Vector ray(
							getPosition(elementPosition),
							getPosition(connectedElementPosition)
						);

						auto& connection = getElement(elementPosition).connections[direction.getIndex()];
						auto distortions = obstacle->distortion(ray);

						std::sort(distortions.begin(), distortions.end());

						for (const auto& distortion : distortions)
						{
							SignalSimulationDistortion signalDistortion(
								distortion.normalVector,
								distortion.distance,
								distortion.coefficient.get(PowerCoefficientUnit::coefficient)
							);

							connection.distortion = connection.distortion + signalDistortion;
						}
					}
				}
			}
		}
	}
};
using SignalSimulationSpacePtr = std::shared_ptr<const SignalSimulationSpace>;