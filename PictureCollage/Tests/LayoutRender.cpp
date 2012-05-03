#include "LayoutRender.h"

LayoutRender::LayoutRender( b2World *world )
{
	for (b2Body *body=world->GetBodyList();body;body=body->GetNext()){
		//不算相框
		if (body->GetType()==b2_staticBody)
		{
			continue;
		}
		this->m_LayoutShowUnitList.push_back(LayoutShowUnit(body));
	}
}


LayoutRender::~LayoutRender(void)
{
}

// 获取body经过重要区域约束后的显示区域
//1. 获取
bee::PolygonList LayoutRender::CalcBodyShowArea( b2Body *body )
{
	bee::PolygonList retVal;
	for (b2Fixture *fixture=body->GetFixtureList();fixture;fixture=fixture->GetNext())
	{
		FixtureData* fixData=((FixtureData*)fixture->GetUserData());
		if (fixData->GetFixtureType()==inner)//只取内部多边形的碰撞多边形
		{
			b2Fixture *outerFixture=GetAnotherFixtureInSameBody(fixture);
			b2PolygonShape* fixtureShape=((b2PolygonShape*)outerFixture->GetShape());
			b2Transform fx=outerFixture->GetBody()->GetTransform();
			bee::Polygon *fixturePolygon=new bee::Polygon(fixtureShape->m_vertices,fixtureShape->m_vertexCount,fx);
			bee::PolygonList contactPolygons;
			fixturePolygon->Draw(1,1,bee::Color(0.1,0.5,0.5));
			std::vector<b2Fixture*> contactlist=fixData->GetContactedFixtureList();
			for (uint i=0;i<contactlist.size();++i)
			{
				b2PolygonShape *tmpShape=(b2PolygonShape *)(contactlist[i]->GetShape());
				b2Transform tx=contactlist[i]->GetBody()->GetTransform();
				bee::Polygon *poly=new bee::Polygon(tmpShape->m_vertices,tmpShape->m_vertexCount,tx);
				contactPolygons.AddPolygon(poly);
			}
			bee::PolygonList polygonToShow=fixturePolygon->Segment(contactPolygons);
			polygonToShow.Draw(1,1);
			return polygonToShow;
		}
	}
	return retVal;
}

void LayoutRender::CalcEveryBodyShowArea()
{
	for (uint i=0;i<this->m_LayoutShowUnitList.size();++i)
	{
		this->m_LayoutShowUnitList[i].showArea=this->CalcBodyShowArea(this->m_LayoutShowUnitList[i].body);
	}
}

LayoutShowUnit::LayoutShowUnit( b2Body *body )
{
	this->body=body;
}

LayoutShowUnit::LayoutShowUnit( LayoutShowUnit & copyInstance)
{
	this->body=copyInstance.body;
	this->showArea=copyInstance.showArea;
}

LayoutShowUnit::~LayoutShowUnit()
{

}
