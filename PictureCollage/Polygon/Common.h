#ifndef POLYGON_COMMON_H
#define POLYGON_COMMON_H
#include <vector>
#include <utility>
#include <sstream>
#include <string>
#include <iostream>
#include <ProjectCommon.h>
#include <Polygon/Basic.h>
#include <freeglut/GL/glut.h>
typedef float float32;
#define DEBUG_MODE 1
#define	RAND_LIMIT	32767
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#ifdef DEBUG_MODE
#define DEBUG_PRINT(x) std::cout<<__FILE__<<__LINE__<<(x)<<std::endl;
#else
#define DEBUG_PRINT(x) ;
#endif

inline bee::Point GetWorldPoint(const bee::Point& local_point,const bee::Transform &xf){
	return bee::Point(b2Mul(b2Transform(xf.position,xf.R),b2Vec2(local_point.x,local_point.y)));
}

bool TestLineIntersect(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float &InterX, float &InterY);

#include <Polygon/Polygon.h>
#endif
