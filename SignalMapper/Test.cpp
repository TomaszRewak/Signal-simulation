#include <iostream>
#include <fstream>

#include "SimulationSpace.hpp"
#include "SignalSimulation.hpp"

using namespace std;

int main()
{
	SimulationSpace building;

	Material material(0.8, 4, 0.4, 0.01);

	vector<Point> points{
		Point(-4, 4),
		Point(-0.25, 4),
		Point(-0.25, 2.5),
		Point(0, 2.5),
		Point(0, 4),
		Point(4, 4),
		Point(4, 0),
		Point(2.5, 0),
		Point(2.5, -0.25),
		Point(4, -0.25),
		Point(4, -1.5),
		Point(4.25, -1.5),
		Point(4.25, 4),
		Point(6, 4),
		Point(6, -4),
		Point(4.25, -4),
		Point(4.25, -2.5),
		Point(4, -2.5),
		Point(4, -4),
		Point(0, -4),
		Point(0, -0.25),
		Point(1.5, -0.25),
		Point(1.5, 0),
		Point(0, 0),
		Point(0, 1.5),
		Point(-0.25, 1.5),
		Point(-0.25, 0),
		Point(-4, 0),
		Point(-4, 4)
	};

	for (int i = 0; i < points.size() - 1; i++)
		building.addObstacle(WallObstacle(points[i], points[i + 1], material));

	SignalSimulationParameters simulationParameters(0.02, 2000);
	SignalSimulation simulation(building, simulationParameters);

	Transmitter transmitter(560, 0.5);
	SignalMap map = simulation.simulate(transmitter, Point(2, -3));

	// save file

	Rectangle boundingBox = building.boundingBox();
	boundingBox.setWidth(boundingBox.getWidth() * 1.5);
	boundingBox.setHeight(boundingBox.getHeight() * 1.5);

	size_t imageSize = 1000;

	double buildingLongerSide = max(boundingBox.getWidth(), boundingBox.getHeight());

	cout << "Ready" << endl;

	fstream file;
	file.open("Debug/test.pgm", ios::out);

	file << "P2\n";
	file << imageSize << ' ' << imageSize << ' ' << 256 << "\n";

	for (size_t i = 0; i < imageSize; i++)
	{
		for (size_t u = 0; u < imageSize; u++)
		{
			Point point(
				boundingBox.minX() + buildingLongerSide * i / imageSize,
				boundingBox.minY() + buildingLongerSide * u / imageSize
			);

			double signal = map.getSignalStrength(point);
			bool obstacle = map.hasObstacle(point);

			int color = obstacle ? 100 : (int)(255 * std::min(signal, 1.));

			file << color << ' ';
		}

		file << "\n";
	}

	file.close();

	//cin.get();
}