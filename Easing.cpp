#include "Easing.h"

float Easing::EaceIn(float x, float y, float t) {
	t = std::clamp(t, 0.0f, 1.0f);

	// EaseIn の計算：t の3乗
	float easedT = t * t * t;

	// 線形補間 (Lerp)
	return x + (y - x) * easedT;
}

float Easing::EaceOut(float x, float y, float t) {
	t = std::clamp(t, 0.0f, 1.0f);

	float easedT = 1.0f - std::pow(1.0f - t, 3.0f);

	return x + (y - x) * easedT;
}
