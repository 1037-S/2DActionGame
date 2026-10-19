#pragma once
#include "KamataEngine.h"
#include "Object.h"
#include "Randomize.h"
#include "WTU.h"
#include <cmath>

class HitEffect {
public:
	enum class Behavior {
		kUnknown, // リクエストなし
		kRoot,    // 通常状態
		kDeath,   // デス演出
	};

	static HitEffect* Create(KamataEngine::Vector3& position);

	void BehaviorRootUpdate();

	void BehaviorDeathInitialize();

	void BehaviorDeathUpdate();

	void Initialize(KamataEngine::Vector3& position);
	void Update();
	void Draw();

	bool IsDead() { return isDead_; }

	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

private:
	WTU wtu_;

	Behavior behavior_ = Behavior::kRoot;

	Behavior behaviorRequest_ = Behavior::kUnknown;

	uint32_t deathCounter_;

	// デスフラグ

	bool isDead_ = false;

	static KamataEngine::Model* model_;

	static KamataEngine::Camera* camera_;

	KamataEngine::WorldTransform cirleWorldTransform_;

	std::array<KamataEngine::WorldTransform, 2> effipseWorldTransforms_;

	Randomize randomaize_;
};
