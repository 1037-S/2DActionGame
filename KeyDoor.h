#pragma once
#include "KamataEngine.h"
#include "Object.h"
#include "WTU.h"

using namespace KamataEngine;

class KeyDoor {
public:
	// 当たり判定
	AABB GetAABB();

	void Initialize(Model* model, Camera* camera, const Vector3& position ,uint8_t doorID, uint8_t pairID);
	void Update();
	void Draw();

	// 開錠状態の確認・設定
	bool IsOpened() const { return isOpened_; }
	void Open() { isOpened_ = true; }

	uint8_t GetDoorID() const { return doorID_; }
	uint8_t GetPairID() const { return pairID_; }
	const Vector3& GetPosition() const { return worldTransform_.translation_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	WTU wtu_;

	bool isOpened_ = false; // ドアが開いたかどうか

	uint8_t doorID_ = 0; // 自分のドアID (例: 0)
	uint8_t pairID_ = 0; // ペアとなる移動先のドアID (例: 1)

	// 当たり判定のサイズ
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;
};
