#pragma once
#include "KamataEngine.h"
#include "WTU.h"
#include "Object.h"

using namespace KamataEngine;

class Ladder {
public:

	// 当たり判定
	AABB GetAABB();

	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

	// 吸着処理
	float GetPosX() const { return worldTransform_.translation_.x; }

private:

	// 数学関数
	WTU wtu_;

	// ワールド座標系
	WorldTransform worldTransform_;

	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	//当たり判定のサイズ
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;



};
