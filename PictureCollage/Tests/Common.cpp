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


void WindowPos(GLint xx,GLint yy)
{/*old method */
	GLint viewport[4];
	GLdouble modelview[16],projection[16];
	GLdouble x,y,z;
	glGetIntegerv(GL_VIEWPORT,viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
	glGetDoublev(GL_PROJECTION_MATRIX,projection);

	gluUnProject(xx,yy,0.5,modelview,projection,viewport,&x,&y,&z);
	glRasterPos3d(x,y,z);

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

void DrawString(int x, int y, const char *fmt, ...)
{
	char buffer[128];
	va_list arg;
	va_start(arg, fmt);
	vsprintf(buffer, fmt, arg);
	va_end(arg);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.9f, 0.6f, 0.6f);
	WindowPos(x,y);
	int32 length = (int32)strlen(buffer);
	for (int32 i = 0; i < length; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buffer[i]);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
b2Fixture * GetOutsideFixture( b2Body *body )
{
	for (b2Fixture *fixture=body->GetFixtureList();fixture;fixture=fixture->GetNext())
	{
		if(((FixtureData *)fixture->GetUserData())->GetFixtureType()==outer){
			return fixture;
		}
	}
	return NULL;
}

b2Fixture * GetInsideFixture( b2Body *body )
{
	for (b2Fixture *fixture=body->GetFixtureList();fixture;fixture=fixture->GetNext())
	{
		if(((FixtureData *)fixture->GetUserData())->GetFixtureType()==inner){
			return fixture;
		}
	}
	return NULL;
}

void DrawPoint( const bee::Point &p )
{

}
