#ifndef SYNTHETICDATA_H
#define  SYNTHETICDATA_H

#include "../VesselNess/Data3D.h"
#include <random>
#include <cmath>

namespace SyntheticData {
	void Doughout( Data3D<short>& im_short ) {
		const int SX = 30;
		const int SY = 30;
		const int SZ = 30;
		const double r1 = 5.5; // SX * 0.3;
		const double r2 = 0.9;

		im_short.reset( Vec3i(SX, SY, SZ) );

		// center of the doughout
		float cx = (float) SX / 2;
		float cy = (float) SY / 2;
		float cz = (float) SZ / 2 + 0.5f;

		for( int z=0; z<im_short.SZ(); z++ ) {
			float dz = z - cz; // difference in z
			for( int y=0; y<SY; y++ ) for( int x=0; x<SX; x++ ) {
				// difference in x
				float dx = x - cx;
				// difference in y
				float dy = y - cy;

				// distance to the center of the doughout
				double dc = sqrt( 1.0*dx*dx + 1.0*dy*dy );

				if( (dc-r1)*(dc-r1) + dz * dz < r2 * r2 ) {
					im_short.at(x,y,z) = 10000;
				}
			}
		}
	}


	void Spiral( Data3D<short>& im_short ) {
		const int SX = 30;
		const int SY = 30;
		const int SZ = 30;
		const double r1 = 5.5; // SX * 0.3;
		const double r2 = 1.9;

		im_short.reset( Vec3i(SX, SY, SZ) );

		// center of the doughout
		float cx = (float) SX / 2;
		float cy = (float) SY / 2;
		float cz = (float) SZ / 2 + 0.5f;

		for( int z=0; z<im_short.SZ(); z++ ) for( int y=0; y<SY; y++ ) for( int x=0; x<SX; x++ ) {
			double dz = z - cz + atan2( 1.0*(y-cy), 1.0*(x-cx) ); // difference in z
			// difference in x
			float dx = x - cx;
			// difference in y
			float dy = y - cy;

			// distance to the center of the doughout
			double dc = sqrt( 1.0*dx*dx + 1.0*dy*dy );

			if( (dc-r1)*(dc-r1) + dz * dz < r2 * r2 ) {
				im_short.at(x,y,z) = 10000;
			}
		}
	}

	void Stick( Data3D<short>& im_short ) {
		im_short.reset( Vec3i(20,20,20) );
		for( int i=2; i<18; i++ ) {
			im_short.at(i,  i,  i)   = 10000;
			im_short.at(i,  i,  i+1) = 10000;
			im_short.at(i,  i+1,i)   = 10000;
			im_short.at(i+1,i,  i)   = 10000;
			im_short.at(i+1,i+1,i+1) = 10000;
		}
	}

	void Yes( Data3D<short>& im_short, const float radius = 3.0f ) {
		const int SX = 30;
		const int SY = 30;
		const int SZ = 30;

		std::vector<cv::Vec6f> intervals;
		intervals.push_back( Vec6f(SX*0.5f, SY*0.5f, SZ*0.0f, SX*0.5f, SY*0.5f, SZ*0.5f) );
		intervals.push_back( Vec6f(SX*0.0f, SY*0.5f, SZ*1.0f, SX*0.5f, SY*0.5f, SZ*0.5f) );
		intervals.push_back( Vec6f(SX*1.0f, SY*0.5f, SZ*1.0f, SX*0.5f, SY*0.5f, SZ*0.5f) );

		im_short.reset( Vec3i(SX, SY, SZ), 0 );
		for( int z=0; z<im_short.SZ(); z++ ) for( int y=0; y<SY; y++ ) for( int x=0; x<SX; x++ ) {
			Vec3f point( 1.0f*x, 1.0f*y, 1.0f*z );
			for( unsigned i=0; i<intervals.size(); i++ ) {
				// distant from line to point
				Vec3f pos = cv::Vec3f( &intervals[i][0] );
				Vec3f dir = cv::Vec3f( &intervals[i][3] ) - cv::Vec3f( &intervals[i][0] );
				float t = ( point - pos ).dot( dir ) / dir.dot( dir );

                // TODO: why the following line of code do not compile in g++?
				// t = std::min(1.0f, std::max(0.0f, t));
				if( t<0.0f ) t = 0.0f;
				if( t>1.0f ) t = 1.0f;

				Vec3f proj = pos + dir * t;
				Vec3f proj_point = proj - point;
				float ratio = radius - sqrt( proj_point.dot( proj_point ) );

				// todo: why the following line of code do not compile in g++
				// ratio = min(1.0f, max( 0.0f, ratio ));
				if( ratio<0.0f ) ratio = 0.0f;
				if( ratio>1.0f ) ratio = 1.0f;

				im_short.at(x,y,z) = max( im_short.at(x,y,z), short(ratio * 1000) );
			}
		}
	}
}

#endif // SYNTHETICDATA_H

