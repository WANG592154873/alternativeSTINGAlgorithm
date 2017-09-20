/*
 * cell.cpp
 *
 *  Created on: 16 mai 2017
 *      Author: dosne
 */

#include "cell.h"
#include <stdio.h>
#include <string.h>



#define PI 3.14159265

double Cell::DistanceTo(double lat1, double lon1, double lat2, double lon2)
{
	double rlat1 = PI * lat1/180;
	double rlat2 = PI * lat2/180;

	double theta = lon1-lon2;
	double rtheta = PI * theta/180;

	double dist = sin(rlat1) *sin(rlat2) +cos(rlat1) *cos(rlat2) * cos(rtheta);
	dist = acos(dist);
	dist = dist * 180/PI;
	dist = dist * 60 * 1.1515;

	dist = dist * 1.609344;
	return dist;
}

Cell::Cell(bool isRoot, Cell *father, char id, double minLong, double maxLong, double minLat, double maxLat, double centerLong,double centerLat)
{

	listUsers = new set<int>;
	listPOI = new set<int>;

	this->ru = NULL;
	this->rd = NULL;
	this->lu = NULL;
	this->ld = NULL;

	this->sons = new Cell*[nbSons];
	for(int i =0;i<nbSons;i++)
		this->sons[i] = NULL;

	this->checks = new ListIndexCheckings;
	this->checks->firstElem = NULL;
	this->father = father;
	this->nbChecks = 0;
	this->nbPOI = 0;
	this->nbUsers = 0;
	this->width = DistanceTo(minLat,minLong,minLat,maxLong);
	this->heigth = DistanceTo(minLat,minLong,maxLat,minLong);
	this->cumulLat = 0;
	this->cumumLong = 0;
	this->gpsCenter = new GpsPoint;
	this->gpsCenter->latitude = centerLat;
	this->gpsCenter->longitude = centerLong;
	this->gpsLeftd = new GpsPoint;
	this->gpsLeftd->latitude = minLat;
	this->gpsLeftd->longitude = minLong;
	this->gpsRightu = new GpsPoint;
	this->gpsRightu->latitude = maxLat;
	this->gpsRightu->longitude = maxLong;
	this->isCluster = false;

	if(isRoot) //If root init cell s ID with '0'
	{
		this->currentLevel = 0;
		this->id = new char[this->currentLevel+2];
		this->id[this->currentLevel] = '0';
		this->id[this->currentLevel+1] = '\0';
	}
	else //if we are not in the root add number (0, 1 ,2 or 3) to the current cell s father s ID
	{
		this->currentLevel = father->getCurrentLevel()+1;
		this->id = new char[this->currentLevel+2];
		strcpy(this->id,father->id);
		this->id[this->currentLevel] = id;
		this->id[this->currentLevel+1] = '\0';

	}

}

Cell::~Cell(){

	delete this->ru;
	delete this->rd;
	delete this->lu;
	delete this->ld;
	deleteChecksList(this->checks);
	delete this->gpsCenter;
	delete this->gpsLeftd;
	delete this->gpsRightu;
	delete [] this->sons;
}

void Cell::addChecks(ListIndexCheckings *list, int index,double latitude, double longitude)
{

	IndexChecks *newChecks = new IndexChecks;

	newChecks->index = index;
	newChecks->next = list->firstElem;
	list->firstElem = newChecks;
	this->cumulLat += latitude;
	this->cumumLong += longitude;
}


void Cell::deleteChecksList(ListIndexCheckings *list)
{
	IndexChecks *current = new IndexChecks;
	IndexChecks *next = new IndexChecks;
	current = list->firstElem;
	next = current->next;

	while(next) //while there is an element
	{
		delete current;
		current = next;
		next = current->next;
	}

	delete current;
	delete next;

}



GpsPoint* Cell::getAVGCenterGPS()
{
	GpsPoint *toReturn = new GpsPoint;
	toReturn->latitude = this->cumulLat/(double)this->nbChecks;
	toReturn->longitude = this->cumumLong/(double)this->nbChecks;
	return toReturn;
}


