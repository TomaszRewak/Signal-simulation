#pragma once

#include "Physics.hpp"
#include "SimulationSpace.hpp"
#include "SignalMap.hpp"

#include <vector>
#include <algorithm>

struct SignalSimulationSpaceDefinition
{
	std::vector<ObstaclePtr> obstacles;
	Surface spaceSize;
	Distance precision;

	SignalSimulationSpaceDefinition(std::vector<ObstaclePtr> obstacles, Surface spaceSize, Distance precision) :
		obstacles(obstacles),
		spaceSize(spaceSize),
		precision(precision)
	{ }
};
using SignalSimulationSpaceDefinitionPtr = std::shared_ptr<const SignalSimulationSpaceDefinition>;

class SignalSimulation
{
public:
	virtual SignalMapPtr simulate(Position transmitterPosition) const = 0;
};
using SignalSimulationPtr = std::shared_ptr<SignalSimulation const>;