#pragma once

#include "UniformFiniteElementsSpace.hpp"
#include "Obstacle.hpp"
#include "SimulationSpace.hpp"

#include <algorithm>
#include <memory>

struct SignalSimulationDistortion
{
	AbsorptionCoefficient absorption;
	ReflectionCoefficient reflection;

	SignalSimulationDistortion()
	{}
};

class SignalDistortionConnectionSpace : public ConnectionsSpace<SignalSimulationDistortion>
{
public:
	SignalDistortionConnectionSpace(
		Frequency frequency,
		SimulationSpacePtr simulationSpace
	) :
		ConnectionsSpace(simulationSpace)
	{
		const auto directions = DiscreteDirection::baseDirections();

		for (const auto& obstacle : simulationSpace->obstacles)
		{
			for (int x = 0; x < resolution.width; x++)
			{
				for (int y = 0; y < resolution.height; y++)
				{
					DiscretePoint elementDiscretePosition(x, y);
					Position elementPosition = getPosition(elementDiscretePosition);

					auto& element = getElement(elementDiscretePosition);
					auto absorption = obstacle->absorption(elementPosition, frequency);

					if (absorption.get(AbsorptionCoefficient::Unit::coefficient, Distance(1, Distance::Unit::m)) < 1)
						for (int i = 0; i < 4; i++)
							element[i].absorption = absorption;
				}
			}
		}
	}
};

//struct SignalSimulationElement
//{
//	MaterialPtr absorption;
//	std::array<SignalSimulationConnection, 4> connections;
//};
//
//class SignalSimulationSpace : public UniformFiniteElementsSpace<SignalSimulationElement>
//{
//public:
//	const Distance::Unit spaceUnit = Distance::Unit::m;
//
//	SignalSimulationSpace(
//		Frequency frequency,
//		const SimulationSpace& simulationSpace,
//		Distance elementsDistance
//	) :
//		UniformFiniteElementsSpace(spaceSize.get(spaceUnit), elementsDistance.get(spaceUnit))
//	{
//		const auto directions = DiscreteDirection::baseDirections();
//
//		for (const auto& obstacle : obstacles)
//		{
//			for (int x = 0; x < resolution.width; x++)
//			{
//				for (int y = 0; y < resolution.height; y++)
//				{
//					DiscretePoint elementDiscretePosition(x, y);
//					Position elementPosition(getPosition(elementDiscretePosition), spaceUnit);
//
//					auto& element = getElement(elementDiscretePosition);
//					element.absorption = obstacle->absorption(elementPosition, frequency).get(
//						AbsorptionCoefficient::Unit::coefficient,
//						elementsDistance);
//
//					for (auto direction : DiscreteDirection::baseDirections())
//					{
//						DiscretePoint connectedElementDiscretePosition = elementDiscretePosition + direction;
//						Position connectedElementPosition(getPosition(connectedElementDiscretePosition), spaceUnit);
//
//						auto& connection = getElement(elementDiscretePosition).connections[direction.getIndex()];
//						auto distortions = obstacle->distortion(elementPosition, connectedElementPosition, frequency);
//
//						std::sort(distortions.begin(), distortions.end());
//
//						for (const auto& distortion : distortions)
//						{
//							SignalSimulationDistortion signalDistortion(
//								distortion.normalVector,
//								distortion.distance,
//								distortion.coefficient.get(ReflectionCoefficient::Unit::coefficient)
//							);
//
//							connection.distortion = connection.distortion + signalDistortion;
//						}
//					}
//				}
//			}
//		}
//	}
//};
//using SignalSimulationSpacePtr = std::shared_ptr<const SignalSimulationSpace>;