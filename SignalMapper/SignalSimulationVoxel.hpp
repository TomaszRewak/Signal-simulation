#pragma once

#include "Physics.hpp"
#include "Space.hpp"

#include <vector>

struct SignalSimulationVoxelObstacle
{
	Material material;
	FreeVector normalVector;

	SignalSimulationVoxelObstacle(Material material, FreeVector normalVector) :
		material(material),
		normalVector(normalVector)
	{ }
};

struct SignalSimulationVoxel
{
	std::vector<SignalSimulationVoxelObstacle> obstacles;

	double signalStrength;
};