#include <iostream>
#include <fstream>
#include <memory>

#include "SignalSimulation.hpp"

using namespace std;

int main()
{
	// http://www.am1.us/Protected_Papers/E10589_Propagation_Losses_2_and_5GHz.pdf

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
		Point(-4.5, -0.5),
		Point(-0.5, -0.5),
		Point(-0.5, -4.5),
		Point(6.5, -4.5),
		Point(6.5, 4.5),
		Point(-4.5, 4.5)
	};

	ShapePtr buildingShape = std::make_shared<CSGShapesDifference>(
		std::make_shared<Polygon>(exterior),
		std::make_shared<Polygon>(interior)
		);

	MaterialPtr material = std::make_shared<UniformMaterial>(
		ReflectionCoefficient(-6.24, ReflectionCoefficientUnit::dB),
		AbsorptionCoefficient(-4.43, 0.2, AbsorptionCoefficientUnit::dB)
		);

	std::vector<ObstaclePtr> obstacles{
		std::make_shared<UniformObstacle>(buildingShape, material)
	};

	cout << "Preparing Space" << endl;

	SignalSimulationSpacePtr simulationSpace = std::make_shared<SignalSimulationSpace>(
		Frequency(2.4, FrequencyUnit::GHz),
		obstacles,
		Rectangle(-5, -5, 7, 5),
		0.05
	);

	cout << "Space ready" << endl;

	SignalSimulationParameters simulationParameters(1000, 5, Power(1e-9, PowerUnit::mW));
	SignalSimulation simulation(simulationSpace, simulationParameters);

	SignalMapPtr signalMap = simulation.simulate(Point(2, -3));

	Transmitter transmitter(
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
				if (simulationSpace->hasObstacle(point))
					color = 50;
				else
				{
					double signal = signalMap->getSignalStrength(transmitter, reciver, point).get(PowerUnit::dBm);

					signal = (signal + 90) / 60;
					signal = std::min(signal, 1.);
					signal = std::max(signal, 0.);

					color = (int)(255 * signal);
				}
			}

			file << color << ' ';
		}

		file << "\n";
	}

	file.close();

	//cin.get();
}