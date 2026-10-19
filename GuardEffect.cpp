#include "GuardEffect.h"
#include <numbers>

using namespace KamataEngine;

Model* GuardEffect::model_ = nullptr;
Camera* GuardEffect::camera_ = nullptr;

GuardEffect* GuardEffect::Create(KamataEngine::Vector3& position) {
	GuardEffect* instanse = new GuardEffect();

#ifdef _DEBUG
	assert(instanse);
#endif // _DEBUG

	instanse->Initialize(position);

	return instanse;
}

void GuardEffect::BehaviorRootUpdate() {
	wtu_.WorldTransformUpdate(cirleWorldTransform_);

	for (WorldTransform& worldTransform : effipseWorldTransforms_) {
		wtu_.WorldTransformUpdate(worldTransform);
	}

	deathCounter_++;
	if (deathCounter_ >= 30) {
		behaviorRequest_ = Behavior::kDeath;
	}
}

void GuardEffect::BehaviorDeathInitialize() { isDead_ = true; }

void GuardEffect::BehaviorDeathUpdate() {}

void GuardEffect::Initialize(KamataEngine::Vector3& position) {
	randomaize_.Initialize();

	wtu_.WorldTransformInitialize(cirleWorldTransform_);
	cirleWorldTransform_.Initialize();
	cirleWorldTransform_.translation_ = position;

	for (WorldTransform& worldTransform : effipseWorldTransforms_) {
		wtu_.WorldTransformInitialize(worldTransform);

		worldTransform.translation_ = position;

		worldTransform.Initialize();
	}
	// deathCounter_ = 0;
}

void GuardEffect::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;

		switch (behavior_) {

		case GuardEffect::Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		default:
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case GuardEffect::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case GuardEffect::Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}
}

void GuardEffect::Draw() {

	if (!isDead_) {
		model_->Draw(cirleWorldTransform_, *camera_);

		for (const WorldTransform& worldTransform : effipseWorldTransforms_) {
			model_->Draw(worldTransform, *camera_);
		}
	}
}
