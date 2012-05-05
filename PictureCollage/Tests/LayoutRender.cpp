#include "LayoutRender.h"

LayoutRender::LayoutRender( b2World *world )
{
	for (b2Body *body=world->GetBodyList();body;body=body->GetNext()){
		//不算相框
		if (body->GetType()==b2_staticBody)
		{
			continue;
		}
		LayoutShowUnit lr=LayoutShowUnit(body);
		this->m_LayoutShowUnitList.push_back(lr);
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
			//fixturePolygon->Draw(1,0,bee::Color(0.1,0.5,0.5));
			std::vector<b2Fixture*> contactlist=fixData->GetContactedFixtureList();
			for (uint i=0;i<contactlist.size();++i)
			{
				b2PolygonShape *tmpShape=(b2PolygonShape *)(contactlist[i]->GetShape());
				b2Transform tx=contactlist[i]->GetBody()->GetTransform();
				bee::Polygon *poly=new bee::Polygon(tmpShape->m_vertices,tmpShape->m_vertexCount,tx);
				contactPolygons.AddPolygon(poly);
			}
			bee::PolygonList polygonToShow=fixturePolygon->Segment(contactPolygons);
			polygonToShow.Draw(1,0);
			return polygonToShow;
		}
	}
	return retVal;
}

void LayoutRender::CalcEveryBodyShowArea()
{
	for (uint i=0;i<this->m_LayoutShowUnitList.size();++i)
	{
		bee::PolygonList tmpList=this->CalcBodyShowArea(this->m_LayoutShowUnitList[i].body);
		this->m_LayoutShowUnitList[i].showArea=bee::TransPolygonListToConvex(tmpList);
		this->m_LayoutShowUnitList[i].DrawTexture();
	}
}

void LayoutRender::DrawTexture()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (uint i=0;i<this->m_LayoutShowUnitList.size();++i)
	{
		this->m_LayoutShowUnitList[i].DrawTexture();
	}
	glFlush();
}

void LayoutRender::init()
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

LayoutShowUnit::LayoutShowUnit( b2Body *body )
{
	this->body=body;
	this->picture=((BodyData*)body->GetUserData())->m_Image;
	//this->picture->LoadImageData();
	this->texName=0;
}

LayoutShowUnit::LayoutShowUnit(const LayoutShowUnit & copyInstance)
{
	this->body=copyInstance.body;
	this->showArea=copyInstance.showArea;
	this->picture=copyInstance.picture;
	this->texName=copyInstance.texName;
}

LayoutShowUnit::~LayoutShowUnit()
{

}
#define checkImageWidth 64
#define checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLuint texName;

void makeCheckImage(void)
{
	int i, j, c;

	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ((((i&0x8)==0)^((j&0x8))==0))*255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
			checkImage[i][j][3] = (GLubyte) 255;
		}
	}
}

void init2(void)
{    
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	makeCheckImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
		checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
		checkImage);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(2.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(2.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(0.0, -1.0, 0.0);

	glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(2.41421, 1.0, -1.41421);
	glTexCoord2f(1.0, 0.0); glVertex3f(2.41421, -1.0, -1.41421);
	glEnd();
	glFlush();
	glDisable(GL_TEXTURE_2D);
}
void LayoutShowUnit::DrawTexture()
{
	this->BindTexture();
	this->CalcTransformToScalePosition();

	glEnable(GL_TEXTURE_2D);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D,this->texName);
	for (uint i=0;i<this->showArea.Size();++i)
	{
		bee::Polygon &curPolygon=this->showArea[i];
		glBegin(GL_POLYGON);
		for (uint j=0;j<this->showArea[i].Size();++j)
		{
			bee::Point p=this->GetCoordinateInPictureFromWorld(curPolygon[j].start);
			glTexCoord2f(p.x, p.y); glVertex3f(curPolygon[j].start.x, curPolygon[j].start.y, 0.0);
		}
		glEnd();
		glFlush();
	}

	glDisable(GL_TEXTURE_2D);
}

void LayoutShowUnit::BindTexture()
{
	if (this->texName>0)
	{
		return;
	}
	glGenTextures(1, &this->texName);
	glBindTexture(GL_TEXTURE_2D, this->texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->picture->width, 
		this->picture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
		this->picture->imgData);
}

void LayoutShowUnit::CalcTransformToScalePosition()
{
	b2Fixture *outsideFixture=GetOutsideFixture(this->body);
	b2PolygonShape *polygonShape=(b2PolygonShape*)outsideFixture->GetShape();
	b2Transform tx=outsideFixture->GetBody()->GetTransform();
	bee::Point b1=GetWorldPoint(polygonShape->m_vertices[0],tx);
	bee::Point b2=GetWorldPoint(polygonShape->m_vertices[1],tx);
	bee::Point b3=GetWorldPoint(polygonShape->m_vertices[3],tx);
	b2Mat33 transform_matrix(b2Vec3(b1.x,b2.x,b3.x),b2Vec3(b1.y,b2.y,b3.y),b2Vec3(1,1,1));
	this->xDirectionVector=transform_matrix.Solve33(b2Vec3(0,1,0));
	this->yDirectionVector=transform_matrix.Solve33(b2Vec3(0,0,1));
}

bee::Point LayoutShowUnit::GetCoordinateInPictureFromWorld( bee::Point positionInBox2d )
{
	b2Vec3 vec(positionInBox2d.x,positionInBox2d.y,1);
	return bee::Point(b2Dot(vec,this->xDirectionVector),b2Dot(vec,yDirectionVector));
}
