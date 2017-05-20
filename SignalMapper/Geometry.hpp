#pragma once

#include "Math.hpp"

#include <vector>
#include <limits>
#include <memory>

struct SolidShape
{
	virtual bool contains(Point point) const = 0;
	virtual std::vector<Intersection> intersections(Vector ray) const = 0;
};
using SolidShapePtr = std::shared_ptr<const SolidShape>;

class CSGShape : public SolidShape
{

};

struct Polygon : public SolidShape
{
private:
	Rectangle bounds;
	std::vector<Point> points;

public:
	Polygon(const std::vector<Point>& points) :
		points(points)
	{
		this->points.push_back(points[0]);

		double
			minX = std::numeric_limits<double>::max(),
			minY = std::numeric_limits<double>::max(),
			maxX = std::numeric_limits<double>::min(),
			maxY = std::numeric_limits<double>::min();

		for (const auto& p : points)
		{
			minX = std::min(minX, p.x);
			minY = std::min(minY, p.y);
			maxX = std::max(minX, p.x);
			maxY = std::max(minY, p.y);
		}

		bounds = Rectangle(
			Point(minX, minY),
			Point(maxX, maxY)
		);
	}

	virtual bool contains(Point point) const
	{
		double alpha = 0.123;

		Vector ray(
			point,
			FreeVector(std::sin(alpha), std::cos(alpha))
		);

		int intersections = 0;

		for (int i = 0; i < points.size() - 1; i++)
		{
			Line wallLine(
				points[i],
				points[i + 1]
			);

			LineIntersection intersection(wallLine, ray);

			if (intersection.intersecting && intersection.distance > 0)
				intersections++;
		}

		return intersections % 2 == 1;
	}

	virtual std::vector<Intersection> intersections(Vector ray) const
	{
		std::vector<Intersection> intersections;

		for (int i = 0; i < points.size() - 1; i++)
		{
			Line wallLine(
				points[i],
				points[i + 1]
			);

			LineIntersection intersection(wallLine, ray);

			if (intersection.intersecting)
				intersections.push_back(intersection);
		}

		return intersections;
	}
};

struct CSGShapesDifference : public CSGShape
{
	const SolidShapePtr shapeA;
	const SolidShapePtr shapeB;

	CSGShapesDifference(SolidShapePtr shapeA, SolidShapePtr shapeB) :
		shapeA(shapeA), shapeB(shapeB)
	{ }

	virtual bool contains(Point point) const
	{
		return shapeA->contains(point) && !shapeB->contains(point);
	}

	virtual std::vector<Intersection> intersections(Vector ray) const
	{
		std::vector<Intersection> intersections;

		std::vector<Intersection> intersectionsA = shapeA->intersections(ray);
		std::vector<Intersection> intersectionsB = shapeB->intersections(ray);

		intersections.reserve(intersectionsA.size() + intersectionsB.size());

		for (Intersection& intersection : intersectionsA)
		{
			if (!shapeB->contains(intersection.position))
			{
				intersections.push_back(intersection);
			}
		}

		for (Intersection& intersection : intersectionsB)
		{
			if (shapeA->contains(intersection.position))
			{
				intersection.normalVector = -intersection.normalVector;
				intersections.push_back(intersection);
			}
		}

		return move(intersections);
	}
};