#pragma once

#include "Math.hpp"

#include <vector>
#include <limits>
#include <memory>
#include <functional>

struct SolidShape
{
	virtual bool contains(Point point) const = 0;
	virtual void intersections(Vector ray, std::function<void(const Intersection&)>&& callback) const = 0;
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

	virtual void intersections(Vector ray, std::function<void(const Intersection&)>&& callback) const
	{
		for (int i = 0; i < points.size() - 1; i++)
		{
			Line wallLine(
				points[i],
				points[i + 1]
			);

			LineIntersection intersection(wallLine, ray);

			if (intersection.intersecting)
				callback(intersection);
		}
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

	virtual void intersections(Vector ray, std::function<void(const Intersection&)>&& callback) const
	{
		std::vector<Intersection> intersections;

		shapeA->intersections(
			ray,
			[this, &callback](const Intersection& intersection)
		{
			if (!this->shapeB->contains(intersection.position))
				callback(intersection);
		});

		shapeB->intersections(
			ray,
			[this, &callback](const Intersection& intersection)
		{
			if (this->shapeA->contains(intersection.position))
				callback(-intersection);
		});
	}
};