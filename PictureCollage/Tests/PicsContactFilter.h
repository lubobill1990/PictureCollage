#ifndef PIC_SCONTACT_FILTER_H
#define PIC_SCONTACT_FILTER_H

#include <Box2D/Box2D.h>
#include "FixtureData.h"
#include "HandleLayoutResult.h"
class PicsContactFilter:public b2ContactFilter
{
private:
	//��ײ�Ƿ�������Ҫ����ͻ����ı߽�֮��
	bool IsCollisionOccurBetweenBoundAndSalient(b2Fixture *fixtureA,b2Fixture *fixtureB);

	//�����Լ������FixtureData�е���Ϣ�����ж��Ƿ�Ӧ����ײ��
	bool ShouldCollideAccordingToFixtureData(b2Fixture *fixtureA,b2Fixture *fixtureB);

public:
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);

};
#endif
