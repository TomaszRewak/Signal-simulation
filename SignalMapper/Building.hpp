#pragma once

#include "Obstacle.hpp"

#include <vector>
#include <memory>

class Building {
private:
	std::vector<std::shared_ptr<Obstacle>> obstacles;

public:

	template<typename T>
	void addObstacle(T& obstacle)
	{
		obstacles.push_back(std::make_shared<T>(obstacle));
	}
};
