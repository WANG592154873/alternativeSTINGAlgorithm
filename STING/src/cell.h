/*
 * cell.h
 */

#include <math.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>
#include <fstream>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <set>
using namespace std;

#define nbSons 4


//GPS Point
typedef struct gpsPoint GpsPoint;
struct gpsPoint{
	double latitude;
	double longitude;
};

//Object to record in each clusters. It represents a checking
typedef struct checkings Checkings;
struct checkings{
	int userID;
	int poiID;
	GpsPoint *coordinates;
	double time;
	Checkings *next; //next elem of the list
};

//CheckingsID list
typedef struct indexChecks IndexChecks;
struct indexChecks{
	int index;
	IndexChecks *next;
};

typedef struct listIndexCheckings ListIndexCheckings;
struct listIndexCheckings{

	IndexChecks *firstElem;
};


//It represents a cell of the hierachical structure from STING algorthm
class Cell{

private:
	Cell *father;//son s father
	Cell *lu; // son left upper cell
	Cell *ru; //son right upper cell
	Cell *ld; //son left down cell
	Cell *rd; //son right down cell
	Cell **sons; //0-> upper left, 1->upper right, 2->lower left, 3->lower right
	int nbChecks; //NB checkings inside cell
	int nbUsers; //the cell contains nbUsers users
	int nbPOI; //the cell contains nbPOI POI
	GpsPoint *gpsLeftd; //left down gps point from cell
	GpsPoint *gpsRightu; //right upper gps point from cell
	GpsPoint *gpsCenter; //center point from cell
	double cumulLat; //usefull to compute latitude average of the cell
	double cumumLong;//usefull to compute longitude average of the cell
	double width; //Cell s Width
	double heigth; //Cell s Heigth
	int currentLevel;//Curent level in the tree
	bool isCluster; //True if cell is a super POI otherwise False

public:
	set<int>*listUsers; //usefull to compute nbUsers
	set<int>*listPOI; //usefeull to compute nbPOI

	char *id; //Cell s ID

	//Every checkings that contains the Cell. We work with checkingsID because it's lighter
	ListIndexCheckings *checks;

	//constructor
	Cell(bool isRoot, Cell *father, char id, double minLong, double maxLong, double minLat, double maxLat, double centerLong,double centerLat);
	//destructor
	~Cell();

	//Add new checkingsID to the list
	void addChecks(ListIndexCheckings *list, int index, double latitude, double longitude);

	//Delete each elem of the checkings list
	void deleteChecksList(ListIndexCheckings *list);

	GpsPoint *getAVGCenterGPS();
	GpsPoint *getCenterGPS();

	//Init upper left son
	void init_UL();
	//Init upper right son
	void init_UR();
	//Init down left son
	void init_DL();
	//Init down right son
	void init_DR();
	 //init the corresponding cell if i = {0-> upper left, 1->upper right, 2->lower left, 3->lower right}
	void init_son(int i);
	//Compute the distance between two points (lat1,long1) and (long1,long2)
	static double DistanceTo(double lat1, double lon1, double lat2, double lon2);

	//Getters
	Cell* getLd();
	Cell* getLu();
	Cell* getRd();
	Cell* getRu();
	int getCurrentLevel();
	Cell*getFather() ;
	int getNbChecks();
	double getArea();
	double getDensity();
	int getNbPoi();
	int getNbUsers();
	bool getIsCluster();
	Cell* getSons(int i);

	//Setters
	void incrNBChecks();
	void setNbPoi(int nbPoi);
	void setNbUsers(int nbUsers);
	void setIsCluster(bool isCluster);
};
