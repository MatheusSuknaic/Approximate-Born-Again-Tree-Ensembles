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

#ifndef BORNAGAINDECISIONTREE_H
#define BORNAGAINDECISIONTREE_H

#include "Params.h"
#include "RandomForest.h"
#include "FSpace.h"
#include <set>

#ifdef USING_CPLEX
#include "MIPCertificate.h"
#endif

#define BIG_M 100000 // Weight attributed to the depth in objective 2 (optimize weight then number of leaves)
#define MY_EPSILON 0.00001 // Precision parameter, useful to avoid numerical instabilities

class BornAgainDecisionTree
{
private:

	Params * params;							// Access to the problem and dataset parameters
	RandomForest * randomForest;	// Access to the random forest serving as input
	FSpace fspaceOriginal;				// Original feature space
	FSpace fspaceFinal;						// Feature space after filtering unnecessary hyperplanes

#ifdef USING_CPLEX
	MIPCertificate * myMIPcertificate;	// Access to the solver
#endif

	// Variables used by the heuristic BA trees
	std::vector<std::vector<double>> orderedHyperplaneLevelsHeuristic;  // Ordered hyperplane values for each feature
	std::vector<unsigned short int> bottomLeftCell;                     // Bottom left cell (for the heuristic)
	std::vector<unsigned short int> topRightCell;			            			// Top right cell (for the heuristic)
	std::vector<std::vector<unsigned short int>> sampledCellsIndices;   // vector of cell indices representing the samples (for the heuristic)
	std::vector<std::vector<double>> sampledCellsCoords;								// vector of cell coordinates representing the samples (for the heuristic)
	std::vector<unsigned short int> classSampledCells;                  // class of the sampled cells
	std::set<int> nonTrivialFeatures;                                   // Contains the list of features which are not restricted to a single index
	std::set<int> nonTrivialFeaturesBeforeSplits;                       // Contains the list of features which are not restricted to a single index (not updated when splitting)

	// The regions are maintained contiguously in a vector using a perfect hash
	// This demonstrated a better memory performance than an unordered_map for obj 1 and 2

	std::vector<std::vector<unsigned int>> regions;	
	
	//std::vector<std::vector<double>> begins;
	//std::vector<std::vector<double>> ends;

	// Born-again tree produced by the algorithm, using the same internal representation as scikit-learn

	std::vector<Node> rebornTree;

	// Run statistics

	unsigned long int iterationsDP;
	unsigned long int regionsMemorizedDP;
	double finalObjective;
	unsigned int finalSplits;
	unsigned int finalLeaves;
	unsigned int finalDepth;

	/*******************************************************************************************************************************************/
	// Description: Dynamic programming procedure to reconstruct the decision tree optimizing the depth of the tree (D)
	//
	// Inputs: indexBottom - hash value to the bottom leftmost cell of a region
	//		   indexTop - hash value to the top rightmost cell of a region
	//
	// Output: The best upper bound found while computing the BA tree.
	//
	/******************************************************************************************************************************************/
		
	unsigned int dynamicProgrammingOptimizeDepth(int indexBottom, int indexTop);
	
	/*******************************************************************************************************************************************/
	// Description: Dynamic programming procedure to reconstruct the decision tree optimizing the new depth of the tree (ND)
	//
	// Inputs: indexBottom - hash value to the bottom leftmost cell of a region
	//		   indexTop - hash value to the top rightmost cell of a region
	//		   depth - Maximal depth allowed for the born again tree
	//
	// Output: The best upper bound found while computing the BA tree.
	//
	/*******************************************************************************************************************************************/
		
	unsigned int dynamicProgrammingOptimizeNewDepth(int indexBottom, int indexTop, unsigned int depth);

	/********************************************************************************************************************************************
	// Description: Dynamic programming procedure to reconstruct the decision tree optimizing the number of splits in the tree equivalent 
	//				to optimizing the number of leaves (L)
	//
	// Inputs: indexBottom - hash value to the bottom leftmost cell of a region
	//		   indexTop - hash value to the top rightmost cell of a region
	//
	// Output: The best upper bound found while computing the BA tree.
	//
	/*******************************************************************************************************************************************/
	
