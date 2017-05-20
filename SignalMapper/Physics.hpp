#pragma once

struct Distance 
{
protected:
	double m;

public:
	enum class Unit
	{
		m,
		mm
	};

	Distance() :
		m(0)
	{ }

	Distance(double value, Unit unit)
	{
		set(value, unit);
	}

	double get(Unit unit)
	{
		switch (unit)
		{
		case Distance::Unit::m:
			return m;
		case Distance::Unit::mm:
			return m * 1000;
		}
	}

	void set(double value, Unit unit)
	{
		switch (unit)
		{
		case Distance::Unit::m:
			m = value;
			break;
		case Distance::Unit::mm:
			m = value / 1000;
			break;
		}
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

	Power(double value, Unit unit)
	{
		set(value, unit);
	}

	double get(Unit unit) const
	{
		switch (unit)
		{
		case Unit::mW:
			return mW;
		case Unit::W:
			return get(Unit::mW) / 1000;
		case Unit::dBm:
			return 10 * std::log10(mW);
		case Unit::dBW:
			return get(Unit::dBm) - 30;
		}
	}

	void set(double value, Unit unit)
	{
		switch (unit)
		{
		case Unit::mW:
			mW = value;
			break;
		case Unit::W:
			set(value / 1000, Unit::mW);
			break;
		case Unit::dBm:
			mW = std::pow(10.0, value / 10.0);
			break;
		case Unit::dBW:
			set(value + 30, Unit::dBm);
			break;
		}
	}
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

	Frequency(double value, Unit unit)
	{
		set(value, unit);
	}

	double get(Unit unit) const
	{
		switch (unit)
		{
		case Unit::m:
			return meters;
		case Unit::GHz:
			return 0.299792458 / meters;
		}
	}

	void set(double value, Unit unit)
	{
		switch (unit)
		{
		case Unit::m:
			meters = value;
			break;
		case Unit::GHz:
			meters = 0.299792458 / value;
			break;
		}
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

	AntenaGain(double value, Unit unit)
	{
		set(value, unit);
	}

	double get(Unit unit) const
	{
		switch (unit)
		{
		case Unit::coefficient:
			return coefficient;
		case Unit::dBi:
			return 10 * std::log10(coefficient);
		case Unit::dBd:
			return 10 * std::log10(coefficient / 1.64);
		}
	}

	void set(double value, Unit unit)
	{
		switch (unit)
		{
		case Unit::coefficient:
			coefficient = value;
			break;
		case Unit::dBi:
			coefficient = std::pow(10, value / 10);
			break;
		case Unit::dBd:
			coefficient = std::pow(10, value / 10) * 1.64;
			break;
		}
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

	AbsorptionCoefficient(double value, Unit unit, Distance thickness)
	{
		set(value, unit, thickness);
	}

	double get(Unit unit, Distance thickness) const
	{
		switch (unit)
		{
		case Unit::alpha:
			return alpha * thickness.get(Distance::Unit::m);
		case Unit::dB:
			return 10 * std::log10(get(Unit::coefficient, thickness));
		case Unit::coefficient:
			return std::exp(-get(Unit::alpha, thickness));
		}
	}

	void set(double value, Unit unit, Distance thickness)
	{
		switch (unit)
		{
		case Unit::alpha:
			alpha = value / thickness.get(Distance::Unit::m);
			break;
		case Unit::dB:
			set(std::pow(10.0, value / 10.0), Unit::coefficient, thickness);
			break;
		case Unit::coefficient:
			set(-std::log(value), Unit::alpha, thickness);
			break;
		}
	}
};

struct ReflectionCoefficient {
private:
	double coefficient;

public:
	enum class Unit
	{
		coefficient,
		dB
	};

	ReflectionCoefficient() :
		coefficient(0)
	{ }

	ReflectionCoefficient(double value, Unit unit)
	{
		set(value, unit);
	}

	double get(Unit unit) const
	{
		switch (unit)
		{
		case Unit::coefficient:
			return coefficient;
		case Unit::dB:
			return 10 * std::log10(coefficient);
		}
	}

	void set(double value, Unit unit)
	{
		switch (unit)
		{
		case Unit::coefficient:
			coefficient = value;
			break;
		case Unit::dB:
			coefficient = std::pow(10.0, value / 10.0);
			break;
		}
	}
};