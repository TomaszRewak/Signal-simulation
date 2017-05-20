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
	const Rectangle bounds;
	const double elementsDistance;
	const DiscreteSize resolution;

	UniformFiniteElementsSpace(Rectangle bounds, double elementsDistance) :
		bounds(bounds),
		elementsDistance(elementsDistance),
		resolution(bounds.getWidth(), bounds.getHeight(), elementsDistance)
	{
		elements = std::vector<Element>(resolution.width * resolution.height);
	}

	Element& getElement(DiscretePoint discretePoint)
	{
		return elements[discretePoint.y * resolution.width + discretePoint.x];
	}

	const Element& getElement(DiscretePoint discretePoint) const
	{
		return elements[discretePoint.y * resolution.width + discretePoint.x];
	}

	Element& getElement(Point point)
	{
		return getElement(getDiscretePosition(point));
	}

	const Element& getElement(Point point) const
	{
		return getElement(getDiscretePoint(point));
	}

	bool inRange(DiscretePoint point) const
	{
		return
			point.x >= 0 &&
			point.x < resolution.width &&
			point.y >= 0 &&
			point.y < resolution.height;
	}

	bool inRange(Point point) const
	{
		return inRange(getDiscretePoint(point));
	}

	Point getPoint(DiscretePoint discretePoint) const {
		return Point(
			discretePoint.x * elementsDistance + bounds.minX(),
			discretePoint.y * elementsDistance + bounds.minY()
		);
	}

	DiscretePoint getDiscretePoint(Point point) const {
		return DiscretePoint(
			(int)std::floor((point.x - bounds.minX()) / elementsDistance),
			(int)std::floor((point.y - bounds.minY()) / elementsDistance)
		);
	}
};