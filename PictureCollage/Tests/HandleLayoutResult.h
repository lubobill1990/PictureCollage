#include "Common.h"
#include <Polygon/Polygon.h>

VecVector ReorderVertices(VecVector vertices);

float GetPolygonFixtureOverlapArea(b2Fixture* fixtureA,b2Fixture* fixtureB);

void HandleContactFixture(b2World *world);
void AdjustAngle(b2World *world);