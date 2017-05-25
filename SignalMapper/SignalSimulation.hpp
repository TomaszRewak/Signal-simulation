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

template<typename Element>
class SignalSimulation : public ConnectionsSpace<Element>
{
protected:
	const SignalSimulationSpaceDefinitionPtr simulationSpace;

public:
	SignalSimulation(SignalSimulationSpaceDefinitionPtr simulationSpace) :
		ConnectionsSpace(simulationSpace->spaceSize, simulationSpace->precision),
		simulationSpace(simulationSpace)
	{ }

	virtual SignalMapPtr simulate(Position transmitterPosition) const = 0;
};