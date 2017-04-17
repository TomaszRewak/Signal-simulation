#pragma once

#include "SimulationSpace.hpp"
#include "SignalSimulationSpace.hpp"
#include "SignalMap.hpp"
#include "Transmitter.hpp"
#include "VoxelSpace.hpp"

#include <vector>

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
		Vector vector;
		int reflections = 0;
		double conductivity = 1;

		Ray(double strength, Vector vector, int reflections) :
			strength(strength),
			vector(vector),
			reflections(reflections)
		{ }
	};

	const SimulationSpace simulationSpace;
	const SignalSimulationParameters simulationParameters;

	void shootRays(VoxelSpace<SignalSimulationVoxel>& voxelSpace, Point transmitterPosition) const
	{
		std::vector<Ray> rays;

		for (int i = 0; i < simulationParameters.raysCount; i++)
		{
			double alpha = 0.123 + std::atan(1.) * 8 * i / simulationParameters.raysCount;

			Vector rayVector(
				transmitterPosition,
				FreeVector(std::sin(alpha), std::cos(alpha))
			);

			Ray ray(1, rayVector, simulationParameters.reflectionCount);

			rays.push_back(ray);
		}
		
		while (rays.size() > 0)
		{
			Ray ray = *rays.rbegin();
			rays.pop_back();

			if (!voxelSpace.inRange(ray.vector.point))
				continue;

			auto& voxel = voxelSpace.getVoxel(ray.vector.point);
			voxel.signalStrength = std::max(voxel.signalStrength, ray.strength);

			Point newPoint = ray.vector.point + ray.vector.freeVector.normalized() * voxelSpace.getVoxelSize() * 0.5;

			/*bool reflected = false;
			FreeVector reflectedVector;*/

			IntersectionPoint closestIntersection;
			double newConductivity = ray.conductivity;

			for (int obstacleIndex : voxel.obstacles)
			{
				const WallObstacle& obstacle = simulationSpace.obstacles[obstacleIndex];

				Line rayLine(ray.vector.point, newPoint);

				IntersectionPoint intersection(obstacle.wallLine, rayLine);

				if (intersection.inBounds() && intersection < closestIntersection)
				{
					closestIntersection = intersection;

					if (obstacle.wallLine.normalVector() * ray.vector.freeVector < 0) {
						newConductivity = 0.98;
					}
					else
					{
						newConductivity = 1;
					}
				}
			}

			Ray newRay = ray;

			newRay.vector.point = closestIntersection.inBounds() ? closestIntersection : newPoint;
			newRay.conductivity = newConductivity;
			newRay.strength *= newRay.conductivity * 0.995;
			rays.push_back(newRay);

			/*if (!voxelSpace.inRange(ray.vector.point))
			continue;

			auto& voxel = voxelSpace.getVoxel(ray.vector.point);
			voxel.signalStrength = std::max(voxel.signalStrength, ray.strength);

			Ray newRay = ray;
			newRay.vector.point = ray.vector.point + ray.vector.freeVector.normalized() * (voxelSpace.getVoxelSize() / 2);
			newRay.strength *= 0.99;

			if (!voxelSpace.inRange(newRay.vector.point))
			continue;

			auto& newVoxel = voxelSpace.getVoxel(newRay.vector.point);*/

			/*if (newVoxel.obstacles.size() > 0)
			{
			for (auto& obstacle : newVoxel.obstacles)
			newRay.strength *= (1 - obstacle.material.absorption);


			if (ray.reflections > 0)
			{
			Ray reflectedRay = ray;

			newRay.reflections--;
			reflectedRay.reflections--;

			for (auto& obstacle : newVoxel.obstacles)
			{
			reflectedRay.vector.freeVector = reflectedRay.vector.freeVector.reflectedBy(obstacle.normalVector);
			reflectedRay.strength *= obstacle.material.reflection;
			}

			rays.push_back(reflectedRay);
			}
			}

			rays.push_back(newRay);*/
		}
	}

public:
	SignalSimulation(SimulationSpace simulationSpace, SignalSimulationParameters simulationParameters) :
		simulationSpace(simulationSpace),
		simulationParameters(simulationParameters)
	{ }

	SignalMap simulate(Transmitter transmitter, Point transmitterPosition) const
	{
		SignalSimulationSpace voxelSpace(
			this->simulationSpace.boundingBox(),
			this->simulationParameters.voxelSize,
			this->simulationSpace.obstacles);

		this->shootRays(voxelSpace, transmitterPosition);

		return SignalMap(voxelSpace);
	}
};