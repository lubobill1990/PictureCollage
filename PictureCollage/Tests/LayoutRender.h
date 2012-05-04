#ifndef PICCOLLA_LAYOUTRENDER_H
#define PICCOLLA_LAYOUTRENDER_H
#include "Common.h"
class LayoutShowUnit{
public:
	LayoutShowUnit(b2Body *body);
	LayoutShowUnit(LayoutShowUnit &);
	void BindTexture();
	void DrawTexture();
	bee::Point SaclePositionInImage(bee::Point positionInBox2d);
	~LayoutShowUnit();
	bee::PolygonList showArea;
	b2Body *body;
	bee::Picture *picture;
	GLuint texName;
private:
	b2Mat33 s_TransformToScalePosition;
	void CalcTransformToScalePosition();
};
class LayoutRender
{
public:
	LayoutRender(void);
	LayoutRender(b2World *world);
	void init();
	bee::PolygonList CalcBodyShowArea(b2Body *body);
	void CalcEveryBodyShowArea();
	void DrawTexture();
	~LayoutRender(void);
	vector<LayoutShowUnit> m_LayoutShowUnitList;

};

#endif