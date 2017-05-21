#pragma once

#include "UniformFiniteElementsSpace.hpp"
#include "Obstacle.hpp"
#include "SimulationSpace.hpp"

#include <algorithm>
#include <memory>

struct SignalSimulationDistortion
{
	AbsorptionCoefficient absorption;
	ObstacleDistortion reflection;

	SignalSimulationDistortion()
	{}
};

class SignalDistortionConnectionSpace : public ConnectionsSpace<SignalSimulationDistortion>
{
public:
	SignalDistortionConnectionSpace(
		Frequency frequency,
		SimulationSpacePtr simulationSpace
	) :
		ConnectionsSpace(simulationSpace)
	{
		const auto directions = DiscreteDirection::baseDirections();

		for (const auto& obstacle : simulationSpace->obstacles)
		{
			for (int x = 0; x < resolution.width; x++)
			{
				for (int y = 0; y < resolution.height; y++)
				{
					DiscretePoint firstDiscretePosition(x, y);
					Position firstPosition = getPosition(firstDiscretePosition);

					auto& element = getElement(firstDiscretePosition);

					for (int i = 0; i < directions.size(); i++)
					{
						DiscretePoint secondDiscretePosition = firstDiscretePosition + directions[i];
						Position secondPosition = getPosition(secondDiscretePosition);

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
};