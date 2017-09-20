
using namespace std;
#include "grid.h"

int main(int argc, char *argv[]) {

	//<nb Lvl><nb checks> <path check file> <path output>

	cout << "Begin..." << endl;
	//Constructor
	Grid *grid = new Grid(atoi(argv[1]), atoi(argv[2]));
	//Read each checkins from the file and store datas
	cout << "Read and record every checkins from "  <<  argv[3] << endl;
	grid->initAllChecks(argv[3]);
	cout << "Compute clusters, nb lvl = " << argv[1] << endl;
	//Compute clusters at each levels (for different NMax values) an build the output file
	grid->computeClustersAtEachLevels(argv[4]);
	cout << "... End" << endl;

		return 0;
}
