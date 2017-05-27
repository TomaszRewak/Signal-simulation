#pragma once

#include "Math.hpp"

struct Distance
{
protected:
	double m;

public:
	enum class Unit
	{
		m,
		cm,
		mm
	};

	Distance() :
		m(0)
	{ }

	explicit Distance(double m) :
		m(m)
	{ }

	template<Unit U>
	static Distance in(double value)
	{
		Distance d;
		d.set<U>(value);
		return d;
	}

	template<Unit U>
	double get() const = 0;
	template<>
	double get<Unit::m>() const { return m; }
	template<>
	double get<Unit::cm>() const { return m * 100; }
	template<>
	double get<Unit::mm>() const { return m * 1000; }

	template<Unit U>
	void set(double value) = 0;
	template<>
	void set<Unit::m>(double value) { m = value; }
	template<>
	void set<Unit::cm>(double value) { m = value / 100; }
	template<>
	void set<Unit::mm>(double value) { m = value / 1000; }

	friend Distance operator+(const Distance& a, const Distance& b) {
		return Distance(a.m + b.m);
	}

	friend Distance operator-(const Distance& a, const Distance& b) {
		return Distance(a.m - b.m);
	}

	friend double operator/(const Distance& a, const Distance& b) {
		return a.m / b.m;
	}

	friend Distance operator*(const Distance& distance, double by) {
		return Distance(distance.m * by);
	}

	friend bool operator<(const Distance& a, const Distance& b) {
		return a.m < b.m;
	}

	friend Distance abs(const Distance& distance)
	{
		return Distance(std::abs(distance.m));
	}
};

struct Position
{
protected:
	Point m;

public:
	Position()
	{ }

	explicit Position(Point m) :
		m(m)
	{ }

	Position(const Distance& x, const Distance& y)
	{
		m.x = x.get<Distance::Unit::m>();
		m.y = y.get<Distance::Unit::m>();
	}

	template<Distance::Unit U>
	static Position in(Point value)
	{
		Position p;
		p.set<U>(value);
		return p;
	}

	template<Distance::Unit U>
	Point get() const
	{
		return Point(
			Distance::in<Distance::Unit::m>(m.x).get<U>(),
			Distance::in<Distance::Unit::m>(m.y).get<U>()
		);
	}

	template<Distance::Unit U>
	void set(Point value)
	{
		m = Point(
			Distance::in<U>(value.x).get<Distance::Unit::m>(),
			Distance::in<U>(value.y).get<Distance::Unit::m>()
		);
	}

	Distance distanceTo(const Position& second) const
	{
		return Distance::in<Distance::Unit::m>(FreeVector(m, second.m).d());
	}

	friend Point operator/(const Position& position, const Distance& by) {
		return Point(
			position.m.x / by.get<Distance::Unit::m>(),
			position.m.y / by.get<Distance::Unit::m>()
		);
	}

	Distance x() const { return Distance::in<Distance::Unit::m>(m.x); }
	Distance y() const { return Distance::in<Distance::Unit::m>(m.y); }
};

struct Surface
{
protected:
	Rectangle m;

public:
	Surface()
	{}

	explicit Surface(Rectangle m) :
		m(m)
	{ }

	template<Distance::Unit U>
	static Surface in(Rectangle value)
	{
		Surface s;
		s.set<U>(value);
		return s;
	}

	template<Distance::Unit U>
	Rectangle get() const
	{
		return Rectangle(
			Distance::in<Distance::Unit::m>(m.minX()).get<U>(),
			Distance::in<Distance::Unit::m>(m.minY()).get<U>(),
			Distance::in<Distance::Unit::m>(m.maxX()).get<U>(),
			Distance::in<Distance::Unit::m>(m.maxY()).get<U>()
		);
	}

