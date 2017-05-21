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
		coefficient(0, PowerCoefficient::Unit::coefficient)
	{ }

	ObstacleDistortion(const FreeVector& normalVector, const PowerCoefficient& coefficient) :
		normalVector(normalVector),
		coefficient(coefficient)
	{ }

	ObstacleDistortion operator+(const ObstacleDistortion& second) const {


		return ObstacleDistortion(
			(normalVector * coefficient.get(PowerCoefficient::Unit::coefficient) + second.normalVector * second.coefficient.get(PowerCoefficient::Unit::coefficient)).normalized(),
			std::max(coefficient, second.coefficient)
		);
	}
};

class Obstacle
{
public:
	virtual bool inside(Position position) const = 0;
	virtual AbsorptionCoefficient absorption(Position begin, Position end, Frequency frequency) const = 0;
	virtual ObstacleDistortion distortion(Position begin, Position end, Frequency frequency) const = 0;
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

	virtual AbsorptionCoefficient absorption(Position begin, Position end, Frequency frequency) const
	{
		AbsorptionCoefficient coefficient;
		AbsorptionCoefficient materialCoefficient = material->absorption(frequency);

		if (shape->contains(begin.get(spaceUnit)))
			coefficient = materialCoefficient;

		double distance = begin.distanceTo(end).get(spaceUnit);
		Vector vector(
			begin.get(spaceUnit),
			end.get(spaceUnit)
		);

		for (const auto& intersection : shape->intersections(vector))
			if (intersection.inRange && intersection.distance > 0)
			{
				if (intersection.inRange && intersection.normalVector * vector.freeVector < 0)
					coefficient = coefficient + materialCoefficient * (1 - intersection.distance / distance);
				else
					coefficient = coefficient - materialCoefficient * (1 - intersection.distance / distance);
			}

		return coefficient;
	}

	virtual ObstacleDistortion distortion(Position begin, Position end, Frequency frequency) const
	{
		Vector vector(
			begin.get(spaceUnit),
			end.get(spaceUnit)
		);

		ObstacleDistortion distortion;

		for (const auto& intersection : shape->intersections(vector))
			if (intersection.inRange && intersection.normalVector * vector.freeVector < 0)
				distortion = distortion + ObstacleDistortion(intersection.normalVector, material->reflection(frequency));

		return distortion;
	}
};