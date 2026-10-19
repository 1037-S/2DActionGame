#include "WTU.h"

using namespace KamataEngine;

WTU::~WTU() {
	// delete rtt_;
	// delete mm_;
	// delete wm4_;
	// delete m4_;

	// rtt_ = nullptr;
	// mm_ = nullptr;
	// wm4_ = nullptr;
	// m4_ = nullptr;
}

void WTU::WorldTransformInitialize(KamataEngine::WorldTransform& worldTransform) {
	worldTransform.Initialize();
	// rtt_ = new Rtt();
	// mm_ = new MM();
	// wm4_ = new WM4();
	// m4_ = new M4();
}

void WTU::WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform) {
	// if (!rtt_ || !mm_ || !m4_) {
	//	return;
	// }
	//  スケール、回転、平行移動を合成して行列を計算
	rtt_.SetRotate(worldTransform.rotation_);
	rtt_.Update();
	Matrix4x4 scale = mm_.MakeScaleMatrix(worldTransform.scale_);
	Matrix4x4 rotation = rtt_.GetRotateXYZMatrix();
	Matrix4x4 translation = mm_.MakeTransLateMatrix(worldTransform.translation_);

	worldTransform.matWorld_ = m4_.Multiply(scale, m4_.Multiply(rotation, translation));
	// 行列を定数バッファに転送
	worldTransform.TransferMatrix();
}

float WTU::Lerp(float start, float end, float tMax, float t) {
	float newt = t / tMax;
	newt = 1.0f - newt;
	float easedt = newt * newt;

	return (1.0f - easedt) * start + easedt * end;
}

float WTU::Lerp(float start, float end, float t) { return (1.0f - t) * start + t * end; }
