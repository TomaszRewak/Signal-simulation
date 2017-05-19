#pragma once

enum class PowerUnit {
	W,
	mW,
	dBW,
	dBm
};

struct Power {
private:
	double mW;

public:
	Power() :
		mW(0)
	{ }

	Power(double value, PowerUnit unit)
	{
		set(value, unit);
	}

	double get(PowerUnit unit) const
	{
		switch (unit)
		{
		case PowerUnit::mW:
			return mW;
		case PowerUnit::W:
			return get(PowerUnit::mW) / 1000;
		case PowerUnit::dBm:
			return 10 * std::log10(mW);
		case PowerUnit::dBW:
			return get(PowerUnit::dBm) - 30;
		}
	}

	void set(double value, PowerUnit unit)
	{
		switch (unit)
		{
		case PowerUnit::mW:
			mW = value;
			break;
		case PowerUnit::W:
			set(value / 1000, PowerUnit::mW);
			break;
		case PowerUnit::dBm:
			mW = std::pow(10.0, value / 10.0);
			break;
		case PowerUnit::dBW:
			set(value + 30, PowerUnit::dBm);
			break;
		}
	}
};

enum class FrequencyUnit {
	m,
	GHz
};

struct Frequency {
protected:
	double meters;

public:
	Frequency() :
		meters(0)
	{ }

	Frequency(double value, FrequencyUnit unit)
	{
		set(value, unit);
	}

	double get(FrequencyUnit unit) const
	{
		switch (unit)
		{
		case FrequencyUnit::m:
			return meters;
		case FrequencyUnit::GHz:
			return 0.299792458 / meters;
		}
	}

	void set(double value, FrequencyUnit unit)
	{
		switch (unit)
		{
		case FrequencyUnit::m:
			meters = value;
			break;
		case FrequencyUnit::GHz:
			meters = 0.299792458 / value;
			break;
		}
	}
};

enum class AntenaGainUnit {
	coefficient,
	dBi,
	dBd
};

struct AntenaGain {
protected:
	double coefficient;

public:
	AntenaGain() :
		coefficient(1)
	{ }

	AntenaGain(double value, AntenaGainUnit unit)
	{
		set(value, unit);
	}

	double get(AntenaGainUnit unit) const
	{
		switch (unit)
		{
		case AntenaGainUnit::coefficient:
			return coefficient;
		case AntenaGainUnit::dBi:
			return 10 * std::log10(coefficient);
		case AntenaGainUnit::dBd:
			return 10 * std::log10(coefficient / 1.64);
		}
	}

	void set(double value, AntenaGainUnit unit)
	{
		switch (unit)
		{
		case AntenaGainUnit::coefficient:
			coefficient = value;
			break;
		case AntenaGainUnit::dBi:
			coefficient = std::pow(10, value / 10);
			break;
		case AntenaGainUnit::dBd:
			coefficient = std::pow(10, value / 10) * 1.64;
			break;
		}
	}
};

enum class AbsorptionCoefficientUnit
{
	alpha,
	dB,
	coefficient
};

struct AbsorptionCoefficient {
private:
	double alpha;

public:
	AbsorptionCoefficient() :
		alpha(0)
	{ }

	AbsorptionCoefficient(double value, double thickness, AbsorptionCoefficientUnit unit)
	{
		set(value, thickness, unit);
	}

	double get(AbsorptionCoefficientUnit unit, double thickness) const
	{
		switch (unit)
		{
		case AbsorptionCoefficientUnit::alpha:
			return alpha * thickness;
		case AbsorptionCoefficientUnit::dB:
			return 10 * std::log10(get(AbsorptionCoefficientUnit::coefficient, thickness));
		case AbsorptionCoefficientUnit::coefficient:
			return std::exp(-get(AbsorptionCoefficientUnit::alpha, thickness));
		}
	}

	void set(double value, double thickness, AbsorptionCoefficientUnit unit)
	{
		switch (unit)
		{
		case AbsorptionCoefficientUnit::alpha:
			alpha = value / thickness;
			break;
		case AbsorptionCoefficientUnit::dB:
			set(std::pow(10.0, value / 10.0), thickness, AbsorptionCoefficientUnit::coefficient);
			break;
		case AbsorptionCoefficientUnit::coefficient:
			set(-std::log(value), thickness, AbsorptionCoefficientUnit::alpha);
			break;
		}
	}
};

enum class ReflectionCoefficientUnit 
{
	coefficient,
	dB
};

struct ReflectionCoefficient {
private:
	double coefficient;

public:
	ReflectionCoefficient() :
		coefficient(0)
	{ }

	ReflectionCoefficient(double value, ReflectionCoefficientUnit unit)
	{
		set(value, unit);
	}

	double get(ReflectionCoefficientUnit unit) const
	{
		switch (unit)
		{
		case ReflectionCoefficientUnit::coefficient:
			return coefficient;
		case ReflectionCoefficientUnit::dB:
			return 10 * std::log10(coefficient);
		}
	}

	void set(double value, ReflectionCoefficientUnit unit)
	{
		switch (unit)
		{
		case ReflectionCoefficientUnit::coefficient:
			coefficient = value;
			break;
		case ReflectionCoefficientUnit::dB:
			coefficient = std::pow(10.0, value / 10.0);
			break;
		}
	}
};