	template<Distance::Unit U>
	void set(Rectangle value)
	{
		m = Rectangle(
			Distance::in<U>(value.minX()).get<Distance::Unit::m>(),
			Distance::in<U>(value.minY()).get<Distance::Unit::m>(),
			Distance::in<U>(value.maxX()).get<Distance::Unit::m>(),
			Distance::in<U>(value.maxY()).get<Distance::Unit::m>()
		);
	}

	Distance minX() const { return Distance::in<Distance::Unit::m>(m.minX()); }
	Distance minY() const { return Distance::in<Distance::Unit::m>(m.minY()); }
	Distance maxX() const { return Distance::in<Distance::Unit::m>(m.maxX()); }
	Distance maxY() const { return Distance::in<Distance::Unit::m>(m.maxY()); }
};

struct Frequency {
protected:
	double meters;

public:
	enum class Unit {
		m,
		GHz
	};

	Frequency() :
		meters(0)
	{ }

	Frequency(double meters) :
		meters(meters)
	{ }

	template<Unit U>
	static Frequency in(double value)
	{
		Frequency f;
		f.set<U>(value);
		return f;
	}

	template<Unit U>
	double get() const = 0;
	template<>
	double get<Unit::m>() const { return meters; };
	template<>
	double get<Unit::GHz>() const { return 0.299792458 / meters; };

	template<Unit U>
	void set(double value) = 0;
	template<>
	void set<Unit::m>(double value) { meters = value; };
	template<>
	void set<Unit::GHz>(double value) { meters = 0.299792458 / value; };

	friend double operator/(const Frequency& frequency, const Distance& distance) {
		return frequency.meters / distance.get<Distance::Unit::m>();
	}
};

struct AbsorptionCoefficient {
private:
	double alpha;

public:
	enum class Unit
	{
		alpha,
		dB,
		coefficient
	};

	AbsorptionCoefficient() :
		alpha(0)
	{ }

	explicit AbsorptionCoefficient(double alpha) :
		alpha(alpha)
	{ }

	template<Unit U>
	static AbsorptionCoefficient in(double value, Distance thickness)
	{
		AbsorptionCoefficient c;
		c.set<U>(value, thickness);
		return c;
	}

	template<Unit U>
	double get(Distance thickness) const = 0;
	template<>
	double get<Unit::alpha>(Distance thickness) const { return alpha * thickness.get<Distance::Unit::m>(); }
	template<>
	double get<Unit::dB>(Distance thickness) const { return 10 * std::log10(get<Unit::coefficient>(thickness)); }
	template<>
	double get<Unit::coefficient>(Distance thickness) const { return std::exp(-get<Unit::alpha>(thickness)); }

	template<Unit T>
	void set(double value, Distance thickness) = 0;
	template<>
	void set<Unit::alpha>(double value, Distance thickness) { alpha = value / thickness.get<Distance::Unit::m>(); }
	template<>
	void set<Unit::dB>(double value, Distance thickness) { set<Unit::coefficient>(std::pow(10.0, value / 10.0), thickness); }
	template<>
	void set<Unit::coefficient>(double value, Distance thickness) { set<Unit::alpha>(-std::log(value), thickness); }

	bool affects() const { return alpha != 0; }

	AbsorptionCoefficient normalized()
	{
		return AbsorptionCoefficient(std::max(0., alpha));
	}

	friend AbsorptionCoefficient operator+(const AbsorptionCoefficient& a, const AbsorptionCoefficient& b)
	{
		return AbsorptionCoefficient(a.alpha + b.alpha);
	}

	friend AbsorptionCoefficient operator-(const AbsorptionCoefficient& a, const AbsorptionCoefficient& b)
	{
		return AbsorptionCoefficient(a.alpha - b.alpha);
	}

	friend AbsorptionCoefficient operator*(const AbsorptionCoefficient& a, double by)
	{
		return AbsorptionCoefficient(a.alpha * by);
	}
};

struct PowerCoefficient {
private:
	double coefficient;

public:
	enum class Unit
	{
		coefficient,
		dB,
		dBm
	};

