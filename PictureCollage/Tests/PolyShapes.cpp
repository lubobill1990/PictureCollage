#include "PolyShapes.h"
#include <iostream>
#define SCALAR ((float32)50)
#include <freeglut/GL/glut.h>
#include "LayoutRender.h"

float32 testDistance(b2Fixture* fixtureA,b2Fixture *fixtureB)
{
	b2DistanceInput input;
	input.proxyA.Set(fixtureA->GetShape());
	input.proxyB.Set(fixtureB->GetShape());
	input.transformA = fixtureA->GetBody()->GetTransform();
	input.transformB = fixtureB->GetBody()->GetTransform();
	input.useRadii = true;
	b2SimplexCache cache;
	cache.count = 0;
	b2DistanceOutput output;
	b2Distance(&output, &cache, &input);
	return output.distance;
}
float32 picApproxWidthInBox2d,picApproxHeightInBox2d;
PolyShapes::PolyShapes()
{
	this->body_num=0;
	this->inputFiles=bee::PictureIO::GetPictureVector("D:\\pics");
	this->pictureCount=this->inputFiles.size();
	this->canvasWidth=50;
	this->canvasHeight=70;
	float32 leftbottomx=-this->canvasWidth/2;
	float32 leftbottomy=0;

	float32 unitAreaLength=1.0/b2InvSqrt(this->canvasHeight*this->canvasWidth/this->pictureCount);
	picApproxWidthInBox2d=unitAreaLength/b2InvSqrt(4.0/3);
	picApproxHeightInBox2d=unitAreaLength/b2InvSqrt(3.0/4);
	//load image and init position
	for (uint i=0;i<this->pictureCount;++i){
		this->inputFiles[i].LoadImageData();

		float32 length=picApproxWidthInBox2d*(i+1)*0.6;
		uint row=floor(length/this->canvasWidth);
		float32 x=leftbottomx+length-row*this->canvasWidth-picApproxWidthInBox2d/2;
		float32 y=leftbottomy+(row+0.5)*picApproxHeightInBox2d;
		this->initialPosition.push_back(bee::Point(x,y));
	}
	int borderWidth=5,borderLength=40;
	// Ground body
	{
		//画最初的背景
		b2BodyDef bd;
		b2Body* ground = m_world->CreateBody(&bd);
		b2PolygonShape shape;
		shape.SetAsBox(borderWidth,borderLength,b2Vec2(-(this->canvasWidth+borderWidth)/2,this->canvasHeight/2),0);
		ground->CreateFixture(&shape, 0.0f);
		shape.SetAsBox(borderWidth,borderLength,b2Vec2((this->canvasWidth+borderWidth)/2,this->canvasHeight/2),0);
		ground->CreateFixture(&shape, 0.0f);
		shape.SetAsBox(borderLength,borderWidth,b2Vec2(0,0-borderWidth/2),0);
		ground->CreateFixture(&shape, 0.0f);
		shape.SetAsBox(borderLength,borderWidth,b2Vec2(0,this->canvasHeight+borderWidth/2),0);
		ground->CreateFixture(&shape, 0.0f);

		for (b2Fixture* f = ground->GetFixtureList(); f; f = f->GetNext())
		{
			b2Filter b=f->GetFilterData();
			f->SetUserData(new FixtureData(bound));
			b.categoryBits=bound;
			b.maskBits=inner;
			f->SetFilterData(b);
		}
	}
	m_world->SetGravity(b2Vec2(0,-5));
	m_bodyIndex = 0;
	memset(m_bodies, 0, sizeof(m_bodies));
	//m_world->SetContactListener(new PicsCollisionCallback());
	m_world->SetContactFilter(new PicsContactFilter());

}
void PolyShapes::textureInit(bee::Picture *image_file)
{    
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1,&image_file->texId);

	glBindTexture(GL_TEXTURE_2D,image_file->texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_file->width,
		image_file->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		image_file->imgData);

	glEnable(GL_TEXTURE_2D);
}



extern b2Vec2 ConvertWorldToScreen(float32 x,float32 y);
extern b2Vec2 ConvertScreenToWorld(int x,int y);
void DrawBodyId(b2World*world )
{
	for (b2Body* body=world->GetBodyList();body;body=body->GetNext())
	{
		if (body->GetType()==b2_staticBody)
		{
			return;
		}
		int body_id=((BodyData*)body->GetUserData())->m_Id;
		std::string str_id=Num2Str(body_id).c_str();
		const char *char_id=str_id.c_str();
		b2Fixture*fixture=body->GetFixtureList()->GetNext();
		b2AABB ab=fixture->GetAABB();

		b2Vec2 screenPosition=ConvertWorldToScreen((ab.lowerBound.x+ab.upperBound.x)/2,(ab.lowerBound.y+ab.upperBound.y)/2);
		DrawString(screenPosition.x,screenPosition.y,char_id);
	}
}

