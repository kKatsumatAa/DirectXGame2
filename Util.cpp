#include "Util.h"

float GetLength3(XMFLOAT3& end, XMFLOAT3& start)
{
	return sqrtf(powf(end.x - start.x, 2) + powf(end.y - start.y, 2) + powf(end.z - start.z, 2));
}

bool CircleCollision(WorldTransform& c1, WorldTransform& c2) //仮（scaleが全て一緒の時使える）
{ 
	if ((GetLength3(c1.translation_, c2.translation_)) <= (c1.scale_.x * 2.0f + c2.scale_.x * 2.0f))
		return true;
	else
		return false;
}
