//#pragma once
//
//#include <array>
//
//#include "VoxelSpace.hpp"
//#include "Space.hpp"
//#include "Obstacle.hpp"
//
//struct SignalWaveSimulationObstacle {
//	bool hasObstacle;
//	FreeVector normalVector;
//	double offset;
//	double absorption;
//	double reflection;
//
//	SignalWaveSimulationObstacle(
//		bool hasObstacle,
//		FreeVector normalVector,
//		double offset,
//		double absorption,
//		double reflection
//	) :
//		hasObstacle(hasObstacle),
//		normalVector(normalVector),
//		offset(offset),
//		absorption(absorption),
//		reflection(reflection)
//	{ }
//
//	SignalWaveSimulationObstacle() :
//		hasObstacle(false)
//	{ }
//};
//
//struct SignalWaveSimulationVoxel {
//	std::array<SignalWaveSimulationObstacle, 4> neighbours;
//};
//
//class SignalWaveSimulationSpace: public FiniteElementsSpace<SignalWaveSimulationSpace> {
//private:
//	void initializeObstacles(const std::vector<WallObstacle>& obstacles)
//	{
//
//	}
//
//public:
//	SignalWaveSimulationSpace(Rectangle spaceSize, double voxelSize, const std::vector<WallObstacle>& obstacles) :
//		FiniteElementsSpace(spaceSize, voxelSize)
//	{
//		this->initializeObstacles(obstacles);
//	}
//};