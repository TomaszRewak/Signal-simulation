#pragma once

#include "VoxelSpace.hpp"
#include "Obstacle.hpp"

struct SignalSimulationVoxel
{
	std::vector<int> obstacles;
	bool hasObstacle = false;

	double signalStrength = 0;
};

class SignalSimulationSpace : public VoxelSpace<SignalSimulationVoxel>
{
private:
	void initializeVoxel(int column, int row, int obstacleIndex, bool swap)
	{
		if (swap)
			std::swap(column, row);

		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				int c = column + i,
					r = row + j;

				if (inRange(c, r) && (getVoxel(c, r).obstacles.size() == 0 || *getVoxel(c, r).obstacles.rbegin() != obstacleIndex))
					getVoxel(c, r).obstacles.push_back(obstacleIndex);
			}
		}

		if (inRange(column, row))
			getVoxel(column, row).hasObstacle |= true;
	}

	void initializeObstacle(int index, Point a, Point b)
	{
		Rectangle box(a, b);
		Rectangle spaceSize = getSpaceSize();
		double voxelSize = getVoxelSize();

		double
			ax = (a.x - spaceSize.minX()) / voxelSize,
			ay = (a.y - spaceSize.minY()) / voxelSize,
			bx = (b.x - spaceSize.minX()) / voxelSize,
			by = (b.y - spaceSize.minY()) / voxelSize;

		bool swapXY = box.maxX() - box.minX() < box.maxY() - box.minY();

		if (swapXY)
		{
			std::swap(ax, ay);
			std::swap(bx, by);
		}

		if (ax > bx)
		{
			std::swap(ax, bx);
			std::swap(ay, by);
		}

		double dy = (by - ay) / (bx - ax);
		int x0 = (int)ax;
		double y0 = ay - ax * dy;

		int segments = (int)(std::ceil(bx) - std::floor(ax));

		for (int i = 0; i < segments; i++)
		{
			int x = x0 + i;
			int y = (x0 + 0.5 + i) * dy + y0;
			int yHalf = (x0 + 1 + i) * dy + y0;

			initializeVoxel(x, y, index, swapXY);
			initializeVoxel(x + (yHalf == y ? 1 : 0), yHalf, index, swapXY);
		}
	}

	void initializeObstacles(const std::vector<WallObstacle>& obstacles)
	{
		for (int i = 0; i < obstacles.size(); i++)
		{
			const WallObstacle& obstacle = obstacles[i];

			this->initializeObstacle(i, obstacle.wallLine.a, obstacle.wallLine.b);
		}
	}

public:
	SignalSimulationSpace(Rectangle spaceSize, double voxelSize, const std::vector<WallObstacle>& obstacles) :
		VoxelSpace(spaceSize, voxelSize)
	{
		this->initializeObstacles(obstacles);
	}
};