#pragma once
#include <DirectXMath.h>
#include "WorldTransform.h"

using namespace DirectX;

float GetLength3(XMFLOAT3& end, XMFLOAT3& start);

bool CircleCollision(WorldTransform& circle, WorldTransform& circle2);