#include <iostream>
#include <fstream>
#include <memory>

#include "SignalSimulation.hpp"

using namespace std;

int main()
{
	Material material(0.8, 0.9);

	std::vector<Point> interior{
		Point(-4, 0),
		Point(-0.25, 0),
		Point(-0.25, 1.5),
		Point(0, 1.5),
		Point(0, 0),
		Point(1.5, 0),
		Point(1.5, -0.25),
		Point(0, -0.25),
		Point(0, -4),
		Point(4, -4),
		Point(4, -2.5),
		Point(4.25, -2.5),
		Point(4.25, -4),
		Point(6, -4),
		Point(6, 4),
		Point(4.25, 4),
		Point(4.25, -1.5),
		Point(4, -1.5),
		Point(4, -0.25),
		Point(2.5, -0.25),
		Point(2.5, 0),
		Point(4, 0),
		Point(4, 4),
		Point(0, 4),
		Point(0, 2.5),
		Point(-0.25, 2.5),
		Point(-0.25, 4),
		Point(-4, 4),
	};

	std::vector<Point> exterior{
		Point(-4.5, -4.5),
		Point(6.5, -4.5),
		Point(6.5, 4.5),
		Point(-4.5, 4.5)
	};

	ShapePtr buildingShape = std::make_shared<CSGShapesDifference>(
		std::make_shared<Polygon>(exterior),
		std::make_shared<Polygon>(interior)
		);

	std::vector<ObstaclePtr> obstacles{
		std::make_shared<UniformObstacle>(buildingShape, Material(0.8, 0.9))
	};

	cout << "Preparing Space" << endl;

	SignalSimulationSpacePtr simulationSpace = std::make_shared<SignalSimulationSpace>(
		Rectangle(-5, -5, 7, 5),
		0.05,
		obstacles
	);

	cout << "Space ready" << endl;

	SignalSimulationParameters simulationParameters(1000, 5, Power(1e-9, PowerUnit::mW));
	SignalSimulation simulation(simulationSpace, simulationParameters);

	SignalMapPtr signalMap = simulation.simulate(Point(2, -3));

	Transmitter transmitter(
		Frequency(2.4, FrequencyUnit::GHz), 
		Power(20., PowerUnit::dBm), 
		AntenaGain(12, AntenaGainUnit::dBi)
	);
	Reciver reciver(
		AntenaGain(0, AntenaGainUnit::dBd)
	);

	// save file

	Rectangle boundingBox = simulationSpace->bounds;

	size_t imageSize = 1000;

	double buildingLongerSide = max(boundingBox.getWidth(), boundingBox.getHeight());

	cout << "Simulation ready" << endl;

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

			int color = 0;

			if (simulationSpace->inRange(point))
			{
				double signal = signalMap->getSignalStrength(transmitter, reciver, point).get(PowerUnit::dBm);
				double absorption = simulationSpace->getElement(point).absorption;

				signal = (signal + 90) / 60;
				signal = std::min(signal, 1.);
				signal = std::max(signal, 0.);

				color = (int)(255 * signal);

				if (absorption != 0)
					color = 50;
			}

			file << color << ' ';
		}

		file << "\n";
	}

	file.close();

	//cin.get();
}