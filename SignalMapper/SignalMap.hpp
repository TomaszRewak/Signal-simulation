#pragma once

#include "UniformFiniteElementsSpace.hpp"
#include "SignalSimulationSpace.hpp"
#include "Transmitter.hpp"
#include "SimulationSpace.hpp"

#include <vector>

class SignalSimulation;

class BuildingMap : protected SimulationUniformFiniteElementsSpace<int>
{
public:
	BuildingMap(SimulationSpacePtr simulationSpace) :
		SimulationUniformFiniteElementsSpace(simulationSpace->spaceSize, simulationSpace->precision)
	{
		for (int x = 0; x < resolution.width; x++)
			for (int y = 0; y < resolution.height; y++)
				getElement(DiscretePoint(x, y)) = 0;

		for (const auto& obstacle : simulationSpace->obstacles)
		{
			for (int x = 0; x < resolution.width; x++)
			{
				for (int y = 0; y < resolution.height; y++)
				{
					DiscretePoint elementDiscretePosition(x, y);
					Position elementPosition = getPosition(elementDiscretePosition);

					if (obstacle->inside(elementPosition))
					{
						auto& element = getElement(elementDiscretePosition);
						element++;
					}
				}
			}
		}
	}

	bool hasObstacle(Position position) const
	{
		if (!inRange(position))
			return false;

		return getElement(position) > 0;
	}
};
using BuildingMapPtr = std::shared_ptr<const BuildingMap>;

class SignalMap : protected SimulationUniformFiniteElementsSpace<PowerCoefficient>
{
private:
	PowerCoefficient getSignalStrength(DiscretePoint position, const Transmitter& transmitter, const Reciver& reciver) const
	{
		position.x = std::max(position.x, 0);
		position.y = std::max(position.y, 0);
		position.x = std::min(position.x, resolution.width);
		position.y = std::min(position.y, resolution.height);

		return getElement(position);
	}

public:
	SignalMap(SimulationSpacePtr simulationSpace) :
		SimulationUniformFiniteElementsSpace(simulationSpace->spaceSize, simulationSpace->precision)
	{
		for (int x = 0; x < resolution.width; x++)
			for (int y = 0; y < resolution.height; y++)
				getElement(DiscretePoint(x, y)) = PowerCoefficient::in<PowerCoefficient::Unit::coefficient>(0);
	}

	Power getSignalStrength(Position position, const Transmitter& transmitter, const Reciver& reciver) const
	{
		if (!inRange(position))
			return Power();

		DiscretePoint point = getDiscretePoint(position);

		DiscretePoint points[]{
			point,
			point + DiscreteDirection(1, 0),
			point + DiscreteDirection(1, 0) + DiscreteDirection(0, 1),
			point + DiscreteDirection(0, 1),
		};

		double db = 0;

		for (auto p : points)
		{
			auto pPosition = getPosition(p);

			db +=
				getSignalStrength(p, transmitter, reciver).get<PowerCoefficient::Unit::dB>() *
				(1 - abs(pPosition.x() - position.x()) / precision) *
				(1 - abs(pPosition.y() - position.y()) / precision);
		}

		return transmitter.power * transmitter.antenaGain * reciver.antenaGain * PowerCoefficient::in<PowerCoefficient::Unit::dB>(db);
	}

	friend SignalSimulation;
};
using SignalMapPtr = std::shared_ptr<const SignalMap>;