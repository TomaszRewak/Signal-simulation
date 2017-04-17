#pragma once

#include "Obstacle.hpp"

#include <vector>
#include <memory>
#include <limits>
#include <algorithm>

class SignalSimulation;

class SimulationSpace
{
private:
	std::vector<WallObstacle> obstacles;
	
	bool useCustomSimulationArea = false;
	Rectangle customSimulationArea;

public:
	template<typename T>
	void addObstacle(const T& obstacle)
	{
		this->obstacles.push_back(obstacle);
	}

	void setCustomSimulationArea(Rectangle rectangle)
	{
		this->useCustomSimulationArea = true;
		this->customSimulationArea = rectangle;
	}

	Rectangle boundingBox() const
	{
		if (this->useCustomSimulationArea)
			return this->customSimulationArea;

		double
			minX = std::numeric_limits<double>::max(),
			maxX = std::numeric_limits<double>::min(),
			minY = std::numeric_limits<double>::max(),
			maxY = std::numeric_limits<double>::min();

		for (auto obstacle : this->obstacles)
		{
			auto obstacleBoundingBox = obstacle.boundingBox();

			minX = std::min(minX, obstacleBoundingBox.minX());
			maxX = std::max(maxX, obstacleBoundingBox.maxX());

			minY = std::min(minY, obstacleBoundingBox.minY());
			maxY = std::max(maxY, obstacleBoundingBox.maxY());
		}

		return Rectangle(Point(minX, minY), Point(maxX, maxY));
	}

	friend SignalSimulation;
};
