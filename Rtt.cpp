#include "Rtt.h"
using namespace KamataEngine;

Rtt::~Rtt() {}
// 1.X軸回転行列
KamataEngine::Matrix4x4 Rtt::MakeRotateXMatrix(float radian) {
	Matrix4x4 result{};

	result.m[0][0] = 1.0f;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
}
// 2.Y軸回転行列
KamataEngine::Matrix4x4 Rtt::MakeRotateYMatrix(float radian) {
	Matrix4x4 result{};

	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);
	result.m[1][1] = 1.0f;
	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[3][3] = 1.0f;

	return result;
}
// 3.Z軸回転行列
KamataEngine::Matrix4x4 Rtt::MakeRotateZMatrix(float radian) {
	Matrix4x4 result{};

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	return result;
}

void Rtt::Update() {
	rotateMatrixX_ = MakeRotateXMatrix(rotate_.x);
	rotateMatrixY_ = MakeRotateYMatrix(rotate_.y);
	rotateMatrixZ_ = MakeRotateZMatrix(rotate_.z);

	rotateXYZMatrix_ = m4_.Multiply(rotateMatrixX_, m4_.Multiply(rotateMatrixY_, rotateMatrixZ_));
}
