#pragma once

#include <cmath>

struct Point
{
	double x;
	double y;

	Point() : Point(0, 0)
	{ }

	Point(double x, double y) :
		x(x), y(y)
	{ }
};

struct FreeVector
{
	double dx;
	double dy;

	FreeVector() : FreeVector(0, 0)
	{ }

	FreeVector(double dx, double dy) :
		dx(dx), dy(dy)
	{ }

	FreeVector(Point a, Point b) :
		dx(b.x - a.x),
		dy(b.y - a.y)
	{ }

	double d() const { return std::sqrt(dx*dx + dy*dy); }

	FreeVector normalized() const
	{
		double length = d();
		return FreeVector(dx / length, dy / length);
	}

	FreeVector transposed() const
	{
		return FreeVector(dy, -dx);
	}

	FreeVector operator*(double by) const 
	{
		return FreeVector(dx * by, dy * by);
	}
};

struct Vector
{
	Point point;
	FreeVector freeVector;

	Vector(double x, double y, double dx, double dy) :
		point(x, y), freeVector(dx, dy)
	{ }

	Vector(Point p, FreeVector v) :
		point(p), freeVector(v)
	{ }
};

struct Rectangle
{
private:
	Point center;
	double width, height;

public:
	Rectangle()
	{ }

	Rectangle(Point a, Point b) :
		center(Point((a.x + b.x) / 2, (a.y + b.y) / 2)),
		width(std::abs(a.x - b.x)),
		height(std::abs(a.y - b.y))
	{ }

	Rectangle(Point center, double width, double height) :
		center(center),
		width(width),
		height(height)
	{ }

	Point getCenter() const { return center; }
	double getWidth() const { return width; }
	double getHeight() const { return height; }
	double minX() const { return center.x - width / 2; }
	double maxX() const { return center.x + width / 2; }
	double minY() const { return center.y - height / 2; }
	double maxY() const { return center.y + height / 2; }

	void setWidth(double width) { this->width = width; }
	void setHeight(double height) { this->height = height; }
};