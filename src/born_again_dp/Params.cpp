#include "Params.h"

std::vector<std::vector<double>> Params:: readDataSet(std::string data)
{
	std::vector<std::vector<double>> points;
	std::ifstream file(data);
	std::string line, cood;
	int pos = -1;

	if (file.is_open())
	{
		std::cout << "File has been opened" << "\n";

		file >> line; // We don't need the column names

		while (file >> line)
		{
			std::vector<double> coordenates;
			std::stringstream  lineStream(line);
			double hold;

			while (std::getline(lineStream, cood, ','))
			{
				hold = std::stod(cood);
				coordenates.push_back(hold);
			}

			points.push_back(coordenates);
			//std::cout << line << "\n";
		}

		file.close();
	}

	/*for (int i = 0; i < points.size(); i++)
	{
		for (int j = 0; j < points[i].size(); j++)
		{
			std::cout << dataPoints[i][j]<<" ";
		}
		std::cout << "\n";
	}*/

	return points;
}

Params::Params(std::ifstream & inputFile, int nbTrees, int objectiveFunction, int seed, int value, int depth, std::string databaseName) : nbTrees(nbTrees), objectiveFunction(objectiveFunction), seed(seed), value(value), depth(depth),databaseName(databaseName)
{
	nbCellsSampled = 1000; // Currently setting manually the number of manufactured samples and seed
	generator.seed(seed);
	std::string useless;
	inputFile >> useless >> datasetName;
	inputFile >> useless >> ensembleMethod;
	std::getline(inputFile, useless);
	if (nbTrees == -1) inputFile >> useless >> this->nbTrees;
	else inputFile >> useless >> useless;
	inputFile >> useless >> nbFeatures;
	inputFile >> useless >> nbClasses;
	inputFile >> useless >> maxDepth;
	std::getline(inputFile, useless);
	std::getline(inputFile, useless);
	std::getline(inputFile, useless);

	if (value == 2)
	{
		databaseName = "/home/matheus/Desktop/Approximated Born Again Tree Ensembles/src/resources/datasets/" + databaseName +"/" + databaseName + ".full.csv" ;
		dataPoints = readDataSet(databaseName);
		std::cout <<" Name of the database: " << databaseName << "\n";
	}

};
