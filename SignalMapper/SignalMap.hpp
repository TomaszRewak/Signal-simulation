#pragma once

#include <vector>

class Simulation;
class SignalMap
{
private:
	std::vector<double> map;
	double voxelSize;
	int mapWidth;
	Rectangle area;

	SignalMap(std::vector<double> map, double voxelSize, int mapWidth, Rectangle area) :
		map(map), voxelSize(voxelSize), mapWidth(mapWidth), area(area)
	{ }

public:
	double getSignalStrength(double x, double y)
	{
		return 0;
	}

	friend Simulation;
};