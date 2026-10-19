#pragma once
#include "KamataEngine.h"
#include "WM4.h"

class WTU {
public:
	// WTU();
	~WTU();

	void WorldTransformInitialize(KamataEngine::WorldTransform& worldTransform);
	void WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform);
	float Lerp(float start, float end, float tMax, float t);
	float Lerp(float start, float end, float t);

private:
	// ワールド変換
	Rtt rtt_;
	MM mm_;
	WM4 wm4_;
	M4 m4_;
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
};
