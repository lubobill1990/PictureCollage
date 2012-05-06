#include <iostream>
#include "HandleLayoutResult.h"
#include "FixtureData.h"
#include <math.h>
#include "Common.h"
using namespace std;
#include "freeglut/GL/glut.h"


float32 ComputeArea(VecVector vertices){
	if (vertices.size()<=2)
	{
		return 0;
	}
	b2Vec2 center;
	center.Set(0.0f, 0.0f);
	float32 area = 0.0f;
	float32 I = 0.0f;

	// pRef is the reference point for forming triangles.
	// It's location doesn't change the result (except for rounding error).
	b2Vec2 pRef(0.0f, 0.0f);
#if 0
	// This code would put the reference point inside the polygon.
	for (int32 i = 0; i < m_vertexCount; ++i)
	{
		pRef += m_vertices[i];
	}
	pRef *= 1.0f / count;
#endif

	const float32 k_inv3 = 1.0f / 3.0f;

	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		// Triangle vertices.
		b2Vec2 p1 = pRef;
		b2Vec2 p2 = vertices[i];
		b2Vec2 p3 = i + 1 < vertices.size() ? vertices[i+1] : vertices[0];

		b2Vec2 e1 = p2 - p1;
		b2Vec2 e2 = p3 - p1;

		float32 D = b2Cross(e1, e2);

		float32 triangleArea = 0.5f * D;
		area += triangleArea;

		// Area weighted centroid
		center += triangleArea * k_inv3 * (p1 + p2 + p3);

		float32 px = p1.x, py = p1.y;
		float32 ex1 = e1.x, ey1 = e1.y;
		float32 ex2 = e2.x, ey2 = e2.y;

		float32 intx2 = k_inv3 * (0.25f * (ex1*ex1 + ex2*ex1 + ex2*ex2) + (px*ex1 + px*ex2)) + 0.5f*px*px;
		float32 inty2 = k_inv3 * (0.25f * (ey1*ey1 + ey2*ey1 + ey2*ey2) + (py*ey1 + py*ey2)) + 0.5f*py*py;

		I += D * (intx2 + inty2);
	}

	// Total mass

	// Center of mass
	if (area<b2_epsilon)
	{
		area=0;
	}
	return area;
}


bool operator <(const VecPair &a,const VecPair &b){
	return a.first.y<b.first.y;
}

bool operator !=(const b2Vec2&a,const b2Vec2&b){
	return (a.x!=b.x)||(a.y!=b.y);
}

