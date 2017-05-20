#pragma once

#include "Math.hpp"
#include "Geometry.hpp"
#include "Material.hpp"
#include "UniformFiniteElementsSpace.hpp"

#include <memory>

struct ObstacleDistortion
{
	FreeVector normalVector;
	double distance;
	ReflectionCoefficient coefficient;

	ObstacleDistortion()
	{ }

	ObstacleDistortion(const Intersection& intersection, const ReflectionCoefficient& coefficient) :
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
	virtual bool inside(Position position) const = 0;
	virtual AbsorptionCoefficient absorption(Position position, Frequency frequency) const = 0;
	virtual std::vector<ObstacleDistortion> distortion(Position begin, Position end, Frequency frequency) const = 0;
};
using ObstaclePtr = std::shared_ptr<const Obstacle>;

class UniformObstacle : public Obstacle
{
public:
	const SolidShapePtr shape;
	const Distance::Unit spaceUnit;
	const MaterialPtr material;

	UniformObstacle(SolidShapePtr shape, Distance::Unit spaceUnit, MaterialPtr material) :
		shape(shape), spaceUnit(spaceUnit), material(material)
	{ }

	virtual bool inside(Position position) const
	{
		return shape->contains(position.get(spaceUnit));
	}

	virtual AbsorptionCoefficient absorption(Position position, Frequency frequency) const
	{
		if (shape->contains(position.get(spaceUnit)))
			return material->absorption(frequency);
		else
			return AbsorptionCoefficient();
	}

	virtual std::vector<ObstacleDistortion> distortion(Position begin, Position end, Frequency frequency) const
	{
		Vector vector(
			begin.get(spaceUnit),
			end.get(spaceUnit)
		);

		std::vector<ObstacleDistortion> distortions;

		for (const auto& intersection : shape->intersections(vector))
			if (intersection.inRange && intersection.normalVector * vector.freeVector < 0)
				distortions.push_back(ObstacleDistortion(intersection, material->reflection(frequency)));

		return distortions;
	}
};