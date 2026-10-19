#pragma once
#include "KamataEngine.h"
#include "MM.h"
#include "Object.h"
#include "Rtt.h"
#include "WM4.h"
#include "WTU.h"
#include <algorithm>
#include <array>
#include <cmath>

class Player;
// デス演出用のパーティクル

class DeathParticles {
public:
	//~DeathParticles();

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	// デスフラグのヘッダー
	bool IsFinshed() const { return isFinished_; }

private:
	// ワールド座標
	// ワールド変換
	WM4 wm4_;
	MM mm_;
	WTU wtu_;
	Rtt rtt_;

	// モデル
	KamataEngine::Model* deathParticlesModel_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;
	// パーティクルの存続時間(消えるまでの時間)<秒>
	static inline const float kDuration = 3.0f;
	// 移動の速さ
	static inline const float kspeed = 0.05f;
	// 分割した1個分の角度
	static inline const float kAngleUnit = (3.14f * 2.0f) / 8.0f;
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;
	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;

	// 色変更オブジェクト
	KamataEngine::ObjectColor objectColor_;
	// 色の数値
	KamataEngine::Vector4 color_;
};
