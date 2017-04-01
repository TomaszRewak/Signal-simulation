#include <iostream>

#include "Building.hpp"
#include "Simulation.hpp"

using namespace std;

int main()
{
	Building building;

	building.addObstacle(WallObstacle(Point(0, 0), Point(5, 0), 2, Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(0, 0), Point(0, 5), 2, Material(0.5, 4, 0.1, 0.01)));

	Building buildingWithDoor = building;

	buildingWithDoor.addObstacle(WallObstacle(Point(5, 0), Point(5, 4), 1, Material(0.3, 4, 0.1, 0.01)));

	SimulationParameters simulationParameters;
	Simulation simulation(building, simulationParameters);

	Transmitter transmitter(560, 1.);
	SignalMap map = simulation.simulate(transmitter, Point(1, 1));

	for (double x = -10; x < 10; x += .5)
	{
		for (double y = -10; y < 10; y += .5)
		{
			cout << (int)(map.getSignalStrength(x, y) * 10);
		}

		cout << endl;
	}

	cin.get();
}