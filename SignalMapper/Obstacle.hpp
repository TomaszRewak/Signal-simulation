#pragma once

#include "Space.hpp"
#include "Physics.hpp"

class Obstacle
{
public:
	Material material;

	Obstacle(Material material) :
		material(material)
	{ }
};

class WallObstacle : public Obstacle
{
public:
	Point a, b;
	double width;

	WallObstacle(Point a, Point b, double width, Material material) :
		a(a), b(b), width(width), Obstacle(material)
	{ }
};