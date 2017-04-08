#pragma once

#include "SimulationSpace.hpp"
#include "SignalSimulationVoxel.hpp"
#include "SignalMap.hpp"
#include "Transmitter.hpp"
#include "VoxelSpace.hpp"



struct SignalSimulationParameters {
	double voxelSize;
	int raysCount;
	int reflectionCount;

	SignalSimulationParameters(double voxelSize, int raysCount = 100, int reflectionCount = 5) :
		voxelSize(voxelSize),
		raysCount(raysCount),
		reflectionCount(reflectionCount)
	{ }
};

class SignalSimulation
{
private:
	struct Ray
	{
		double strength;
		Vector location;

		Ray(double strength, Vector location) :
			strength(strength),
			location(location)
		{ }
	};

	SimulationSpace simulationSpace;
	SignalSimulationParameters simulationParameters;

	void addObstacles(VoxelSpace<SignalSimulationVoxel>& voxelSpace) const
	{
		for (auto obstacle : this->simulationSpace.obstacles)
		{
			obstacle->fill(voxelSpace);
		}
	}

	void shootRays(VoxelSpace<SignalSimulationVoxel>& voxelSpace, Point transmitterPosition) const
	{
		for (int i = 0; i < simulationParameters.raysCount; i++)
		{
			double alpha = std::atan(1.) * 8 * i / simulationParameters.raysCount;

			Vector rayVector(
				transmitterPosition,
				FreeVector(std::sin(alpha), std::cos(alpha))
			);

			Ray ray(0.5, rayVector);

			this->shootRay(voxelSpace, ray);
		}
	}

	void shootRay(VoxelSpace<SignalSimulationVoxel>& voxelSpace, Ray ray) const
	{
		while (voxelSpace.inRange(ray.location.point))
		{
			auto& voxel = voxelSpace.getVoxel(ray.location.point);
			voxel.signalStrength = 1;

			double d = std::max(std::abs(ray.location.freeVector.dx), std::abs(ray.location.freeVector.dy));

			Point newPoint(
				ray.location.point.x + voxelSpace.getVoxelSize() * ray.location.freeVector.dx / d,
				ray.location.point.y + voxelSpace.getVoxelSize() * ray.location.freeVector.dy / d
			);

			ray.location.point = newPoint;

			if (voxel.obstacles.size() > 0)
				break;
		}
	}

public:
	SignalSimulation(SimulationSpace simulationSpace, SignalSimulationParameters simulationParameters) :
		simulationSpace(simulationSpace),
		simulationParameters(simulationParameters)
	{

	}

	SignalMap simulate(Transmitter transmitter, Point transmitterPosition) const
	{
		VoxelSpace<SignalSimulationVoxel> voxelSpace(
			this->simulationSpace.boundingBox(),
			this->simulationParameters.voxelSize);

		this->addObstacles(voxelSpace);
		this->shootRays(voxelSpace, transmitterPosition);

		return SignalMap(voxelSpace);
	}
};