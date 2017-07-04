#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "RaycastingSignalSimulation.hpp"
#include "FriisSignalSimulation.hpp"
#include "BFSSignalSimulation.hpp"
#include "BuildingMap.hpp"

using namespace std;

enum SimulationType
{
	Ray,
	Friis,
	BFS
};

int main()
{
	SimulationType type = SimulationType::Ray;

	// http://www.am1.us/Protected_Papers/E10589_Propagation_Losses_2_and_5GHz.pdf

	std::vector<Point> interior{
		Point(4, 0.5),
		Point(4, 3),
		Point(4.5, 3),
		Point(4.5, 0.5),
		Point(6.5, 0.5),
		Point(6.5, 3),
		Point(7, 3),
		Point(7, 0.5),
		Point(13, 0.5),
		Point(13, 3),
		Point(13.5, 3),
		Point(13.5, 0.5),
		Point(16.5, 0.5),
		Point(16.5, 7),
		Point(13.5, 7),
		Point(13.5, 4.5),
		Point(13, 4.5),
		Point(13, 9.5),
		Point(10, 9.5),
		Point(10, 7),
		Point(7, 7),
		Point(7, 4.5),
		Point(6.5, 4.5),
		Point(6.5, 7.5),
		Point(9.5, 7.5),
		Point(9.5, 10),
		Point(10, 10),
		Point(10, 10.5),
		Point(10.5, 10.5),
		Point(10.5, 10),
		Point(13.5, 10),
		Point(13.5, 7.5),
		Point(19, 7.5),
		Point(19, 12.5),
		Point(10.5, 12.5),
		Point(10.5, 12),
		Point(10, 12),
		Point(10, 12.5),
		Point(0.5, 12.5),
		Point(0.5, 10.5),
		Point(4.5, 10.5),
		Point(4.5, 9.5),
		Point(4, 9.5),
		Point(4, 10),
		Point(0.5, 10),
		Point(0.5, 7.5),
		Point(4, 7.5),
		Point(4, 8),
		Point(4.5, 8),
		Point(4.5, 4.5),
		Point(4, 4.5),
		Point(4, 7),
		Point(0.5, 7),
		Point(0.5, 0.5)
	};

	std::vector<Point> exterior{
		Point(17, 0),
		Point(17, 7),
		Point(19.5, 7),
		Point(19.5, 13),
		Point(0, 13),
		Point(0, 0)
	};

	SolidShapePtr buildingShape = std::make_shared<CSGShapesDifference>(
		std::make_shared<Polygon>(exterior),
		std::make_shared<Polygon>(interior)
		);

	MaterialPtr material = std::make_shared<UniformMaterial>(
		PowerCoefficient::in<PowerCoefficient::Unit::dB>(-6.24),
		AbsorptionCoefficient::in<AbsorptionCoefficient::Unit::dB>(-4.43, Distance::in<Distance::Unit::mm>(300))
		);

	std::vector<ObstaclePtr> obstacles{
		std::make_shared<UniformObstacle<Distance::Unit::m>>(buildingShape, material)
	};

	SignalSimulationSpaceDefinitionPtr simulationSpace = std::make_shared<SignalSimulationSpaceDefinition>(
		obstacles,
		Surface::in<Distance::Unit::m>(Rectangle(-1, -1, 20.5, 14)),
		Distance::in<Distance::Unit::m>(0.05)
		);

	cout << "Preparing simulation" << endl;

	BuildingMapPtr buildingMap = std::make_shared<BuildingMap>(
		simulationSpace
		);

	/*SignalSimulationSpacePtr simulationSpace = std::make_shared<SignalSimulationSpace>(
		obstacles,
		Surface(Rectangle(-5, -5, 7, 5), Distance::Unit::m),
		Distance(5, Distance::Unit::cm)
		);*/

	Transmitter transmitter(
		Power::in<Power::Unit::dBm>(20.),
		AntenaGain::in<AntenaGain::Unit::dBi>(6)
	);
	Receiver receiver(
		AntenaGain::in<AntenaGain::Unit::dBd>(0)
	);

	Frequency frequency = Frequency::in<Frequency::Unit::GHz>(5.2);

	SignalSimulationPtr signalSimulation;
	string filename;

	switch (type) {
	case SimulationType::Ray:
	{
		filename = "ray";
		RaycastingSignalSimulationParameters simulationParameters(
			5000,
			5,
			transmitter,
			receiver,
			Power::in<Power::Unit::dBm>(-70)
		);
		signalSimulation = std::make_shared<RaycastingSignalSimulation>(simulationSpace, frequency, simulationParameters);
		break;
	}
	case SimulationType::Friis:
	{
		filename = "friis";
		FriisSignalSimulationParameters simulationParameters(
			transmitter,
			receiver,
			Power::in<Power::Unit::dBm>(-70)
		);
		signalSimulation = std::make_shared<FriisSignalSimulation>(simulationSpace, frequency, simulationParameters);
		break;
	}	
	case SimulationType::BFS:
	{
		filename = "bfs";
		BFSSignalSimulationParameters simulationParameters(
			transmitter,
			receiver,
			Power::in<Power::Unit::dBm>(-70),
			PowerCoefficient::in<PowerCoefficient::Unit::dBm>(-90)
		);
		signalSimulation = std::make_shared<BFSSignalSimulation>(simulationSpace, frequency, simulationParameters);
		break;
	}
	}

	cout << "Simulating" << endl;

	SignalMapPtr signalMap = signalSimulation->simulate(
		Position::in<Distance::Unit::m>(Point(2, 2))
	);

	// save file

	Rectangle boundingBox = simulationSpace->spaceSize.get<Distance::Unit::m>();

	size_t imageSize = 1000;

	double buildingLongerSide = max(boundingBox.getWidth(), boundingBox.getHeight());

	cout << "Finished" << endl;

	fstream file;
	file.open("x64/" + filename + ".pgm", ios::out);

	file << "P2\n";
	file << imageSize << ' ' << imageSize << ' ' << 256 << "\n";

	for (size_t i = 0; i < imageSize; i++)
	{
		for (size_t u = 0; u < imageSize; u++)
		{
			Position position = Position::in<Distance::Unit::m>(
				Point(
					boundingBox.minX() + buildingLongerSide * i / imageSize,
					boundingBox.minY() + buildingLongerSide * u / imageSize
				)
				);

			int color = 0;

			if (buildingMap->hasObstacle(position))
				color = 50;
			else
			{
				Power strength = signalMap->getSignalStrength(position, transmitter, receiver);

				if (strength.get<Power::Unit::dBW>() != 0)
				{
					double signal = strength.get<Power::Unit::dBm>();

					// -30 (best) - -70(worst) 
					signal = (signal + 70.) / 40.;

					signal = std::max(signal, 0.);
					signal = std::min(signal, 1.);

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