#pragma once
#include "AABBCo.h"
#include "KamataEngine.h"
#include "M4.h"
#include "MM.h"
#include "Rtt.h"
#include "WTU.h"

class PlayerBullet {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);
	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }
	void OnCollision() { isDead_ = true; }

	AABB GetAABB() const;

private:
	WTU wtu;
	MM mm;
	M4 m4;
	Rtt rtt;

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Vector3 velocity_{};

	int32_t lifeTimer_ = 120; // 弾の生存時間 (フレーム)
	bool isDead_ = false;

	static inline const float kWidth = 0.5f;
	static inline const float kHeight = 0.5f;
};
