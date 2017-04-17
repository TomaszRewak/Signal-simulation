#pragma once

#include "VoxelSpace.hpp"
#include "SignalSimulationSpace.hpp"

#include <vector>

class SignalSimulation;

class SignalMap
{
private:
	VoxelSpace<SignalSimulationVoxel> voxelSpace;

	SignalMap(VoxelSpace<SignalSimulationVoxel> voxelSpace) :
		voxelSpace(voxelSpace)
	{ }

public:
	double getSignalStrength(Point p)
	{
		if (!voxelSpace.inRange(p))
			return 0;

		return voxelSpace.getVoxel(p).signalStrength;
	}

	double inRange(Point p) const 
	{
		int c = voxelSpace.getColumn(p),
			r = voxelSpace.getRow(p);

		return voxelSpace.inRange(c, r);
	}

	bool hasObstacle(Point p)
	{
		int c = voxelSpace.getColumn(p),
			r = voxelSpace.getRow(p);

		if (!voxelSpace.inRange(c, r))
			return false;

		return this->voxelSpace.getVoxel(c, r).hasObstacle;
	}

	friend SignalSimulation;
};