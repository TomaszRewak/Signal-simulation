#pragma once

#include "Space.hpp"
#include "Physics.hpp"
#include "VoxelSpace.hpp"

class WallObstacle
{
public:
	Material material;
	Line wallLine;

	WallObstacle(Point a, Point b, Material material) :
		wallLine(a, b), material(material)
	{ }

	Rectangle boundingBox() const
	{
		return Rectangle(wallLine.a, wallLine.b);
	}
};