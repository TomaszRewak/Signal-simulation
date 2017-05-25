#pragma once

#include "SignalSimulation.hpp"

class BuildingMap : protected SimulationUniformFiniteElementsSpace<int>
{
public:
	BuildingMap(SignalSimulationSpaceDefinitionPtr simulationSpace) :
		SimulationUniformFiniteElementsSpace(simulationSpace->spaceSize, simulationSpace->precision)
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
