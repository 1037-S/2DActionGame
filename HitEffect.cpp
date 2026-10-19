#include "HitEffect.h"
#include <numbers>

using namespace KamataEngine;

Model* HitEffect::model_ = nullptr;
Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(KamataEngine::Vector3& position) {
	HitEffect* instanse = new HitEffect();

#ifdef _DEBUG
	assert(instanse);
#endif // _DEBUG

	instanse->Initialize(position);

	return instanse;
}

void HitEffect::BehaviorRootUpdate() {
	wtu_.WorldTransformUpdate(cirleWorldTransform_);

	for (WorldTransform& worldTransform : effipseWorldTransforms_) {
		wtu_.WorldTransformUpdate(worldTransform);
	}

	deathCounter_++;
	if (deathCounter_ >= 30) {
		behaviorRequest_ = Behavior::kDeath;
	}
}

void HitEffect::BehaviorDeathInitialize() { isDead_ = true; }

void HitEffect::BehaviorDeathUpdate() {}

void HitEffect::Initialize(KamataEngine::Vector3& position) {
	randomaize_.Initialize();

	wtu_.WorldTransformInitialize(cirleWorldTransform_);
	cirleWorldTransform_.translation_ = position;

	for (WorldTransform& worldTransform : effipseWorldTransforms_) {
		wtu_.WorldTransformInitialize(worldTransform);

		worldTransform.translation_ = position;

		float scaleX = randomaize_.GetRandomFloat(0.1f, 0.3f);
		float scaleY = randomaize_.GetRandomFloat(1.2f, 1.8f);

		worldTransform.scale_ = {scaleX, scaleY, 1.0f};

		float rotationZ = randomaize_.GetRandomFloat(0.0f, std::numbers::pi_v<float> * 2.0f);

		worldTransform.rotation_ = {0.0f, 0.0f, rotationZ};

		worldTransform.Initialize();
	}
}

void HitEffect::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;

		switch (behavior_) {

		case HitEffect::Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		default:
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case HitEffect::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case HitEffect::Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}
}

void HitEffect::Draw() {

	if (!isDead_) {
		model_->Draw(cirleWorldTransform_, *camera_);

		for (const WorldTransform& worldTransform : effipseWorldTransforms_) {
			model_->Draw(worldTransform, *camera_);
		}
	}
}
