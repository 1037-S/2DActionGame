#pragma once
#include "KamataEngine.h"
#include "WTU.h"

// 前方宣言
class Player;

class CamaraController {
public:
	// 矩形
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};
	~CamaraController();

	void Initialize(KamataEngine::Camera* camera);
	void Update();
	void Reset();

	void SetTarget(Player* target) { target_ = target; }
	void SetMovebleArea(Rect area) { movebleArea_ = area; }

private:
	WTU wtu_;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// カメラの目標座標
	KamataEngine::Vector3 cameraTarget_;

	// カメラの移動範囲
	Rect movebleArea_ = {0, 100, 0, 100};

	// 速度掛け率
	static inline const float kVelocityBias = 1.0f;

	// プレイヤーターゲット
	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差(オフセット)
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};

	// 追従対象の各方向へのカメラの移動範囲
	static inline const Rect margin_ = {-10, 10, -10, 10};

	// 座標補間割合
	static inline const float kInterpolationRate_ = 0.1f;
};
