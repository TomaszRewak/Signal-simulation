#pragma once

#include "Math.hpp"
#include "Geometry.hpp"
#include "Physics.hpp"
#include "UniformFiniteElementsSpace.hpp"

#include <memory>

struct ObstacleDistortion
{
	FreeVector normalVector;
	double distance;
	double coefficient;

	ObstacleDistortion() :
		coefficient(0)
	{ }

	ObstacleDistortion(const Intersection& intersection, double coefficient) :
		normalVector(intersection.normalVector),
		distance(intersection.distance),
		coefficient(coefficient)
	{ }

	bool operator<(const ObstacleDistortion& second) const {
		return distance < second.distance;
	}
};

class Obstacle 
{
public:
	virtual double absorption(Point point) const = 0;
	virtual std::vector<ObstacleDistortion> distortion(Vector vector) const = 0;
};
using ObstaclePtr = std::shared_ptr<const Obstacle>;

class UniformObstacle : public Obstacle
{
public:
	const ShapePtr shape;
	const Material material;

	UniformObstacle(ShapePtr shape, Material material) :
		shape(shape), material(material)
	{ }

	virtual double absorption(Point point) const
	{
		if (shape->contains(point))
			return material.absorption;
		else
			return 0;
	}

	virtual std::vector<ObstacleDistortion> distortion(Vector vector) const
	{
		std::vector<ObstacleDistortion> distortions;

		for (const auto& intersection : shape->intersections(vector))
			if (intersection.inRange && intersection.normalVector * vector.freeVector < 0)
				distortions.push_back(ObstacleDistortion(intersection, material.reflection));

		return distortions;
	}
};