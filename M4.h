#pragma once
#include "KamataEngine.h"

class M4 {
public:
	// 1.行列の加法
	KamataEngine::Matrix4x4 Add(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);
	// 2.行列の減法
	KamataEngine::Matrix4x4 Subtract(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);
	// 3.行列の積
	KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);
	// 4.逆行列
	KamataEngine::Matrix4x4 Inverse(const KamataEngine::Matrix4x4& m);
	// 5.転置行列
	KamataEngine::Matrix4x4 Transpose(const KamataEngine::Matrix4x4& m);
	// 6.単位行列の作成
	KamataEngine::Matrix4x4 MakeIdentity4x4();

private:
};
