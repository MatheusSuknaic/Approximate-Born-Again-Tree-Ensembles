#include "FSpace.h"

int FSpace::cellToKey(const std::vector<int> & myCell)
{
	int key = 0;
	for (int k = 0; k < params->nbFeatures; k++)
		key += (int)myCell[k] * codeBook[k];
	return key;
}

int FSpace::keyToCell(int key, int k)
{
	if (k > 0) return (key % codeBook[k - 1]) / codeBook[k];
	else return key / codeBook[k];
}

int FSpace::keyToHash(int keyBottomLeft, int keyTopRight)
{
	int keyLeft = keyBottomLeft;
	int keyRight = keyTopRight;
	int code = 1;
	int newKey = 0;
	for (int k = params->nbFeatures - 1; k >= 0; k--)
	{
		int size = (int)orderedHyperplaneLevels[k].size();
		if (size != 1)
		{
			int valueLeft = keyLeft % size;
			int valueRight = keyRight % size;
			keyLeft = keyLeft / size;
			keyRight = keyRight / size;
			newKey += (valueRight - valueLeft)*code;
			code *= size - valueLeft;
		}
	}
	return newKey;
}

void FSpace::enumerateCellsRecursion(int k, int myCellIndex, bool isFiltered)
{
	if (k == params->nbFeatures) // All features have been specified, the cell can be evaluated
	{
		cells[myCellIndex].classification = randomForest->majorityClass(myCellValues);

		for (int i = 0; i < params->nbFeatures; i++)
		{
			cells[myCellIndex].begins.push_back(previousCellValues[i]);
			cells[myCellIndex].ends.push_back(myCellValues[i]);
		}

		if (params->value == 0 && isFiltered) //Every cell has the same importance
		{
			cells[myCellIndex].value = 1;
		}
		else if (params->value == 1 && isFiltered) //Each cell has importance equal to its volume
		{
			double value = 1.0;

			for (int i = 0; i < params->nbFeatures; i++)
			{
				value *= myCellValues[i] - previousCellValues[i];
			}

			cells[myCellIndex].value = value;
		}
		else if (params->value == 2 && isFiltered) //Each cell has importance equal to the number of data points inside it
		{
			int sum;
			for (int i = 0; i < params->dataPoints.size(); i++)
			{
				sum = 0;

				for (int j = 0; j < params->nbFeatures; j++)
				{
					if ((previousCellValues[j] <= params->dataPoints[i][j]) && (params->dataPoints[i][j] <= myCellValues[j])) //Has to be between cell boundaries for a feature
					{
						sum = sum + 1;
					}
				}

				if (sum == params->nbFeatures)
				{
					//std::cout << "This point is inside this cell!" << "\n";
					params->dataPoints.erase(params->dataPoints.begin() + i);
					cells[myCellIndex].value = cells[myCellIndex].value + 1;
				}
			}
		}
	}
	else // Otherwise recursive call
	{
		const int codeBookValue = codeBook[k];
		for (int i = 0; i < (int)orderedHyperplaneLevels[k].size(); i++)
		{
			if (i == 0) previousCellValues[k] = 0;
			else previousCellValues[k] = myCellValues[k];

			myCellValues[k] = orderedHyperplaneLevels[k][i];
			enumerateCellsRecursion(k + 1, myCellIndex + codeBookValue * i, isFiltered);
		}
	}
}

void FSpace::hyperplaneFilteringRecursion(int k, int myCellIndex)
{
	if (k == params->nbFeatures)
	{
		if (cells[myCellIndex].classification != cells[myCellIndex + codeBook[hypFeature]].classification)
			detectedDifference = true;
	}
	else if (k == hypFeature)
		hyperplaneFilteringRecursion(k + 1, myCellIndex + codeBook[hypFeature] * hypLevel);
	else
		for (int i = 0; i < (int)orderedHyperplaneLevels[k].size(); i++)
			hyperplaneFilteringRecursion(k + 1, myCellIndex + codeBook[k] * i);
}

void FSpace::initializeCells(const std::vector<std::vector<double>> & hyperplanes, bool isFiltered)
{
	orderedHyperplaneLevels = hyperplanes;
	nbCells = 1;
	nbPossibleRegions = 1.0;
	for (int k = 0; k < params->nbFeatures; k++)
	{
		nbCells *= (int)orderedHyperplaneLevels[k].size();
		nbPossibleRegions *= (double)orderedHyperplaneLevels[k].size()*((double)orderedHyperplaneLevels[k].size() + 1.0) / 2.0;
	}

	if (!isFiltered && nbCells > 10000000)
	{
		std::cout << "WARNING: THIS CASE LEADS TO " << nbCells << " CELLS BEFORE FILTERING." << std::endl;
		std::cout << "MEMORY CONSUMPTION CAN BE HIGH. WE RECOMMEND TO USE THE HEURISTIC SOLVER VARIANT (-obj 4)" << std::endl;
	}

	if (isFiltered && nbCells > 200000)
	{
		std::cout << "WARNING: THIS CASE LEADS TO " << nbCells << " CELLS AFTER FILTERING." << std::endl;
		std::cout << "MEMORY CONSUMPTION CAN BE HIGH. WE RECOMMEND TO USE THE HEURISTIC SOLVER VARIANT (-obj 4)" << std::endl;
	}

	// Initialize structures and evaluate the majority class of each cell
	for (int k = params->nbFeatures - 2; k >= 0; k--) codeBook[k] = codeBook[k + 1] * (int)orderedHyperplaneLevels[k + 1].size();

	cells = std::vector<Cell>(nbCells);
	enumerateCellsRecursion(0, 0, isFiltered);
}

std::vector<std::vector<double>> FSpace::exportUsefulHyperplanes()
{
	std::vector<std::vector<double>> myHyperplanes(params->nbFeatures);
	for (hypFeature = 0; hypFeature < params->nbFeatures; hypFeature++)
	{
		for (hypLevel = 0; hypLevel < (int)orderedHyperplaneLevels[hypFeature].size() - 1; hypLevel++)
		{
			detectedDifference = false;
			hyperplaneFilteringRecursion(0, 0);
			if (detectedDifference) myHyperplanes[hypFeature].push_back(orderedHyperplaneLevels[hypFeature][hypLevel]);
		}
		myHyperplanes[hypFeature].push_back(1.e30);
	}
	return myHyperplanes;
}

void FSpace::printCells()
{
	for (int i = 0; i < nbCells; i++)
	{
		std::cout << "Index: " << i << " Value: " << cells[i].value << " Class: " << cells[i].classification<< "\n";

		for (int j = 0; j < params->nbFeatures; j++)
		{
			std::cout <<"Bounds feature "<< j << ": "<<  cells[i].begins[j] << " " << cells[i].ends[j] << "\n";
		}
		std::cout << "\n";
	}
}
