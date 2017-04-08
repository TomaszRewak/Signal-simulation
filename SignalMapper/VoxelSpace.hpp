#pragma once

#include "Space.hpp"

#include <vector>
#include <math.h>

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
		xResolution((int) spaceSize.getWidth() / voxelSize + 1),
		yResolution((int) spaceSize.getHeight() / voxelSize + 1)
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

	T& getVoxel(Point realPoint)
	{
		Point voxelPoint = toVoxelSpace(realPoint);

		return getVoxel((int)voxelPoint.x, (int)voxelPoint.y);
	}

	bool inRange(int column, int row) const
	{
		return
			column >= 0 &&
			column < xResolution &&
			row >= 0 &&
			row < yResolution;
	}

	bool inRange(Point realPoint) const
	{
		Point voxelPoint = toVoxelSpace(realPoint);

		return inRange((int)voxelPoint.x, (int)voxelPoint.y);
	}

	Point toVoxelSpace(Point point) const
	{
		return Point(
			(point.x - spaceSize.minX()) / voxelSize,
			(point.y - spaceSize.minY()) / voxelSize
		);
	}

	int getColumn(Point p) const { return (int)toVoxelSpace(p).x; }
	int getRow(Point p) const { return (int)toVoxelSpace(p).y; }
};