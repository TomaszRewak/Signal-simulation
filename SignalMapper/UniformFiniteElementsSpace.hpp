#pragma once

#include "Math.hpp"

#include <array>
#include <vector>
#include <math.h>
#include <algorithm>

template<typename Element>
class UniformFiniteElementsSpace
{
protected:
	std::vector<Element> elements;

public:
	const DiscreteSize resolution;

	UniformFiniteElementsSpace(const DiscreteSize& resolution) :
		resolution(resolution)
	{
		elements = std::vector<Element>(resolution.width * resolution.height);
	}

	Element& getElement(const DiscretePoint& discretePoint)
	{
		return elements[discretePoint.y * resolution.width + discretePoint.x];
	}

	const Element& getElement(const DiscretePoint& discretePoint) const
	{
		return elements[discretePoint.y * resolution.width + discretePoint.x];
	}

	bool inRange(const DiscretePoint& point) const
	{
		return
			point.x >= 0 &&
			point.x < resolution.width &&
			point.y >= 0 &&
			point.y < resolution.height;
	}
};