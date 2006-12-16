#include "pcserver.h"
#include <orcaice/orcaice.h>
#include <cmath>

using namespace orca;
using namespace std; 

PointCloudProducerGeometryPtr
orca::PointCloudServer::getGeometry(const Ice::Current& current)
{
  PointCloudProducerGeometryPtr retval = new PointCloudProducerGeometry(); 
  orcaice::setInit(retval->offset); 
  orcaice::setInit(retval->size);
  return retval;  
}

void PointCloudServer::generateSphere(vector<float> *points){
	for(float u=0; u < 1.0; u+=0.01){
          for(float v=0; v < 1.0; v+=0.01){
		points->push_back(500*cos(u*2*M_PI)*cos(v*M_PI-M_PI/2));
		points->push_back(500*sin(u*2*M_PI)*cos(v*M_PI-M_PI/2));
		points->push_back(500*sin(v*M_PI-M_PI/2)); 
		points->push_back(u);
		points->push_back(v); 
		points->push_back(1-u/2-v/2); 
	  }
      }
}

PointCloudPtr
PointCloudServer::getPointCloud(const Ice::Current& current)
{
  PointCloudPtr retval = new PointCloud;
  retval->type = PCXYZRGB;  
  if(currentPointCloud.size() == 0){
    //Read it in from the file "Pointcloud.wrl"
    cerr << "Reading from " << fileName << endl;
    if(fileName == NULL){
	generateSphere(&currentPointCloud);
   }
   else {
     int numPoints = readFromVRML(fileName, &currentPointCloud);
     cerr << "Num points read = " << numPoints;
    }
  }
  retval->points = currentPointCloud;
  return retval;
}

void
orca::PointCloudServer::subscribe(const ::orca::PointCloudConsumerPrx& consumer,
				     const Ice::Current& current)
{
 // Does nothing at the moment. 
}

void
orca::PointCloudServer::unsubscribe(const ::orca::PointCloudConsumerPrx& consumer,
				       const Ice::Current& current)
{
 // Does nothing at the moment. 
}

int orca::PointCloudServer::readFromVRML(const char * filename, vector <float> * points)
{
	// Code courtesy of Raymond Sheh (rsheh@cse.unsw.edu.au). Modified by Waleed Kadous (waleed@cse.unsw.edu.au)
	FILE * wrlFile = fopen(filename, "r"); 
	int succeed = 1, i, numReadPoints;
	if (wrlFile == NULL) return -1; 
	fscanf(wrlFile, "#VRML V2.0 utf8\n"); 
	fscanf(wrlFile, "# Automatically generated VRML file of a pointcloud\n"); 
	fscanf(wrlFile, "# Generated by MapManager, a component of the 2006 \n"); 
	fscanf(wrlFile, "# Team CASualty RoboCup Rescue Robot League entry\n"); 
	fscanf(wrlFile, "# (c) 2006 Raymond Sheh\n"); 
	fscanf(wrlFile, "Viewpoint \n"); 
	fscanf(wrlFile, "{\n"); 
	fscanf(wrlFile, "  description \"View points\"\n"); 
	fscanf(wrlFile, "  position 0 0 0\n"); 
	fscanf(wrlFile, "}\n"); 
	fscanf(wrlFile, "NavigationInfo \n"); 
	fscanf(wrlFile, "{\n"); 
	fscanf(wrlFile, "  type [ \"EXAMINE\" \"ANY\" ]\n"); 
	fscanf(wrlFile, "}\n"); 
	fscanf(wrlFile, "Group \n"); 
	fscanf(wrlFile, "{\n"); 
	fscanf(wrlFile, "  children \n"); 
	fscanf(wrlFile, "  [\n"); 
	fscanf(wrlFile, "    Shape \n"); 
	fscanf(wrlFile, "    {\n"); 
	fscanf(wrlFile, "geometry PointSet"); 
	fscanf(wrlFile, "      {\n"); 
	fscanf(wrlFile, "        coord Coordinate \n"); 
	fscanf(wrlFile, "        {\n"); 
	fscanf(wrlFile, "          point \n"); 
	fscanf(wrlFile, "          [\n"); 
	// Now read a pile of points in "x y z,\n"
	i = 0; 
	while (succeed)
	{
		float newX, newY, newZ; 
		succeed = fscanf(wrlFile, "%f %f %f,\n", &newX, &newY, &newZ); 
		/*
		(*points)[i*6+0] = newX; 
		(*points)[i*6+1] = newY; 
		(*points)[i*6+2] = newZ; 
		*/
		(*points).push_back(newX); 
		(*points).push_back(newY); 
		(*points).push_back(newZ); 
		// Allocate space for colours
		(*points).push_back(0); 
		(*points).push_back(0); 
		(*points).push_back(0); 

		i ++; 
	}
	numReadPoints = i-1; 
	fscanf(wrlFile, "          ]\n"); 
	fscanf(wrlFile, "        }\n"); 
	fscanf(wrlFile, "        color Color\n"); 
	fscanf(wrlFile, "        {\n"); 
	fscanf(wrlFile, "          color\n"); 
	fscanf(wrlFile, "          [\n"); 
	// Now read a pile of colors in "r g b,\n"
	i = 0; 
	succeed = 1; 
	while (succeed)
	{
		float newR, newG, newB; 
		succeed = fscanf(wrlFile, "%f %f %f,\n", &newR, &newG, &newB); 
		(*points)[i*6+3] = newR; 
		(*points)[i*6+4] = newG; 
		(*points)[i*6+5] = newB; 
		i ++; 
	}
	if (fclose(wrlFile) < 0) return -1; 
	if (numReadPoints != i-1) return -1; // mismatch
	return numReadPoints; 
}

PointCloudServer::PointCloudServer(char *in_fileName){
  fileName = in_fileName;
}

