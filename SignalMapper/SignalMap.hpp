#pragma once

#include "Transmitter.hpp"
#include "SimulationSpace.hpp"

#include <vector>
#include <functional>

template<typename T>
struct SmoothingFilter {
	virtual T smooth(Point point, std::function<T(Point)> f) const = 0;
};
template<typename T>
using SmoothingFilterPtr = std::shared_ptr<SmoothingFilter<T> const>;

class SignalMap : public SimulationUniformFiniteElementsSpace<PowerCoefficient>
{
private:
	PowerCoefficient getSignalStrength(DiscretePoint position, const Transmitter& transmitter, const Receiver& receiver) const
	{
		position.x = std::max(position.x, 0);
		position.y = std::max(position.y, 0);
		position.x = std::min(position.x, resolution.width);
		position.y = std::min(position.y, resolution.height);

		return getElement(position);
	}

public:
	SignalMap(Surface spaceSize, Distance precision) :
		SimulationUniformFiniteElementsSpace(spaceSize, precision)
	{
		for (int x = 0; x < resolution.width; x++)
			for (int y = 0; y < resolution.height; y++)
				getElement(DiscretePoint(x, y)) = PowerCoefficient::in<PowerCoefficient::Unit::coefficient>(0);
	}

	Power getSignalStrength(Position position, const Transmitter& transmitter, const Receiver& receiver) const
	{
		if (!inRange(position))
			return Power();

		DiscretePoint point = getDiscretePoint(position);

		DiscretePoint points[]{
			point,
			point + DiscreteDirection(1, 0),
			point + DiscreteDirection(1, 1),
			point + DiscreteDirection(0, 1),
		};

		double db = 0;

		for (auto p : points)
		{
			if (!inRange(p))
				p = point;

			auto pPosition = getPosition(p);

			PowerCoefficient coefficient = getSignalStrength(p, transmitter, receiver);

			if (coefficient.get<PowerCoefficient::Unit::coefficient>() > 0)
				db +=
					coefficient.get<PowerCoefficient::Unit::dB>() *
					(1 - abs(pPosition.x() - position.x()) / precision) *
					(1 - abs(pPosition.y() - position.y()) / precision);
		}

		PowerCoefficient finalCoefficient =
			db > 0 ?
			PowerCoefficient::in<PowerCoefficient::Unit::dB>(db)
			:
			PowerCoefficient::in<PowerCoefficient::Unit::coefficient>(0);

		return transmitter.power * transmitter.antenaGain * receiver.antenaGain * getSignalStrength(point, transmitter, receiver);
	}
};
using SignalMapPtr = std::shared_ptr<const SignalMap>;