VecVector ReorderVertices(VecVector vertices){
	if (vertices.size()<=2)
	{
		return vertices;
	}
	unsigned int i=0;
	std::vector<b2Vec2> vecVector;
	vecVector.push_back(vertices[0]);

	while(i<vertices.size()-1)
	{
		std::vector<std::pair<b2Vec2 ,b2Vec2> > vec; 
		for (unsigned int j=0;j<vertices.size();++j)
		{
			if (vertices[j]-vecVector[vecVector.size()-1]!=b2Vec2(0,0))//error
			{
				b2Vec2 edge=vertices[j]-vecVector[vecVector.size()-1];
				vec.push_back(std::make_pair(b2Vec2((float32)j,0.0),edge));
			}
		}
		for (uint j=0;j<vec.size();++j)
		{
			float32 angle=atan(vec[j].second.y/(vec[j].second.x==0?b2_epsilon:vec[j].second.x));
			angle=angle*180/b2_pi;
			if (vec[j].second.x<0)
			{
				angle+=180;
			}else if(vec[j].second.y<0){
				angle+=360;
			}
			vec[j].first.y=angle;
		}
		//根据vec.first.y排序,算出之间相差180(pi)以上的两个角
		//所对应的vec.first.x即为下一个点
		//vec.second为邻边的向量
		sort(vec.begin(),vec.end());
		//对排好序的,后一个减去前一个,如果大于180(pi),则后一条边对应的顶点为下一个顶点
		for (uint j=0;j<vec.size();++j)
		{
			float32 angle_diff;
			int next_index=(j==vec.size()-1?0:j+1);
			if (next_index==0)
			{
				angle_diff=vec[next_index].first.y+360-vec[j].first.y;
			}else{
				angle_diff=vec[next_index].first.y-vec[j].first.y;
			}
			if (angle_diff>180)
			{
				vecVector.push_back(vertices[vec[next_index].first.x]);
				++i;
				break;
			}
		}
	}
	return vecVector;
}
/************************************************************************/
/* 计算两个fixture相互交叠的面积                                        */
/************************************************************************/
float GetPolygonFixtureOverlapArea(b2Fixture*fixtureA,b2Fixture*fixtureB){
	b2PolygonShape *shapeA, *shapeB;
	shapeA=(b2PolygonShape*)fixtureA->GetShape();
	shapeB=(b2PolygonShape*)fixtureB->GetShape();
	const b2Transform tA=fixtureA->GetBody()->GetTransform();
	const b2Transform tB=fixtureB->GetBody()->GetTransform();
	bee::Polygon polyA=bee::Polygon(shapeA->m_vertices,shapeA->m_vertexCount,tA);
	bee::Polygon polyB=bee::Polygon(shapeB->m_vertices,shapeB->m_vertexCount,tB);
	bee::PolygonList tmp=polyA.GetOverlapdArea(polyB);
	return tmp.ComputeArea();
}
void HandleContactFixture(b2World *world){
	static int count=0;
	++count;
	if (count<30)
	{
		return;
	}else{
		count=0;
	}
	
	//每三十个step执行一次
	for (b2Body *body=world->GetBodyList();body;body=body->GetNext())
	{	
		//如果是边框,跳过
		if (body->GetType()==0)
		{
			continue;
		}
		for (b2Fixture *fixture=body->GetFixtureList();fixture;fixture=fixture->GetNext())
		{	//对每个fixture,获取和它相碰的fixtureList
			FixtureData* fixture_data=(FixtureData*)fixture->GetUserData();
			if (fixture_data->GetFixtureType()!=inner)
			{
				continue;
			}
			std::vector<b2Fixture*> contacted_fixture_list=fixture_data->GetContactedFixtureList();
			for (uint i=0;i<contacted_fixture_list.size();++i)
			{
				b2Fixture* contacted_body_outer_fixture=contacted_fixture_list[i];
				b2Fixture* contacted_body_inner_fixture=GetAnotherFixtureInSameBody(contacted_body_outer_fixture);
				b2Fixture* outer_fixture=GetAnotherFixtureInSameBody(fixture);
				float32 area_thisouter_contactedinner=GetPolygonFixtureOverlapArea(contacted_body_inner_fixture,outer_fixture);

				//float32 area_thisinner_contactedouter=GetPolygonFixtureOverlapArea(contacted_body_outer_fixture,fixture);
				//只有当另一对有相交面积,本对的存在才有意义,所以另一对如果没有了相交面积,则需要消除本对的contactlist
				if (area_thisouter_contactedinner==0)
				{
					fixture->GetBody()->DestroyFixtureContact(fixture);
					fixture->GetBody()->DestroyFixtureContact(outer_fixture);
					fixture->GetBody()->DestroyFixtureContact(contacted_body_inner_fixture);
					fixture->GetBody()->DestroyFixtureContact(contacted_body_outer_fixture);
					contacted_body_inner_fixture->GetBody()->DestroyFixtureContact(fixture);
					contacted_body_inner_fixture->GetBody()->DestroyFixtureContact(outer_fixture);
					contacted_body_inner_fixture->GetBody()->DestroyFixtureContact(contacted_body_inner_fixture);
					contacted_body_inner_fixture->GetBody()->DestroyFixtureContact(contacted_body_outer_fixture);
					fixture_data->DeleteContactedFixture(contacted_fixture_list[i]);
				}
			}
		}
	}
}

//调整图片的倾斜角度,使之不至于太斜
void AdjustAngle(b2World *world){
	for (b2Body *body=world->GetBodyList();body;body=body->GetNext())
	{			
		//如果是边框,跳过
		if (body->GetType()==0)
		{
			continue;
		}
		float32 angle360=abs(body->GetAngle()-(floor(abs(body->GetAngle()/(2*b2_pi)))))/b2_pi*180;
		float32 body_angle=body->GetAngle();
		//如果图片和水平的夹角超过40度,则设为-30到30度的任一个角度
		if(angle360>40&&angle360<320){
			body->SetTransform(body->GetPosition(),RandomFloat(30/180*b2_pi,-30/180*b2_pi));
		}
		//如果图片和水平夹角超过30度并且小于40度,则向0角度方向施加一个冲量
		else if (angle360>30&&angle360<330)
		{
			float32 nextAngle=body_angle+body->GetAngularVelocity()/60.0;
			float totalRotation = - nextAngle;
			while ( totalRotation < -180 * DEGTORAD ) totalRotation += 360 * DEGTORAD;
			while ( totalRotation >  180 * DEGTORAD ) totalRotation -= 360 * DEGTORAD;
			float desiredAngularVelocity = totalRotation * 60;
			float torque = body->GetInertia() * desiredAngularVelocity ;

			body->ApplyTorque(torque);
		}else{//如果夹角小于30度,则设角速度为0,让其停止角运动
			body->SetAngularVelocity(0);
		}
	}
}

void init (void) 
{
	/* select clearing color 	*/
	glClearColor (0.0, 0.0, 0.0, 0.0);

	/* initialize viewing values  */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

