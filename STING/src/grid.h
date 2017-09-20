/*
 * grid.h
 */
#include "cell.h"
#include <stdio.h>
#include <string.h>

class Grid{

private:
	Cell *root; //The root represents the entire map
	vector<Cell*> *clusters;
	Checkings *tabChecks; //Store every checkings  [checkingsIndex] = checkings
	int nbLvl; //Current nbLvl in the tree

public:
	//Constructor
	Grid(int nbLvl, int nbChecks);
	//Destructor
	~Grid();
	//Load and record every checkings from the checkins file
	void initAllChecks(string checkingsPathFile);
	//Record a checkins inside a cell
	void putChecks(int indexCheck, Cell *root);
	//Determine where to put a checking in a split Cell (a cell is divided in 4 equal parts)
	void putACheckInASplitCell(int indexCheck, Cell *root);
	//Compute clusters for a particular NMax
	void computeClusters(Cell *root, int NMax, int hauteur, bool firstTime, int lvl);
	//Build the output file for each levels (each Nmax)
	void gridFullOut(string path, vector<int> Nmax);
	//Compute clusters for each levels (each Nmax) and build the output file
	void computeClustersAtEachLevels(string path);

	//getter
	Cell* getRoot();
	vector<Cell*>getClusters(int i);

};
