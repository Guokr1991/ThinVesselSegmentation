// ModelFitting.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Line3D.h"

// For multithreading
#include <Windows.h>
#include <process.h>
#include <iomanip>

// This project is build after VesselNess. 
// Some of the building blocks (Data3D, Visualization) are borrowed from VesselNess. 
#include "Data3D.h" 
#include "GLViwerModel.h"
#include "MinSpanTree.h" 

// For the use of graph cut
#include "GCoptimization.h"
typedef GCoptimization GC; 

#include "Line3DTwoPoint.h" 
#include "LevenburgMaquart.h" 
#include "GraphCut.h"
#include "SyntheticData.h" 

// for visualization
GLViwerModel ver;

void visualization_func( void* data ) {
	GLViwerModel& ver = *(GLViwerModel*) data; 
	ver.go();
}


#include <assert.h>
#include <iostream>



const double LOGLIKELIHOOD = 0.01; 

int main(int argc, char* argv[])
{
	CreateDirectory(L"./output", NULL);
	
	Data3D<short> im_short;
	//Synthesic Data
	im_short.reset( Vec3i(20,20,20) ); 
	for( int i=5; i<15; i++ ) {
		im_short.at(i,  i,  i)   = 10000; 
		im_short.at(i,  i,  i+1) = 10000; 
		im_short.at(i,  i+1,i)   = 10000; 
		im_short.at(i+1,i,  i)   = 10000; 
	}
	//im_short.at(5, 4, 14) = 10000; 
	// im_short.at(5, 5, 15) = 10000; 
	// im_short.at(5, 6, 15) = 10000; 
	//im_short.at(6, 5, 16) = 10000; 
	// OR real data
	//im_short.load( "../data/data15.data" );
	// make a doghout
	// SyntheticData::Doughout( im_short ); 

	// threshold the data and put the data points into a vector
	Data3D<int> indeces;
	vector<cv::Vec3i> dataPoints;
	IP::threshold( im_short, indeces, dataPoints, short(4500) );
	if( dataPoints.size()==0 ) return 0; 

	GLViewer::GLLineModel *model = new GLViewer::GLLineModel( im_short.get_size() );
	ver.objs.push_back( model );

	//////////////////////////////////////////////////
	// create a thread for rendering
	//////////////////////////////////////////////////
	HANDLE thread_render = NULL; 
	thread_render = (HANDLE) _beginthread( visualization_func, 0, (void*)&ver ); 
	
	model->updatePoints( dataPoints ); 

	//////////////////////////////////////////////////
	// Line Fitting
	//////////////////////////////////////////////////
	// Initial Samplings
	const int num_init_labels = (int) dataPoints.size(); 
	vector<Line3D*> lines; 
	for( int i=0; i<num_init_labels; i++ ) {
		Line3DTwoPoint *line  = new ::Line3DTwoPoint();
		Vec3i randomDir = Vec3i(
				rand() % 100, 
				rand() % 100, 
				rand() % 100 ); 
		line->setPositions( dataPoints[i], dataPoints[i] + randomDir ); 
		lines.push_back( line ); 
	}
	
	vector<int> labelings = vector<int>( dataPoints.size(), 0 ); 
	for( int i=0; i<num_init_labels; i++ ) labelings[i] = i; 

	model->updateModel( lines, labelings ); 
	
	Sleep( 20000 ); 

	cout << "Graph Cut Begin" << endl; 
	try{
		// keep track of energy in previous iteration
		GC::EnergyType energy_before = -1;

		// TODO: let's run the algorithm for only one iteration for now
		for( int i=0; i<1; i++ ) { 
			// TODO: let's not have background model for now. We will add background model later
			
			// GC::EnergyType energy = GraphCut::estimation( dataPoints, labelings, lines ); 
			
			model->updateModel( lines, labelings ); 
			LevenburgMaquart::reestimate( dataPoints, labelings, lines, indeces ); 
		}
	}
	catch (GCException e){
		e.Report();
	}
	
	cout << "Main Thread is Done. " << endl; 
	WaitForSingleObject( thread_render, INFINITE);

	for( int i=0; i<num_init_labels; i++ ){
		delete lines[i]; 
		lines[i] = NULL; 
	}

	return 0; 
}