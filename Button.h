#pragma once
#include "KamataEngine.h"
#include "Object.h"
#include "WTU.h"

using namespace KamataEngine;

class Button {
public:
	// 当たり判定
	AABB GetAABB();

	void Initialize(Model* model, Camera* camera, const Vector3& position, uint8_t buttonID);
	void Update();
	void Draw();

	// 押されたフラグのゲッター・セッター
	bool IsPressed() const { return isPressed_; }
	void Press() { isPressed_ = true; }

	uint8_t GetButtonID() const { return buttonID_; }
	const Vector3& GetPosition() const { return worldTransform_.translation_; }

	// サイズ定数のゲッター
	static float GetWidth() { return kWidth; }
	static float GetHeight() { return kHeight; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	WTU wtu_;

	bool isPressed_ = false; // ボタンが押されたか
	uint8_t buttonID_ = 0;   // ボタンのID

	// 当たり判定のサイズ
	static inline const float kWidth = 1.1f;
	static inline const float kHeight = 1.1f;
};
