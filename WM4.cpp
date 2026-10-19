#include "WM4.h"
using namespace KamataEngine;

WM4::~WM4() {}
// アフィン変換行列
KamataEngine::Matrix4x4 WM4::MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate) {
	Matrix4x4 result = {};

	scaleMatrix_ = mm_.MakeScaleMatrix(scale);
	rtt_.SetRotate(rotate);
	rtt_.Update();
	rotateMatrix_ = rtt_.GetRotateXYZMatrix();
	translateMatrix_ = mm_.MakeTransLateMatrix(translate);

	result = m4_.Multiply(scaleMatrix_, m4_.Multiply(rotateMatrix_, translateMatrix_));

	return result;
}
