#include <iostream>
#include <fstream>

#include "SimulationSpace.hpp"
#include "SignalSimulation.hpp"

using namespace std;

int main()
{
	SimulationSpace building;

	building.addObstacle(WallObstacle(Point(-5, -5), Point(0, -5), Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(0, -5), Point(0, 0), Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(0, 0), Point(5, 0), Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(5, 0), Point(5, 5), Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(5, 5), Point(-5, 5), Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(-5, 5), Point(-5, -5), Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(-5, 0), Point(-3, 0), Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(-2, 0), Point(0, 0), Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(0, 0), Point(0, 2), Material(0.5, 4, 0.1, 0.01)));
	building.addObstacle(WallObstacle(Point(0, 3), Point(0, 5), Material(0.5, 4, 0.1, 0.01)));


	/*SimulationSpace buildingWithDoor = building;

	buildingWithDoor.addObstacle(WallObstacle(Point(5, 0), Point(5, 4), Material(0.3, 4, 0.1, 0.01)));*/

	SignalSimulationParameters simulationParameters(0.2, 400);
	SignalSimulation simulation(building, simulationParameters);

	Transmitter transmitter(560, 0.5);
	SignalMap map = simulation.simulate(transmitter, Point(2.5, 2.5));

	// save file

	Rectangle boundingBox = building.boundingBox();
	boundingBox.setWidth(boundingBox.getWidth() * 1.5);
	boundingBox.setHeight(boundingBox.getHeight() * 1.5);

	size_t imageSize = 1000;

	double buildingLongerSide = max(boundingBox.getWidth(), boundingBox.getHeight());

	fstream file;
	file.open("Debug/test.pgm", ios::out);

	file << "P2\n";
	file << imageSize << ' ' << imageSize << ' ' << 256 << "\n";

	for (size_t i = 0; i<imageSize; i++)
	{
		for (size_t u = 0; u<imageSize; u++)
		{
			Point point(
				boundingBox.minX() + buildingLongerSide * i / imageSize,
				boundingBox.minY() + buildingLongerSide * u / imageSize
			);

			double signal = map.getSignalStrength(point);
			bool obstacle = map.hasObstacle(point);

			int color = obstacle ? 100 : (255 * std::min(signal, 1.));

			file << color << ' ';
		}

		file << "\n";
	}

	file.close();

	//cin.get();
}