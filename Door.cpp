#include "Door.h"

using namespace KamataEngine;

void Door::Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, uint8_t targetStageIndex) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	model_ = model;
	targetStageIndex_ = targetStageIndex;
	isEntered_ = false;
}

void Door::Update(const Vector3& playerPos) {
	wtu_.WorldTransformUpdate(worldTransform_);

	// プレイヤーとの距離計算 (球判定)
	Vector3 diff = {worldTransform_.translation_.x - playerPos.x, worldTransform_.translation_.y - playerPos.y, worldTransform_.translation_.z - playerPos.z};
	float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

	// 距離内で上キー(Wキー)を押したらドアに入る
	if (distanceSq <= interactRadius_ * interactRadius_) {
		Input* input = Input::GetInstance();
		if (input->TriggerKey(DIK_UP)) {
			isEntered_ = true;
		}
	}
}

void Door::Draw(const Camera& camera) {
	if (model_) {
		model_->Draw(worldTransform_, camera);
	}
}