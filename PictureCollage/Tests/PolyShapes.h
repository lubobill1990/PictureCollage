/*
* Copyright (c) 2006-2009 Erin Catto http://www.gphysics.com
*
	* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
	* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef POLY_SHAPES_H
#define POLY_SHAPES_H
#include <Box2D/Collision/b2Collision.h>

#include "../Framework/Test.h"
#include "../Framework/Render.h"
#include "PicsContactFilter.h"
#include "FixtureData.h"
#include "HandleLayoutResult.h"

#include <utility>
#include <vector>
using namespace std;

class PolyShapes : public Test
{
public:
	void DestroyBody()
	{
		for (int32 i = 0; i < k_maxBodies; ++i)
		{
			if (m_bodies[i] != NULL)
			{
				m_world->DestroyBody(m_bodies[i]);
				m_bodies[i] = NULL;
				return;
			}
		}
	}

	//�����¼������ķ�Ӧ
	void Keyboard(unsigned char key)
	{
		switch (key)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			Create(key - '1');
			break;
		case '0':
			this->body_num=0;
			break;
		case 'f':
			layoutfinished=true;
			break;
		case 'r':
			layoutfinished=false;
			break;
		case 'a':
			for (int32 i = 0; i < k_maxBodies; i += 2)
			{
				if (m_bodies[i])
				{
					bool active = m_bodies[i]->IsActive();
					m_bodies[i]->SetActive(!active);
				}
			}
			break;

		case 'd':
			DestroyBody();
			break;
		}
	}
	

	//�൱��Test��������
	static Test* Create()
	{
		return new PolyShapes;
	}
	PolyShapes();

	//���ݼ������봴��Body
	void Create(int32 index);
	void Step(Settings* settings);
	int32 m_bodyIndex;//��ǰbody���Է���m_bodies����һ��������
	b2Body* m_bodies[k_maxBodies];//�洢���е�body
	b2PolygonShape m_polygons[4];
	b2CircleShape m_circle;
	float32 canvasWidth,canvasHeight;
	uint pictureCount;
	vector<bee::Picture> inputFiles;
	vector<bee::Point> initialPosition;
	bool layoutfinished;
private:
	float32 TestLayoutStability();
	bool IsLayoutDone();
	int body_num;
};

#endif