void Cell::init_UL(){
	double minLong, maxLat,centerLong,centerLat;
	minLong = this->gpsLeftd->longitude;
	maxLat = this->gpsRightu->latitude;
	centerLong = this->gpsCenter->longitude;
	centerLat = this->gpsCenter->latitude;


	this->sons[0] = new Cell(false, this,'0',minLong,centerLong,centerLat,maxLat,centerLong+((minLong-centerLong)/(double)2),
			centerLat+((maxLat-centerLat)/(double)2)
	);
}
void Cell::init_UR(){
	double maxLong, maxLat,centerLong,centerLat;
	maxLong = this->gpsRightu->longitude;
	maxLat = this->gpsRightu->latitude;
	centerLong = this->gpsCenter->longitude;
	centerLat = this->gpsCenter->latitude;

	this->sons[1] =  new Cell(false, this,'1',centerLong,maxLong,centerLat, maxLat, centerLong+((maxLong-centerLong)/(double)2),
			centerLat+((maxLat-centerLat)/(double)2));
}
void Cell::init_DL(){
	double minLong, minLat,centerLong,centerLat;
	minLong = this->gpsLeftd->longitude;
	minLat = this->gpsLeftd->latitude;
	centerLong = this->gpsCenter->longitude;
	centerLat = this->gpsCenter->latitude;

	this->sons[2] = new Cell(false, this,'2',minLong,centerLong,minLat,centerLat,centerLong+((minLong-centerLong)/(double)2),
			centerLat+((minLat-centerLat)/(double)2));
}
void Cell::init_DR(){
	double maxLong, minLat,centerLong,centerLat;
	minLat = this->gpsLeftd->latitude;
	centerLong = this->gpsCenter->longitude;
	centerLat = this->gpsCenter->latitude;
	maxLong = this->gpsRightu->longitude;

	this->sons[3] = new Cell(false, this,'3',centerLong,maxLong,minLat,centerLat,centerLong+((maxLong-centerLong)/(double)2),
			centerLat+((minLat-centerLat)/(double)2));

}


void Cell::init_son(int i)
{
	switch(i){
	case 0:
		this->init_UL();
		break;
	case 1:
		this->init_UR();
		break;
	case 2:
		this->init_DL();
		break;
	case 3:
		this->init_DR();
		break;
	}
}

//Return heigth * width
double Cell::getArea()
{
	if(this->heigth != 0 && this->width != 0)
		return this->heigth*this->width;
	else
		return 0;
}

//Return density nbUsers / Area
double Cell::getDensity()
{
	if(this->getArea() != 0 && this->nbUsers != 0)
	{
		return (double)this->nbUsers/(double)this->getArea();
	}
	else
	{
		return 0;
	}
}


Cell* Cell::getLd() {
	return this->ld;
}

Cell* Cell::getLu() {
	return this->lu;
}

Cell* Cell::getRd() {
	return this->rd;
}

Cell* Cell::getRu() {
	return this->ru;
}

void Cell::incrNBChecks() {
	this->nbChecks++;

}

GpsPoint* Cell::getCenterGPS() {

	return this->gpsCenter;
}

int Cell::getNbChecks() {
	return nbChecks;
}

Cell* Cell::getFather() {
	return father;
}

int Cell::getCurrentLevel() {
	return currentLevel;
}

int Cell::getNbPoi() {
	return nbPOI;
}

void Cell::setNbPoi(int nbPoi) {
	nbPOI = nbPoi;
}

void Cell::setNbUsers(int nbUsers) {
	this->nbUsers = nbUsers;
}

int Cell::getNbUsers() {
	return nbUsers;
}

bool Cell::getIsCluster() {
	return isCluster;
}

void Cell::setIsCluster(bool isCluster) {
	this->isCluster = isCluster;
}

Cell* Cell::getSons(int i) {
	return sons[i];
}
