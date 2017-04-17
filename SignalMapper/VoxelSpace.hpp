#pragma once

#include "Space.hpp"

#include <vector>
#include <math.h>
#include <algorithm>

template<typename T>
class VoxelSpace
{
private:
	std::vector<T> voxels;

	const Rectangle spaceSize;
	const double voxelSize;
	const int xResolution, yResolution;

public:
	VoxelSpace(Rectangle spaceSize, double voxelSize) :
		spaceSize(spaceSize),
		voxelSize(voxelSize),
		xResolution((int) (spaceSize.getWidth() / voxelSize + 1)),
		yResolution((int) (spaceSize.getHeight() / voxelSize + 1))
	{
		voxels = std::vector<T>(xResolution * yResolution);
	}

	Rectangle getSpaceSize() const { return this->spaceSize; }
	double getVoxelSize() const { return this->voxelSize; }

	T& getVoxel(int column, int row)
	{
		T& voxel = voxels[row * xResolution + column];

		return voxel;
	}

	T& getVoxel(Point point)
	{
		return getVoxel(getColumn(point), getRow(point));
	}

	bool inRange(int column, int row) const
	{
		return
			column >= 0 &&
			column < xResolution &&
			row >= 0 &&
			row < yResolution;
	}

	bool inRange(Point point) const
	{
		return inRange(getColumn(point), getRow(point));
	}

	Vector moveToNextVoxel(Vector vector) const
	{
		Vector newVector = vector;

		if (vector.freeVector.dx == 0 && vector.freeVector.dy == 0)
			return newVector;

		if (std::abs(vector.freeVector.dx) > std::abs(vector.freeVector.dy))
		{
			double newX = getVoxelCenter(
				getColumn(vector.point) + (vector.freeVector.dx > 0 ? 1 : -1),
				getRow(vector.point)
			).x;

			double dx = newX - vector.point.x;

			double newY = vector.point.y + vector.freeVector.dy / vector.freeVector.dx * dx;

			newVector.point = Point(newX, newY);
		}
		else
		{
			double newY = getVoxelCenter(
				getColumn(vector.point),
				getRow(vector.point) + (vector.freeVector.dy > 0 ? 1 : -1)
			).y;

			double dy = newY - vector.point.y;

			double newX = vector.point.x + vector.freeVector.dx / vector.freeVector.dy * dy;

			newVector.point = Point(newX, newY);
		}

		return newVector;
	}

	int getColumn(const Point& point) const { return (int)std::floor((point.x - spaceSize.minX()) / voxelSize); }
	int getRow(const Point& point) const { return (int)std::floor((point.y - spaceSize.minY()) / voxelSize); }

	Point getVoxelCenter(int column, int row) const {
		return Point(
			(column + 0.5) * voxelSize + spaceSize.minX(),
			(row + 0.5) * voxelSize + spaceSize.minY()
		);
	}
};