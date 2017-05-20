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
private:
	static const Distance::Unit spaceUnit = Distance::Unit::m;

public:
	SimulationUniformFiniteElementsSpace(SimulationSpacePtr simulationSpace) :
		UniformFiniteElementsSpace(
			simulationSpace->spaceSize.get(spaceUnit),
			simulationSpace->precision.get(spaceUnit)
		)
	{ }

	Element& getElement(Position position)
	{
		return getElement(position.get(spaceUnit));
	}

	const Element& getElement(Position position) const
	{
		return getElement(position.get(spaceUnit));
	}

	bool inRange(Position position) const
	{
		return inRange(position.get(spaceUnit));
	}

	Position getPosition(DiscretePoint discretePoint) const {
		return Position(getPoint(discretePoint), spaceUnit);
	}

	Position getPosition(Point point) const {
		return Position(point, spaceUnit);
	}

	DiscretePoint getDiscretePoint(Position position) const {
		return getDiscretePoint(getPoint(position));
	}

	Point getPoint(Position position) const {
		return position.get(spaceUnit);
	}

	using UniformFiniteElementsSpace::getElement;
	using UniformFiniteElementsSpace::getDiscretePoint;
	using UniformFiniteElementsSpace::getPoint;
	using UniformFiniteElementsSpace::inRange;
};

template <typename T>
class ConnectionsSpace : public SimulationUniformFiniteElementsSpace<std::array<T, 4>>
{
public:
	ConnectionsSpace(SimulationSpacePtr simulationSpace) :
		SimulationUniformFiniteElementsSpace(simulationSpace)
	{ }
};