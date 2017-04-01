#pragma once

#include "Building.hpp"
#include "SignalMap.hpp"
#include "Transmitter.hpp"

struct SimulationParameters {
	double voxelSize = 1.;
	Rectangle simulationArea;
};

class Simulation
{
private:
	Building building;
	SimulationParameters parameters;

public:
	Simulation(Building& building, SimulationParameters parameters) :
		building(building),
		parameters(parameters)
	{
		if (parameters.simulationArea.height() == 0 || parameters.simulationArea.width() == 0)
		{
			// TODO
		}
	}

	SignalMap simulate(Transmitter transmitter, Point transmitterPosition)
	{
		std::vector<double> voxels;
		
		return SignalMap(std::move(voxels), parameters.voxelSize, 0, parameters.simulationArea);
	}
};