/*
 * grid.cpp
 *
 *  Created on: 17 mai 2017
 *      Author: dosne
 */

#include "grid.h"

Grid::Grid(int nbLvl, int nbChecks)
{

	//We guess that our root is the entire map that wy gps coordinate are -180, 180, -80, 80, 0, 0
	this->root = new Cell(true,NULL,'0',-180,180,-80,80,0,0);
	this->tabChecks = new Checkings[nbChecks];
	this->nbLvl = nbLvl;
	this->clusters = new vector<Cell*>[this->nbLvl];

}


Grid::~Grid()
{
	delete this->root;
	delete this->tabChecks;

	for(int i = 0;i< this->clusters->size();i++)
		this->clusters[i].clear();
	delete [] this->clusters;

}

void Grid::putChecks(int indexCheck,Cell *root)
{
	double longiFromCheck = this->tabChecks[indexCheck].coordinates->longitude;
	double latiFromCheck = this->tabChecks[indexCheck].coordinates->latitude;
	root->addChecks(root->checks, indexCheck,latiFromCheck,longiFromCheck);
	root->listUsers->insert(this->tabChecks[indexCheck].userID);
	root->listPOI->insert(this->tabChecks[indexCheck].poiID);
	root->incrNBChecks();
}

void Grid::putACheckInASplitCell(int indexCheck,Cell *root)
{
	double longiFromCheck = this->tabChecks[indexCheck].coordinates->longitude;
	double latiFromCheck = this->tabChecks[indexCheck].coordinates->latitude;
	int index = -1;

	//thanks to gps coordinates we can find the good cell where to put the check
	if(longiFromCheck< root->getCenterGPS()->longitude && latiFromCheck > root->getCenterGPS()->latitude)
	{
		//0
		index =0;
	}
	else if(longiFromCheck>root->getCenterGPS()->longitude && latiFromCheck > root->getCenterGPS()->latitude)
	{
		//1
		index = 1;
	}
	else if(longiFromCheck<root->getCenterGPS()->longitude && latiFromCheck <root->getCenterGPS()->latitude)
	{
		//2
		index = 2;
	}
	else if(longiFromCheck>root->getCenterGPS()->longitude && latiFromCheck <root->getCenterGPS()->latitude)
	{
		//3
		index = 3;
	}
	if(root->getSons(index)!=NULL && index != -1)
		putChecks(indexCheck, root->getSons(index));
}

vector<Cell*>Grid::getClusters(int i) {
	return clusters[i];
}

Cell* Grid::getRoot() {
	return this->root;
}


void Grid::computeClusters(Cell *root, int NMax, int hauteur, bool firstTime, int lvl)
{
	//If we build the tree for the first time we record some informations
	if(firstTime)
	{
		root->setNbUsers(root->listUsers->size());
		root->setNbPoi(root->listPOI->size());
		//The lists are usefull only to know nbUsers and nbPOI that why we free memory
		delete root->listUsers;
		delete root->listPOI;
	}

	//	cout << "Hauteur : "<<hauteur << " nb users : "<< root->getNbUsers() << " nb POIs : " << root->getNbPoi()<< endl;

	//According to our algorithm if nbUsers < Nmax the current cell is a cluster
	if(root->getNbUsers() < NMax)
	{
		this->clusters[lvl].push_back(root);
		root->setIsCluster(true);
	}

	//According to our algorithm if we are not in a cluster we virtually split the cell in 4
	else
	{
		//If we build the tree for the first time we have put each each checks in the good son
		if(firstTime)
		{

			for(int i = 0;i<nbSons;i++)
				root->init_son(i);
			IndexChecks *index = root->checks->firstElem;
			while(index) //for each checkings of the current cell
			{
				//We put the checking in the good son
				this->putACheckInASplitCell(index->index,root);
				index = index->next;
			}

		}

		//We recursively recall the function for each sons
		for(int i = 0;i<nbSons;i++)
		{
			if(root->getSons(i) != NULL)
				computeClusters(root->getSons(i), NMax,hauteur+1,firstTime,lvl);
		}

	}

}


void Grid::initAllChecks(string path) {

	FILE *fileptr = fopen(path.c_str(),"r");
	int user, poi,time;
	char *gps = new char[50];
	char *gpsCoord[2];
	int checkingsCompt = 0;
	//For each line from the file
	while ((fscanf(fileptr, "%d\t%s\t%d", &user, gps, &time) != EOF)) {
		gpsCoord[0] = strtok(gps, ",");
		gpsCoord[1] = strtok(NULL, ",");

		Checkings check;
		check.userID = user;
		check.poiID  = -1;
		check.time = time;
		GpsPoint *point = new GpsPoint;
		point->latitude =   strtod(gpsCoord[0],NULL);
		point->longitude =   strtod(gpsCoord[1],NULL);
		check.coordinates = point;
		this->tabChecks[checkingsCompt] = check;
		this->putChecks(checkingsCompt,this->root);
		checkingsCompt++;

	}

}


void Grid::gridFullOut(string path, 	vector<int> Nmax) {

	stringstream file_path;
	file_path << path;
	file_path<< "/fullOut";
	IndexChecks *tmp = NULL;
	ofstream myfile (file_path.str());

	if (myfile.is_open())
	{
		for(int lvl = 0; lvl<this->nbLvl;lvl++) //For each levels (each Nmax)
		{
			for(int i = 0;i<this->clusters[lvl].size();i++) //For each clusters
			{
				tmp = this->clusters[lvl][i]->checks->firstElem; //We can find every checkings in the cluster cell
				while(tmp)
				{
					myfile << this->tabChecks[tmp->index].userID <<"\t" << this->clusters[lvl][i]->id << "\t"
							<< this->clusters[lvl][i]->getAVGCenterGPS()->latitude << ","
							<< this->clusters[lvl][i]->getAVGCenterGPS()->longitude << "\t"
							<< this->tabChecks[tmp->index].time
							<<"\t" <<Nmax[lvl] << "\n";
//										  <<"\t" <<this->clusters[lvl].size();
					tmp = tmp->next;
				}
			}
		}
	}

	delete tmp;
	myfile.close();

}


void Grid::computeClustersAtEachLevels(string path)
{

	vector<int> Nmax;
	//For each levels
	for(int i = 0; i<this->nbLvl;i++)
	{
		//The first Nmax value is 2
		if(i == 0)
		{
			Nmax.push_back(2);
			//We build the tree once
			this->computeClusters(this->getRoot(),Nmax[0],0,true,i);
		}

		//The last value is nbUsers+1 corresponding to one super POI wich contains every users
		else if(i == this->nbLvl -1)
		{
			Nmax.push_back(this->root->getNbUsers()+1);
		}

		else
		{
			Nmax.push_back((i+1)* (this->root->getNbUsers()/this->nbLvl));
		}
	}

	//For each Nmax != 2 we compute clusters
	for(int i = 0; i<Nmax.size();i++)
	{
		if(i != 0)
		{
			this->computeClusters(this->getRoot(),Nmax[i],0,false,i);
		}

	}

	cout << "Building output file in "<<path << endl;
	//We build the output file
	this->gridFullOut(path, Nmax);
}