	PowerCoefficient() :
		coefficient(0)
	{ }

	PowerCoefficient(double coefficient) :
		coefficient(coefficient)
	{ }

	template<Unit U>
	static PowerCoefficient in(double value)
	{
		PowerCoefficient c;
		c.set<U>(value);
		return c;
	}

	template<Unit U>
	double get() const = 0;
	template<>
	double get<Unit::coefficient>() const { return coefficient; };
	template<>
	double get<Unit::dB>() const { return 10 * std::log10(coefficient); };
	template<>
	double get<Unit::dBm>() const { return get<Unit::dB>() + 30; };

	template<Unit U>
	void set(double value) = 0;
	template<>
	void set<Unit::coefficient>(double value) { coefficient = value; }
	template<>
	void set<Unit::dB>(double value) { coefficient = std::pow(10.0, value / 10.0); }
	template<>
	void set<Unit::dBm>(double value) { set<Unit::dB>(value - 30); };

	bool operator<(const PowerCoefficient& second) const
	{
		return coefficient < second.coefficient;
	}

	friend PowerCoefficient operator*(const PowerCoefficient& a, const PowerCoefficient& b)
	{
		return PowerCoefficient(a.coefficient * b.coefficient);
	}
};

struct AntenaGain {
protected:
	double coefficient;

public:
	enum class Unit {
		coefficient,
		dBi,
		dBd
	};

	AntenaGain() :
		coefficient(1)
	{ }

	explicit AntenaGain(double coefficient) :
		coefficient(coefficient)
	{ }

	template<Unit U>
	static AntenaGain in(double value)
	{
		AntenaGain g;
		g.set<U>(value);
		return g;
	}

	template<Unit U>
	double get() const = 0;
	template<>
	double get<Unit::coefficient>() const { return coefficient; };
	template<>
	double get<Unit::dBi>() const { return 10 * std::log10(coefficient); };
	template<>
	double get<Unit::dBd>() const { return 10 * std::log10(coefficient / 1.64); };

	template<Unit U>
	void set(double value) = 0;
	template<>
	void set<Unit::coefficient>(double value) { coefficient = value; }
	template<>
	void set<Unit::dBi>(double value) { coefficient = std::pow(10, value / 10); }
	template<>
	void set<Unit::dBd>(double value) { coefficient = std::pow(10, value / 10) * 1.64; }

	operator PowerCoefficient() const
	{
		return PowerCoefficient(coefficient);
	}
};

struct Power {
private:
	double mW;

public:
	enum class Unit {
		W,
		mW,
		dBW,
		dBm
	};

	Power() :
		mW(0)
	{ }

	explicit Power(double mW):
		mW(mW)
	{ }

	template<Unit U>
	static Power in(double value)
	{
		Power power;
		power.set<U>(value);
		return power;
	}

	template<Unit U>
	void set(double value) = 0;
	template<>
	void set<Unit::mW>(double value) { mW = value; }
	template<>
	void set<Unit::W>(double value) { mW = value * 1000; }
	template<>
	void set<Unit::dBm>(double value) { mW = std::pow(10.0, value / 10.0); }
	template<>
	void set<Unit::dBW>(double value) { set<Unit::dBm>(value + 30); }
	
	template<Unit U>
	double get() = 0;
	template<>
	double get<Unit::mW>() { return mW; }
	template<>
	double get<Unit::W>() { return mW / 1000; }
	template<>
	double get<Unit::dBm>() { return 10 * std::log10(mW); }
	template<>
	double get<Unit::dBW>() { return get<Unit::dBm>() - 30; }

	friend Power operator*(const Power& power, const PowerCoefficient& coefficient)
	{
		return Power(power.mW * coefficient.get<PowerCoefficient::Unit::coefficient>());
	}

	friend PowerCoefficient operator/(const Power& a, const Power& b)
	{
		return PowerCoefficient(a.mW / b.mW);
	}
};