int testLayoutDoneStep=500;
void PolyShapes::Step(Settings* settings)
{
	if(this->body_num<this->inputFiles.size()){
		this->Create(1);
	}
	--testLayoutDoneStep;
	if(testLayoutDoneStep<0){
		LayoutRender layout=LayoutRender(this->m_world);
		layout.CalcEveryBodyShowArea();
	}
	//首先调用在每一步中，每个test都要做的事情
	Test::Step(settings);
	texCount=0;
	int texImgCount=0;
	DrawBodyId(m_world);
	//画xy轴坐标刻度
	DrawCoordinary();
	//去掉不相接触的fixture之间的user_data中的contact关系	
	HandleContactFixture(m_world);
	//规定角度
	AdjustAngle(m_world);

#ifdef USE_TEXTURE
	for (b2Body *body=m_world->GetBodyList();body;body=body->GetNext())
	{

		bee::Picture *i_f=((bee::Picture*)(body->GetUserData()));

		b2Vec2 vertices[b2_maxPolygonVertices];
		for (b2Fixture *fixture=body->GetFixtureList();fixture;fixture=fixture->GetNext())
		{
			if (fixture->GetFilterData().categoryBits==outer)
			{//以下就是找出当前body的纹理坐标
				const b2Transform& xf = fixture->GetBody()->GetTransform();
				b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
				int32 vertexCount = poly->m_vertexCount;
				b2Assert(vertexCount <= b2_maxPolygonVertices);

				for (int32 i = 0; i < vertexCount; ++i)
				{
					vertices[i] = b2Mul(xf, poly->m_vertices[i]);
				}
				display(i_f,vertices);
				break;
			} 
		}

	}

#endif
}

void PolyShapes::Create(int32 index)
{
	++this->body_num;
	//当前bodyIndex上是否已经有了body，如果有的话，则消除已有body，空出位置
	//用于body个数超出最大限制时
	if (m_bodies[m_bodyIndex] != NULL)
	{
		m_world->DestroyBody(m_bodies[m_bodyIndex]);
		m_bodies[m_bodyIndex] = NULL;
	}
	bee::Picture &pictureInfo=this->inputFiles[m_bodyIndex];
	////////创建和初始化Body//////////////////////////////////////////////
	b2BodyDef bd;
	bd.type = b2_dynamicBody;

	bee::Point initialPosition=this->initialPosition[this->body_num-1];
	bd.position.Set(initialPosition.x,initialPosition.y);

	bd.angle = RandomFloat(-50.0f,50.0f)/180*b2_pi;

	m_bodies[m_bodyIndex] = m_world->CreateBody(&bd);
	/////////组成Body的图形////////////////////////////////////////////////
	b2FixtureDef outer_bound;
	outer_bound.restitution=0.1f;
	outer_bound.userData=new FixtureData(outer);
	b2Vec2 vertices[4];
	float32 scalar=pictureInfo.height/picApproxHeightInBox2d;
	vertices[0].Set(0,0);
	vertices[1].Set(pictureInfo.width/scalar,0);
	vertices[2].Set(pictureInfo.width/scalar,pictureInfo.height/scalar);
	vertices[3].Set(0,pictureInfo.height/scalar);
	b2PolygonShape m_polygons2;
	m_polygons2.Set(vertices,4);
	outer_bound.shape=&m_polygons2;
	outer_bound.density=1.0f;
	outer_bound.friction=0.0f;
	///////设置图形之间的可碰撞关系/////////////////////////////////////////
	outer_bound.filter.categoryBits=outer;
	outer_bound.filter.maskBits=inner;

	b2FixtureDef salent_bound;

	salent_bound.userData=new FixtureData(inner);
	b2Vec2 salient_vertices[8];
	for (unsigned int i=0;i<pictureInfo.salientArea.size();i++)
	{
		////因为给的文件是以右下角为原点的，所以需要转换点坐标
		//salient_vertices[i].Set((pictureInfo.salientArea[i].first)/SCALAR,
		//	(pictureInfo.height-pictureInfo.salientArea[i].second)/SCALAR);

		salient_vertices[i].Set((pictureInfo.salientArea[i].first)/scalar,
			(pictureInfo.height-pictureInfo.salientArea[i].second)/scalar);
	}
	b2PolygonShape m_polygons3;
	m_polygons3.Set(salient_vertices,pictureInfo.salientArea.size());
	salent_bound.shape=&m_polygons3;
	salent_bound.density = 1.0f;
	salent_bound.friction = 0.0f;
	///////设置图形之间的可碰撞关系/////////////////////////////////////////
	salent_bound.filter.categoryBits=inner;
	salent_bound.filter.maskBits=outer|bound;

	b2Fixture* outerFixture=m_bodies[m_bodyIndex]->CreateFixture(&outer_bound);//把图形拼到Body上去
	b2Fixture* salientFixture=m_bodies[m_bodyIndex]->CreateFixture(&salent_bound);
	((FixtureData*)outerFixture->GetUserData())->SetFixture(outerFixture);
	((FixtureData*)salientFixture->GetUserData())->SetFixture(salientFixture);
	BodyData *bodyData=new BodyData(this->body_num);
	bodyData->m_Image=&pictureInfo;
	m_bodies[m_bodyIndex]->SetUserData(bodyData);
#ifdef USE_TEXTURE
	textureInit(&pictureInfo);
#endif

	m_bodyIndex = (m_bodyIndex + 1) % k_maxBodies;
}

float32 PolyShapes::TestLayoutStability()
{
	return 0;
}

bool PolyShapes::IsLayoutDone()
{
	return false;
}