	unsigned int dynamicProgrammingOptimizeNbSplits(int indexBottom, int indexTop);

	/*******************************************************************************************************************************************/
	// Description: Dynamic programming procedure to reconstruct the decision tree optimizing the depth and then the number of splits 
	//				as a secondary objective (DL)
	//
	// Inputs: indexBottom - hash value to the bottom leftmost cell of a region
	//		   indexTop - hash value to the top rightmost cell of a region
	//
	// Output: The best upper bound found while computing the BA tree.
	//
	/*******************************************************************************************************************************************/
	
	unsigned int dynamicProgrammingOptimizeDepthThenNbSplits(int indexBottom, int indexTop);

	/*******************************************************************************************************************************************/
	// Description: Recursive procedure for the heuristic construction of the born again tree
	//
	// Inputs: currentDepth - current depth of the tree
	//
	// Output: Returns the current node index
	//
	/*******************************************************************************************************************************************/
	
	int recursiveHelperHeuristic(unsigned int currentDepth);

	/********************************************************************************************************************************************
	// Description: Extracts the optimal born again decision tree from the DP memory
	//
	// Inputs: indexBottom - hash value to the bottom leftmost cell of a region
	//		   indexTop - hash value to the top rightmost cell of a region
	//		   optValue - Optimal value obtained for the optimal born again decision tree
	//		   currentDepth - current depth of the tree
	//
	// Output: Returns the current node index
	//
	/*******************************************************************************************************************************************/
	
	int collectResultDP(int indexBottom, int indexTop, unsigned int optValue, unsigned int currentDepth);
	
	/*******************************************************************************************************************************************/
	// Description: Extracts the optimal born again decision tree from the DP memory for the new depth objective
	//
	// Inputs: indexBottom - hash value to the bottom leftmost cell of a region
	//		   indexTop - hash value to the top rightmost cell of a region
	//		   optValue - Optimal value obtained for the optimal born again decision tree
	//		   currentDepth - current depth of the tree
	//		   depth - Maximal depth allowed for the born again tree
	//
	// Output: Retunrs the current node index
	//
	/*******************************************************************************************************************************************/

	int collectResultDPNewDepth(int indexBottom, int indexTop, int optValue, unsigned int currentDepth, unsigned int depth);

public:

	/*******************************************************************************************************************************************/
	// Description: Builds the born again decision tree (using an exact algorithm), the result is guaranteed to have the smallest size 
	//				and to be faithful
	//
	// Inputs: None
	//
	// Output: None
	//
	/*******************************************************************************************************************************************/

	void buildOptimal();

	/*******************************************************************************************************************************************/
	// Description: Builds the born again decision tree (using an heuristic based on data manufacturing + an oracle). The result is still 
	//				guaranteed to be faithful
	//
	// Inputs: None
	//
	// Output: None
	//
	/*******************************************************************************************************************************************/
	
	void buildHeuristic();

	/*******************************************************************************************************************************************/
	// Description: Displays some statistics about the run
	//
	// Inputs: None
	//
	// Output: None
	//
	/*******************************************************************************************************************************************/
	
	void displayRunStatistics();

	/*******************************************************************************************************************************************/
	// Description: Exports some statistics about the run in a file
	//
	// Inputs: fileName - string representing the output file name
	//
	// Output: None
	//
	/*******************************************************************************************************************************************/

	void exportRunStatistics(std::string fileName);

	/*******************************************************************************************************************************************/
	// Description: Exports the born-again tree in a file
	//
	// Inputs: fileName - string representing the output file name
	//
	// Output: None
	//
	/*******************************************************************************************************************************************/
	
	void exportBATree(std::string fileName);

	// Constructor
	BornAgainDecisionTree(Params * params, RandomForest * randomForest) : params(params), randomForest(randomForest), fspaceOriginal(params, randomForest), fspaceFinal(params, randomForest) {};
};

#endif
