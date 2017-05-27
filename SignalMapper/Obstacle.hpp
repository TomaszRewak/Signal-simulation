#pragma once

#include "Math.hpp"
#include "Geometry.hpp"
#include "Material.hpp"
#include "UniformFiniteElementsSpace.hpp"

#include <memory>

struct ObstacleDistortion
{
	FreeVector normalVector;
	PowerCoefficient coefficient;

	ObstacleDistortion() :
		coefficient(0)
	{ }

	ObstacleDistortion(const FreeVector& normalVector, const PowerCoefficient& coefficient) :
		normalVector(normalVector),
		coefficient(coefficient)
	{ }

	ObstacleDistortion operator+(const ObstacleDistortion& second) const {
		return ObstacleDistortion(
			(normalVector * coefficient.get<PowerCoefficient::Unit::coefficient>() + second.normalVector * second.coefficient.get<PowerCoefficient::Unit::coefficient>()).normalized(),
			std::max(coefficient, second.coefficient)
		);
	}
};

class Obstacle
{
public:
	virtual bool inside(Position position) const = 0;
	virtual bool inSight(Position begin, Position end) const = 0;
	virtual AbsorptionCoefficient absorption(Position begin, Position end, Frequency frequency) const = 0;
	virtual ObstacleDistortion distortion(Position begin, Position end, Frequency frequency) const = 0;
};
using ObstaclePtr = std::shared_ptr<const Obstacle>;

template<Distance::Unit U>
class UniformObstacle : public Obstacle
{
public:
	const SolidShapePtr shape;
	const MaterialPtr material;

	UniformObstacle(SolidShapePtr shape, MaterialPtr material) :
		shape(shape), material(material)
	{ }

	virtual bool inside(Position position) const
	{
		return shape->contains(position.get<U>());
	}

	virtual bool inSight(Position begin, Position end) const
	{
		if (shape->contains(begin.get<U>()))
			return false;

		bool inSight = true;

		Vector vector(
			begin.get<U>(),
			end.get<U>()
		);

		shape->intersections(vector, [&inSight](const Intersection& intersection) {
			if (intersection.inRange)
				inSight = false;
		});

		return inSight;
	}

	virtual AbsorptionCoefficient absorption(Position begin, Position end, Frequency frequency) const
	{
		AbsorptionCoefficient coefficient;
		AbsorptionCoefficient materialCoefficient = material->absorption(frequency);

		if (shape->contains(begin.get<U>()))
			coefficient = materialCoefficient;

		double distance = begin.distanceTo(end).get<U>();

		Vector vector(
			begin.get<U>(),
			end.get<U>()
		);

		shape->intersections(vector, [&coefficient, &vector, &materialCoefficient, distance](const Intersection& intersection) {
			if (intersection.inRange && intersection.distance > 0)
			{
				if (intersection.inRange && intersection.normalVector * vector.freeVector < 0)
					coefficient = coefficient + materialCoefficient * (1 - intersection.distance / distance);
				else
					coefficient = coefficient - materialCoefficient * (1 - intersection.distance / distance);
			}
		});

		return coefficient.normalized();
	}

	virtual ObstacleDistortion distortion(Position begin, Position end, Frequency frequency) const
	{
		Vector vector(
			begin.get<U>(),
			end.get<U>()
		);

		ObstacleDistortion distortion;
		PowerCoefficient materialCoefficient = material->reflection(frequency);

		shape->intersections(vector, [&distortion, &vector, &materialCoefficient](const Intersection& intersection) {
			if (intersection.inRange && intersection.normalVector * vector.freeVector < 0)
				distortion = distortion + ObstacleDistortion(intersection.normalVector, materialCoefficient);
		});

		return distortion;
	}
};