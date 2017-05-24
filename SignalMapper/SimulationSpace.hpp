#pragma once

#include "UniformFiniteElementsSpace.hpp"
#include "Obstacle.hpp"

#include <algorithm>
#include <memory>

struct SimulationSpace
{
	std::vector<ObstaclePtr> obstacles;
	Surface spaceSize;
	Distance precision;

	SimulationSpace(std::vector<ObstaclePtr> obstacles, Surface spaceSize, Distance precision) :
		obstacles(obstacles),
		spaceSize(spaceSize),
		precision(precision)
	{ }
};
using SimulationSpacePtr = std::shared_ptr<const SimulationSpace>;

template<typename Element>
class SimulationUniformFiniteElementsSpace : protected UniformFiniteElementsSpace<Element>
{
protected:
	const Surface surface;
	const Distance precision;

public:
	SimulationUniformFiniteElementsSpace(Surface surface, Distance precision) :
		UniformFiniteElementsSpace(
			DiscreteSize( 
				surface.get<Distance::Unit::m>().getWidth(), 
				surface.get<Distance::Unit::m>().getHeight(), 
				precision.get<Distance::Unit::m>())
		),
		surface(surface),
		precision(precision)
	{ }

	Element& getElement(Position position)
	{
		return getElement(getDiscretePoint(position));
	}

	const Element& getElement(Position position) const
	{
		return getElement(getDiscretePoint(position));
	}

	bool inRange(Position position) const
	{
		return inRange(getDiscretePoint(position));
	}

	Position getPosition(DiscretePoint discretePoint) const {
		return Position(
			surface.minX() + precision * discretePoint.x,
			surface.minY() + precision * discretePoint.y
		);
	}

	DiscretePoint getDiscretePoint(Position position) const {
		return DiscretePoint(
			(int)std::floor((position.x() - surface.minX()) / precision),
			(int)std::floor((position.y() - surface.minY()) / precision)
		);
		return getDiscretePoint(position);
	}

	using UniformFiniteElementsSpace::getElement;
	using UniformFiniteElementsSpace::inRange;
};

template <typename T>
class ConnectionsSpace : public SimulationUniformFiniteElementsSpace<std::array<T, 4>>
{
public:
	ConnectionsSpace(Surface surface, Distance precision) :
		SimulationUniformFiniteElementsSpace(surface, precision)
	{ }
};