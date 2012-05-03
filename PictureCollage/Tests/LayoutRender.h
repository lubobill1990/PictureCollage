#ifndef PICCOLLA_LAYOUTRENDER_H
#define PICCOLLA_LAYOUTRENDER_H
#include "Common.h"
class LayoutShowUnit{
public:
	LayoutShowUnit(b2Body *body);
	LayoutShowUnit(LayoutShowUnit &);
	~LayoutShowUnit();
	bee::PolygonList showArea;
	b2Body *body;
};
class LayoutRender
{
public:
	LayoutRender(void);
	LayoutRender(b2World *world);
	bee::PolygonList CalcBodyShowArea(b2Body *body);
	void CalcEveryBodyShowArea();
	~LayoutRender(void);
	vector<LayoutShowUnit> m_LayoutShowUnitList;

};



#endif
