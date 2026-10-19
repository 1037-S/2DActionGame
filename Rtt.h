#pragma once
#include "KamataEngine.h"
#include "M4.h"

class Rtt {
public:
	~Rtt();
	// 1.X軸回転行列
	KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);

	// 2.Y軸回転行列
	KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);

	// 3.Z軸回転行列
	KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);

	void Update();

	void SetRotate(const KamataEngine::Vector3& rotate) { rotate_ = rotate; }
	const KamataEngine::Matrix4x4& GetRotateXYZMatrix() const { return rotateXYZMatrix_; }

private:
	M4 m4_;
	KamataEngine::Vector3 rotate_;

	KamataEngine::Matrix4x4 rotateMatrixX_;
	KamataEngine::Matrix4x4 rotateMatrixY_;
	KamataEngine::Matrix4x4 rotateMatrixZ_;
	KamataEngine::Matrix4x4 rotateXYZMatrix_;
};
