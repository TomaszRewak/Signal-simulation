//#pragma once
//
//#include "SignalSimulation.hpp"
//
//#include <vector>
//#include <algorithm>
//
//struct WaveformSignalSimulationParameters {
//	WaveformSignalSimulationParameters()
//	{ }
//};
//
//struct WaveformSignalSimulationDistortion
//{
//	AbsorptionCoefficient absorption;
//	ObstacleDistortion reflection;
//
//	WaveformSignalSimulationDistortion()
//	{}
//};
//
//class WaveformSignalSimulation
//{
//private:
//	struct Ray
//	{
//		DiscretePoint position;
//
//		Position source;
//		Distance distance;
//		Distance previousDistance;
//
//		FreeVector normalVector;
//		FreeVector offset;
//
//		int reflections = 0;
//
//		PowerCoefficient powerCoefficient;
//
//		Ray(Position source, DiscretePoint position, FreeVector normalVector, int reflections) :
//			source(source),
//			position(position),
//			normalVector(normalVector.normalized()),
//			reflections(reflections),
//			powerCoefficient(PowerCoefficient::in<PowerCoefficient::Unit::coefficient>(1))
//		{ }
//	};
//
//	const Frequency frequency;
//	const WaveformSignalSimulationParameters simulationParameters;
//
//	ConnectionsSpace<WaveformSignalSimulationDistortion> simulationSpace;
//
//public:
//	WaveformSignalSimulation(SignalSimulationSpaceDefinitionPtr simulationSpaceDefinition, Frequency frequency, WaveformSignalSimulationParameters simulationParameters = WaveformSignalSimulationParameters()) :
//		frequency(frequency),
//		simulationParameters(simulationParameters),
//		simulationSpace(simulationSpaceDefinition->spaceSize, simulationSpaceDefinition->precision)
//	{
//		const auto directions = DiscreteDirection::baseDirections();
//
//		for (const auto& obstacle : simulationSpaceDefinition->obstacles)
//		{
//			for (int x = 0; x < simulationSpace.resolution.width; x++)
//			{
//				for (int y = 0; y < simulationSpace.resolution.height; y++)
//				{
//					DiscretePoint firstDiscretePosition(x, y);
//					Position firstPosition = simulationSpace.getPosition(firstDiscretePosition);
//
//					auto& element = simulationSpace.getElement(firstDiscretePosition);
//
//					for (int i = 0; i < directions.size(); i++)
//					{
//						DiscretePoint secondDiscretePosition = firstDiscretePosition + directions[i];
//						Position secondPosition = simulationSpace.getPosition(secondDiscretePosition);
//
//						auto& connection = element[i];
//
//						auto absorption = obstacle->absorption(firstPosition, secondPosition, frequency);
//						connection.absorption = connection.absorption + absorption;
//
//						auto distortion = obstacle->distortion(firstPosition, secondPosition, frequency);
//						connection.reflection = connection.reflection + distortion;
//					}
//				}
//			}
//		}
//	}
//
//	virtual SignalMapPtr simulate(Position transmitterPosition) const
//	{
//		auto signalMap = std::make_shared<SignalMap>(simulationSpace.surface, simulationSpace.precision);
//
//
//
//		return signalMap;
//	}
//};