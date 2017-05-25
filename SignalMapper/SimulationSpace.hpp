#pragma once

#include "UniformFiniteElementsSpace.hpp"
#include "Obstacle.hpp"

#include <algorithm>
#include <memory>

template<typename Element>
class SimulationUniformFiniteElementsSpace : protected UniformFiniteElementsSpace<Element>
{
protected:
	const Surface surface;
	const Distance precision;

public:
	SimulationUniformFiniteElementsSpace(const Surface& surface, const Distance& precision) :
		UniformFiniteElementsSpace(
			DiscreteSize( 
				surface.get<Distance::Unit::m>().getWidth(), 
				surface.get<Distance::Unit::m>().getHeight(), 
				precision.get<Distance::Unit::m>())
		),
		surface(surface),
		precision(precision)
	{ }

	Element& getElement(const Position& position)
	{
		return getElement(getDiscretePoint(position));
	}

	const Element& getElement(const Position& position) const
	{
		return getElement(getDiscretePoint(position));
	}

	bool inRange(const Position& position) const
	{
		return inRange(getDiscretePoint(position));
	}

	Position getPosition(const DiscretePoint& discretePoint) const {
		return Position(
			surface.minX() + precision * discretePoint.x,
			surface.minY() + precision * discretePoint.y
		);
	}

	DiscretePoint getDiscretePoint(const Position& position) const {
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
	ConnectionsSpace(const Surface& surface, const Distance& precision) :
		SimulationUniformFiniteElementsSpace(surface, precision)
	{ }
};