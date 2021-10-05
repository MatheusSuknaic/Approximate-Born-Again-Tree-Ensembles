# Approximate Born Again Tree Ensembles

This repository contains the source code and data associated to the master's dissertation "Approximate Born Again Tree ensembles" (.pdf found in the root of this repository). This code is an expansion  from the work of Thibaut Vidal and Maximiliam Schiffer: 

``Vidal, T. and Schiffer, M. (2020). Born-again tree ensembles. In III, H. D. and Singh, A., editors, Proceedings of the 37th International Conference on Machine Learning, volume 119 of Proceedings of Machine Learning Research, pages 9743–9753. PMLR.``

The source code from this paper can be found in <a href=https://github.com/vidalt/BA-Trees></a>. 

## Folder Structure

The repository contains the following folders:

src<br>      |-------born_again_dp<br>     |-------resources<br>

#### src\born_again_dp:

* This folder contains the C++ implementation of the optimal and heuristic BA-Tree algorithms (done by Thibaut Vidal, Maximiliam Schiffer and Toni Pacheco) and the the C++ implementation of the optimal and heuristic approximate BA-Tree algorithms.

#### src\resources

* This folder contains two subfolders, one (datasets) contains the original data sets as well as the training data and test data subsets created by ten-fold cross validiation. 
* The other (forests) contains the random forests which have been generated with scikit-learn v0.22.1 from these data sets. These forests are used as input to the BA-trees algorithms.


## Installation

* Run the make command: `make` on the folder src\born_again_dp. This requires the availability of the g++ compiler.<br> 

By default, the simple makefile provided in this project does not link with CPLEX to facilitate installation and portability.
As a consequence, the call to the MIP solver to prove faithfulness of a region in the heuristic BA-tree is deactivated (USING_CPLEX = false).
To compile with CPLEX and guarantee faithfulness in the heuristic, make sure that CPLEX is installed in your system, adapt the makefile with the correct library path, and run the command "make withCPLEX=1".


## Use of the C++ algorithm:

After compilation, the executable can be directly run on any input file representing a tree ensemble with the following command line:

```
Usage:
   ./bornAgain input_ensemble_path output_BAtree_path [list of options]
Available options:
  -trees X      Limits the number of trees read by the algorithm from the input file
  -obj X	       Objective used in the algorithm: 0 = Depth ; 1 = NbLeaves ; 2 = Depth then NbLeaves ; 4 = Heuristic BA-Tree (defaults to 4); 5 = Approximate BA considering depth; 6 = Beam Search Heuristic; 7 = Greedy Heuristic
  -seed X       Defines the random seed (defaults to 0)
  -value X Cell value used in the algorithm: 0 = Same value; 1 = Volume; 2 = Empirical Measure
  -depth X Maximum depth of the approximate BA tree
  -samples Number of samples for the heuristic objective (obj = 4) 
```
Examples: <br>
`./bornAgain ../resources/forests/FICO/FICO.RF1.txt my_output_file`<br>
`./bornAgain ../resources/forests/COMPAS-ProPublica/COMPAS-ProPublica.RF7.txt my_output_file -trees 30 -obj 5 -value 2 -depth 3`<br>
`./bornAgain ../resources/forests/monks-1/monks-1.RF5.txt my_output_file -trees 21 -obj 4 -seed 89742 -samples 1000`<br>

## Code Structure

The algorithm is structured in the following way:
* **Commandline**: Parses the line of command.
* **Params**: Contains the method parameters and instance data. 
* **TreeEnsemble**: Creates the representation of the tree ensemble given by the input forest file.
* **RandomForest**: Creates the representation of a random forest (subclass of TreeEnsemble) and implements the sample classification criterium.
* **Fspace**: Creates the representation of the tree ensemble's feature space (divided in cells) and, methods to classify and attribute value to the cells.
* **BornAgain**: Displays the statistics and exports the constructed approximate born again tree.
* **BornAgain_Exact**: Constructs the approximate born again tree from the ensemble using exact methods (inherited from BornAgain).
* **BornAgain_Heuristic**: Constructs the approximate born again tree from the ensemble using heuristic methods (inherited from BornAgain).
* **Test**: Measures the adherence of the approximate born again tree compared to the original tree ensemble.

## Team

Contributors to this code:
* <a href="https://github.com/MatheusSuknaic" target="_blank">`Matheus Suknaic`</a>
* <a href="https://github.com/mxschffr" target="_blank">`Maximiliam Schiffer`</a>
* <a href="https://github.com/toni-tsp" target="_blank">`Toni Pacheco`</a>
* <a href="https://github.com/vidalt" target="_blank">`Thibaut Vidal`</a>

## License

[![License](http://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)

- **[MIT license](http://opensource.org/licenses/mit-license.php)**
- Copyright 2020 © <a href="http://fvcproductions.com" target="_blank">Matheus Suknaic, Maximilian Schiffer, Toni Pacheco and Thibaut Vidal</a>.
