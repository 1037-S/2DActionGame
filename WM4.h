#pragma once
#include "KamataEngine.h"
#include "M4.h"
#include "MM.h"
#include "Rtt.h"

class WM4 {
public:
	~WM4();
	// アフィン変換行列
	KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);

private:
	M4 m4_;
	MM mm_;
	Rtt rtt_;

	KamataEngine::Matrix4x4 scaleMatrix_;
	KamataEngine::Matrix4x4 rotateMatrix_;
	KamataEngine::Matrix4x4 translateMatrix_;
};
