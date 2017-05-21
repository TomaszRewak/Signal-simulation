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
		Point(6, 6),
		Point(4.25, 6),
		Point(4.25, -1.5),
		Point(4, -1.5),
		Point(4, -0.25),
		Point(2.5, -0.25),
		Point(2.5, 0),
		Point(4, 0),
		Point(4, 6),
		Point(0, 6),
		Point(0, 2.5),
		Point(-0.25, 2.5),
		Point(-0.25, 6),
		Point(-4, 6),
	};

	std::vector<Point> exterior{
		Point(-4.5, -0.5),
		Point(-0.5, -0.5),
		Point(-0.5, -4.5),
		Point(6.5, -4.5),
		Point(6.5, 6.5),
		Point(-4.5, 6.5)
	};

	SolidShapePtr buildingShape = std::make_shared<CSGShapesDifference>(
		std::make_shared<Polygon>(exterior),
		std::make_shared<Polygon>(interior)
		);

	MaterialPtr material = std::make_shared<UniformMaterial>(
		//PowerCoefficient(0.8, PowerCoefficient::Unit::coefficient),
		PowerCoefficient(-6.24, PowerCoefficient::Unit::dB),
		//AbsorptionCoefficient(0.8, AbsorptionCoefficient::Unit::coefficient, Distance(1, Distance::Unit::m))
		AbsorptionCoefficient(-4.43, AbsorptionCoefficient::Unit::dB, Distance(200, Distance::Unit::mm))
		);

	std::vector<ObstaclePtr> obstacles{
		std::make_shared<UniformObstacle>(buildingShape, Distance::Unit::m, material)
	};

	cout << "Preparing building map" << endl;

	SimulationSpacePtr simulationSpace = std::make_shared<SimulationSpace>(
		obstacles,
		Surface(Rectangle(-5, -5, 7, 7), Distance::Unit::m),
		Distance(0.1, Distance::Unit::m)
		);

	BuildingMapPtr buildingMap = std::make_shared<BuildingMap>(
		simulationSpace
		);

	cout << "Building map ready" << endl;

	/*SignalSimulationSpacePtr simulationSpace = std::make_shared<SignalSimulationSpace>(
		obstacles,
		Surface(Rectangle(-5, -5, 7, 5), Distance::Unit::m),
		Distance(5, Distance::Unit::cm)
		);*/

	SignalSimulationParameters simulationParameters(5000, 3);
	SignalSimulation simulation(simulationSpace, simulationParameters);

	SignalMapPtr signalMap = simulation.simulate(
		Frequency(2.4, Frequency::Unit::GHz),
		Position(Point(2, -3), Distance::Unit::m)
	);

	Transmitter transmitter(
		Power(20., Power::Unit::dBm),
		AntenaGain(6, AntenaGain::Unit::dBi)
	);
	Reciver reciver(
		AntenaGain(0, AntenaGain::Unit::dBd)
	);

	// save file

	Rectangle boundingBox = simulationSpace->spaceSize.get(Distance::Unit::m);

	size_t imageSize = 1000;

	double buildingLongerSide = max(boundingBox.getWidth(), boundingBox.getHeight());

	cout << "Simulation ready" << endl;

	fstream file;
	file.open("Release/test.pgm", ios::out);

	file << "P2\n";
	file << imageSize << ' ' << imageSize << ' ' << 256 << "\n";

	for (size_t i = 0; i < imageSize; i++)
	{
		for (size_t u = 0; u < imageSize; u++)
		{
			Position position(
				Point(
					boundingBox.minX() + buildingLongerSide * i / imageSize,
					boundingBox.minY() + buildingLongerSide * u / imageSize
				),
				Distance::Unit::m
			);

			int color = 0;

			if (buildingMap->hasObstacle(position))
				color = 50;
			else
			{
				double signal = signalMap->getSignalStrength(position, transmitter, reciver).get(Power::Unit::dBm);

				// -30 (best) - -70(worst) 
				signal = (signal + 70.) / 40.;

				signal = std::max(signal, 0.);
				signal = std::min(signal, 1.);

				color = (int)(255 * signal);
			}

			file << color << ' ';
		}

		file << "\n";
	}

	file.close();

	//cin.get();
}