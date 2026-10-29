#pragma once
#include "KamataEngine.h"
#include "Object.h"
#include "WTU.h"

using namespace KamataEngine;

class Key {
public:

// 当たり判定
	AABB GetAABB();

	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

	bool IsGot() const { return isGot_; }
	void SetIsGot(bool isGot) { isGot_ = isGot; }

private:

	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	WTU wtu_;

	bool isGot_ = false;

	// 当たり判定のサイズ
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;
};
