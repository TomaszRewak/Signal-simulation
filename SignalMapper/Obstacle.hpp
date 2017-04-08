#pragma once

#include "Space.hpp"
#include "Physics.hpp"
#include "VoxelSpace.hpp"
#include "SignalSimulationVoxel.hpp"

class Obstacle
{
public:
	virtual void fill(VoxelSpace<SignalSimulationVoxel>& voxelSpace) = 0;
	virtual Rectangle boundingBox() const = 0;
};

class WallObstacle : public Obstacle
{
private:
	void fillVoxel(VoxelSpace<SignalSimulationVoxel>& voxelSpace, int x, int y, double factor)
	{
		if (!voxelSpace.inRange(x, y))
			return;

		SignalSimulationVoxel& voxel = voxelSpace.getVoxel(x, y);

		voxel.obstacles.push_back(SignalSimulationVoxelObstacle(
			this->material * factor,
			this->normalVector()
		));
	}

public:
	Material material;
	Point a, b;

	WallObstacle(Point a, Point b, Material material) :
		a(a), b(b), material(material)
	{ }

	void fill(VoxelSpace<SignalSimulationVoxel>& voxelSpace)
	{
		Rectangle box = this->boundingBox();

		Point voxelA = voxelSpace.toVoxelSpace(a);
		Point voxelB = voxelSpace.toVoxelSpace(b);

		double
			ax = voxelA.x,
			ay = voxelA.y,
			bx = voxelB.x,
			by = voxelB.y;

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
		double x0 = std::floor(ax) + 0.5;
		double y0 = ay - ax * dy;

		int segments = std::ceil(bx) - std::floor(ax);

		for (int i = 0; i < segments; i++)
		{
			double x = x0 + i;
			double y = x * dy + y0;

			double factor = 1;
			if (i == 0)
				factor -= 1 - std::floor(ax);
			if (i == segments - 1)
				factor -= std::ceil(bx) - 1;

			if (!swapXY)
				fillVoxel(voxelSpace, (int)x, (int)y, factor);
			else
				fillVoxel(voxelSpace, (int)y, (int)x, factor);
		}
	}

	FreeVector normalVector() { return FreeVector(a, b).transposed(); }

	Rectangle boundingBox() const
	{
		return Rectangle(a, b);
	}
};