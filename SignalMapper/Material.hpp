#pragma once

#include "Physics.hpp"

#include <memory>

struct Material
{
	virtual ReflectionCoefficient reflection(Frequency frequency) const = 0;
	virtual AbsorptionCoefficient absorption(Frequency frequency) const = 0;
};
using MaterialPtr = std::shared_ptr<const Material>;

struct UniformMaterial: public Material
{
	const ReflectionCoefficient reflectionValue;
	const AbsorptionCoefficient absorptionValue;

	UniformMaterial(ReflectionCoefficient reflectionValue, AbsorptionCoefficient absorptionValue) :
		reflectionValue(reflectionValue),
		absorptionValue(absorptionValue)
	{ }

	virtual ReflectionCoefficient reflection(Frequency frequency) const
	{
		return reflectionValue;
	}

	virtual AbsorptionCoefficient absorption(Frequency frequency) const
	{
		return absorptionValue;
	}
};