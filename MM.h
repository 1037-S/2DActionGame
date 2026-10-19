#pragma once
#include "KamataEngine.h"

class MM {
public:
	~MM();
	// 1.平行移動行列
	KamataEngine::Matrix4x4 MakeTransLateMatrix(const KamataEngine::Vector3& transLate);

	// 2.拡大縮小行列
	KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);

	// 3.座標変換
	KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);

private:
};
