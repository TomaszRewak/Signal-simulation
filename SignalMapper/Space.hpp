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
};

struct Vector : public Point, public FreeVector
{
	Vector(double x, double y, double dx, double dy) :
		Point(x, y), FreeVector(dx, dy)
	{ }
};

struct Rectangle
{
private:
	Point _center;
	double _width, _height;

public:
	Rectangle()
	{ }

	Rectangle(Point a, Point b) :
		_center(Point((a.x + b.x) / 2, (a.y + b.y) / 2)),
		_width(std::abs(a.x - b.x)),
		_height(std::abs(a.y - b.y))
	{ }

	Rectangle(Point center, double width, double height) :
		_center(center),
		_width(width),
		_height(height)
	{ }

	Point center() { return _center; }
	double width() { return _width; }
	double height() { return _height; }
};