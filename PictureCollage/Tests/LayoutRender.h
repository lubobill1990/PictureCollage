#ifndef PICCOLLA_LAYOUTRENDER_H
#define PICCOLLA_LAYOUTRENDER_H
#include "Common.h"
class LayoutShowUnit{
public:
	LayoutShowUnit(b2Body *body);
	LayoutShowUnit(const LayoutShowUnit &);
	void BindTexture();
	void DrawTexture();
	void DrawFullTexture();
	bee::Polygon GetPictureTotalShowArea();
	bee::Point GetCoordinateInPictureFromWorld(bee::Point positionInBox2d);
	~LayoutShowUnit();
	bee::PolygonList m_ShowArea;
	bee::Polygon m_pictureTotalShowArea;
	b2Body *body;
	bee::Picture *picture;
	GLuint texName;
private:
	b2Vec3 xDirectionVector;
	b2Vec3 yDirectionVector;
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