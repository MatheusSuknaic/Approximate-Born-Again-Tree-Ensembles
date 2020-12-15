/*MIT License
Copyright(c) 2020 Thibaut Vidal
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef FSPACE_H
#define FSPACE_H

#include "Params.h"
#include "RandomForest.h"
#define epsilon 0.0000001

struct Cell
{
	unsigned int value;
	int classification;
	std::vector<double>begins;
	std::vector<double>ends;
	
	Cell()
	{
		value = 0;
		classification = 0;
	}

};


class FSpace
{
public:

	Params * params;                                             // Access to the problem and dataset parameters
	RandomForest * randomForest;                                 // Access to the random forest serving as input
	std::vector<std::vector<double>> orderedHyperplaneLevels;    // Lists all hyperplane levels
	std::vector<int> codeBook;                                   // Codebook used to convert cells into indices
	std::vector<Cell> cells;                                    // Class and value of each cell. Indexed based on the codeBook
	long int nbCells;                                            // Number of cells
	double nbPossibleRegions;                                    // Bound on the number of possible DP states

	// Auxiliary variables for some recursive functions
	std::vector<double> myCellValues;
	std::vector<double> previousCellValues;
	bool detectedDifference;
	int hypFeature, hypLevel;

	//std::vector<std::vector<double>> begins;
	//std::vector<std::vector<double>> ends;

	/*************************************************************************************************************************************/
	// Description: Converts a cell to its associated hash code
	//
	// Inputs:  myCell - vector containing all values from each feature for a cell
	//
	// Output: Hash value of the cell given as input
	//
	/*************************************************************************************************************************************/
	
	int cellToKey(const std::vector<int> & myCell);

	/*************************************************************************************************************************************/
	// Description: Gets the kth feature value of the cell for a given hash code
	//
	// Inputs:  key - Has value of the cell
	//			k - kth feature
	//
	// Output: kth feature value from the hash for the cell given as input
	//
	/*************************************************************************************************************************************/
	
	int keyToCell(int key, int k);

	/*************************************************************************************************************************************/
	// Description: Generates the hash code for a region (useful for perfect hashing)
	//
	// Inputs:  keyBottomLeft - hash value to the bottom leftmost cell of a region
	//			keyTopRight - hash value to the top rightmost cell of a region 
	//
	// Output: Hash code for the region enclosed between the two hash codes given as input
	//
	/*************************************************************************************************************************************/

	int keyToHash(int keyBottomLeft, int keyTopRight);

	/*************************************************************************************************************************************/
	// Description: Evaluates the majority class for each cell
	//
	// Inputs:  k - kth feature 
	//			mycellIndex - index of the cell 
	//			isfiltered - Checks if is hyperplanes have been filtered or not
	//
	// Output: None
	//
	/*************************************************************************************************************************************/
	
	void enumerateCellsRecursion(int k, int myCellIndex, bool isFiltered);

	/*************************************************************************************************************************************/
	// Description: Filters unnecessary hyperplanes
	//
	// Inputs:  k - kth feature 
	//			mycellIndex - index of the cell 
	//
	// Output: None
	//
	/*************************************************************************************************************************************/

	void hyperplaneFilteringRecursion(int k, int myCellIndex);

	/*************************************************************************************************************************************/
	// Description: Initializes the cells based on a list of hyperplanes
	//
	// Inputs:  hyperplanes - vector of features containing vectors for the values of each hyperplane
	//			isfiltered - Checks if is hyperplanes have been filtered or not
	//
	// Output: None
	//
	/*************************************************************************************************************************************/
	
	void initializeCells(const std::vector<std::vector<double>> & hyperplanes, bool isFiltered);

	/*************************************************************************************************************************************/
	// Description: Exports only the useful hyperplanes (that do not separate two cells that are from the same class)
	//
	// Inputs: None
	//
	// Output: hyperplanes - vector of features containing vectors for the values of each hyperplane
	//
	/*************************************************************************************************************************************/
	
	std::vector<std::vector<double>> exportUsefulHyperplanes();

	/*************************************************************************************************************************************/
	// Description: Prints each cell index, value, class and delimiting values for each feature.
	//
	// Inputs: None
	//
	// Output: None
	//
	/*************************************************************************************************************************************/

	void printCells();

	// Constructor
	FSpace(Params * params, RandomForest * randomForest) : params(params), randomForest(randomForest)
	{
		orderedHyperplaneLevels = std::vector<std::vector<double>>(params->nbFeatures);
		codeBook = std::vector<int>(params->nbFeatures, 1);
		myCellValues = std::vector<double>(params->nbFeatures);
		previousCellValues = std::vector<double>(params->nbFeatures);
	}
};

#endif
