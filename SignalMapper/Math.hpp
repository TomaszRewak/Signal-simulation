#pragma once

#include <cmath>
#include <limits>
#include <algorithm>
#include <array>

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

	FreeVector operator-() const
	{
		return FreeVector(-dx, -dy);
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

	Vector(Point a, Point b) :
		point(a), freeVector(a, b)
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

	Rectangle(double x1, double y1, double x2, double y2) :
		center(Point((x1 + x2) / 2, (y1 + y2) / 2)),
		width(std::abs(x1 - x2)),
		height(std::abs(y1 - y2))
	{ }

	Rectangle(Point a, Point b) :
		Rectangle(a.x, a.y, b.x, b.y)
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

struct Intersection
{
	Point position;
	FreeVector normalVector;

	bool intersecting = false;
	bool inRange;

	double distance;

	bool operator<(const Intersection& second) const { return distance < second.distance; }
};

struct LineIntersection : public Intersection
{
public:
	LineIntersection(const Line& obstacle, const Vector& ray)
	{
		const double
			x1 = obstacle.a.x,
			x2 = obstacle.b.x,
			y1 = obstacle.a.y,
			y2 = obstacle.b.y,
			x1p = ray.point.x,
			x2p = ray.point.x + ray.freeVector.dx,
			y1p = ray.point.y,
			y2p = ray.point.y + ray.freeVector.dy;

		const double
			dx = x2 - x1,
			dy = y2 - y1,
			dxp = x2p - x1p,
			dyp = y2p - y1p;

		const double
			divider = dyp * dx - dxp * dy;

		const double
			t = ((x1p - x1) * dy - (y1p - y1) * dx) / divider;

		const double
			x = x1p + dxp * t,
			y = y1p + dyp * t;

		const bool
			withinLine = std::abs(dx) > std::abs(dy) ?
			x >= std::min(x1, x2) && x <= std::max(x1, x2) :
			y >= std::min(y1, y2) && y <= std::max(y1, y2);

		if (divider == 0 || t < 0 || !withinLine)
		{
			intersecting = false;
		}
		else
		{
			intersecting = true;
			inRange = t <= 1;
			position = Point(x, y);
			normalVector = obstacle.normalVector();
			distance = ray.freeVector.d() * t;
		}
	}
};

struct DiscreteDirection
{
	const int x;
	const int y;

	DiscreteDirection(int i) :
		x((i >> 1) == 0 ? (i % 2 ? 1 : -1) : 0),
		y((i >> 1) == 1 ? (i % 2 ? 1 : -1) : 0)
	{ }

	DiscreteDirection(int x, int y) :
		x(std::abs(x) > std::abs(y) ? x > 0 ? 1 : -1 : 0),
		y(std::abs(x) <= std::abs(y) ? y > 0 ? 1 : -1 : 0)
	{ }

	DiscreteDirection(const FreeVector& vector) :
		DiscreteDirection(vector.dx, vector.dy)
	{ }

	int getIndex() const
	{
		return x ? (0 << 1) | x == 1 : (1 << 1) | y == 1;
	}

	static const std::array<DiscreteDirection, 4> baseDirections()
	{
		return std::array<DiscreteDirection, 4> {
			{
				DiscreteDirection(0),
					DiscreteDirection(1),
					DiscreteDirection(2),
					DiscreteDirection(3)
			}
		};
	}

	operator FreeVector()
	{
		return FreeVector(x, y);
	}

	DiscreteDirection operator-() const
	{
		return DiscreteDirection(-x, -y);
	}
};

struct DiscretePoint
{
	int x;
	int y;

	DiscretePoint() : DiscretePoint(0, 0)
	{ }

	DiscretePoint(int x, int y) :
		x(x), y(y)
	{ }

	DiscretePoint operator+(const DiscreteDirection& direction) const {
		return DiscretePoint(this->x + direction.x, this->y + direction.y);
	}
};

struct DiscreteSize
{
	int width;
	int height;

	DiscreteSize(int width, int height) :
		width(width),
		height(height)
	{ }

	DiscreteSize(double width, double height, double unitLength) :
		width(width / unitLength + 2),
		height(height / unitLength + 2)
	{ }
};
