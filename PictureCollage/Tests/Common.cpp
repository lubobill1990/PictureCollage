#include "Common.h"
#include <iostream>
float32 zoomScale=1;
int texCount=30;
b2Fixture *GetAnotherFixtureInSameBody(b2Fixture *fixture)
{
	b2Body *body=fixture->GetBody();
	for (b2Fixture *tmpFixture=body->GetFixtureList();tmpFixture!=NULL;tmpFixture=tmpFixture->GetNext())
	{
		if (tmpFixture!=fixture)
		{
			return tmpFixture;
		}
	}
	throw MsgException("Another fixture not exist");
}
/// Random floating point number in range [lo, hi]
inline float32 RandomFloat(float32 lo, float32 hi)
{
	float32 r = (float32)(rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (hi - lo) * r + lo;
	return r;
}


using namespace std;
void PrintFixture(b2Fixture* fixture){
	cout<<((BodyData*)fixture->GetBody()->GetUserData())->m_Id<<" "<<(((FixtureData*)fixture->GetUserData())->GetFixtureType()==outer?"outer":"inner");
}
extern b2Vec2 ConvertWorldToScreen(float32 x,float32 y);
void DrawCoordinary(){
	for (float i=-40;i<=40;i+=2)
	{
		std::string str=Num2Str((int)i);
		b2Vec2 screen_posion_x=ConvertWorldToScreen(i,0.0);
		b2Vec2 screen_posion_y=ConvertWorldToScreen(0.0,i);
		DrawString(screen_posion_x.x,screen_posion_x.y,str.c_str());
		DrawString(screen_posion_y.x,screen_posion_y.y,str.c_str());
	}
}
