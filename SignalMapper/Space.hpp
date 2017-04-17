#pragma once

#include <cmath>
#include <limits>
#include <algorithm>

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

	FreeVector operator+(FreeVector v) const
	{
		return FreeVector(dx + v.dx, dy + v.dy);
	}

	FreeVector operator-(FreeVector v) const
	{
		return FreeVector(dx - v.dx, dy - v.dy);
	}

	double operator*(FreeVector vector) const
	{
		return dx * vector.dx + dy * vector.dy;
	}

	FreeVector reflectedBy(FreeVector normal) const
	{
		FreeVector d = *this;
		FreeVector n = normal.normalized();

		return d - n.normalized() * 2 * (d * n);
	}
};

inline Point operator+(const Point& point, const FreeVector& vector)
{
	return Point(point.x + vector.dx, point.y + vector.dy);
}

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

struct Line
{
	Point a, b;

	Line(Point a, Point b) :
		a(a), b(b)
	{ }

	FreeVector normalVector() const { return FreeVector(a, b).transposed().normalized(); }
};

struct IntersectionPoint
{
private:
	double x, y;
	double t;
	bool isInBounds;

public:
	IntersectionPoint() :
		t(std::numeric_limits<double>::max()),
		isInBounds(false)
	{ }

	IntersectionPoint(const Line& obstacle, const Line& ray)
	{
		const double
			x1 = obstacle.a.x,
			x2 = obstacle.b.x,
			y1 = obstacle.a.y,
			y2 = obstacle.b.y,
			x1p = ray.a.x,
			x2p = ray.b.x,
			y1p = ray.a.y,
			y2p = ray.b.y;

		const double
			dx = x2 - x1,
			dy = y2 - y1,
			dxp = x2p - x1p,
			dyp = y2p - y1p;

		const double
			divider = dyp * dx - dxp * dy;

		if (divider == 0) 
		{
			t = std::numeric_limits<double>::max();

			x = x2p;
			y = y2p;

			isInBounds = false;
		}
		else
		{
			t = ((x1p - x1) * dy - (y1p - y1) * dx) / divider;

			x = x1p + dxp * t;
			y = y1p + dyp * t;

			isInBounds =
				t > 0 &&
				t <= 1 &&
				(
					std::abs(dx) > std::abs(dy) ?
					x >= std::min(x1, x2) && x <= std::max(x1, x2) :
					y >= std::min(y1, y2) && y <= std::max(y1, y2)
				);
		}
	}

	bool inBounds() const { return isInBounds; }

	operator Point() const { return Point(x, y); }
	bool operator<(const IntersectionPoint& second) const { return t < second.t; }
};