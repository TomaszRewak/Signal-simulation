#pragma once

#include "Physics.hpp"

#include <memory>

struct Material
{
	virtual PowerCoefficient reflection(Frequency frequency) const = 0;
	virtual AbsorptionCoefficient absorption(Frequency frequency) const = 0;
};
using MaterialPtr = std::shared_ptr<const Material>;

struct UniformMaterial: public Material
{
	const PowerCoefficient reflectionValue;
	const AbsorptionCoefficient absorptionValue;

	UniformMaterial(PowerCoefficient reflectionValue, AbsorptionCoefficient absorptionValue) :
		reflectionValue(reflectionValue),
		absorptionValue(absorptionValue)
	{ }

	virtual PowerCoefficient reflection(Frequency frequency) const
	{
		return reflectionValue;
	}

	virtual AbsorptionCoefficient absorption(Frequency frequency) const
	{
		return absorptionValue;
	}
};