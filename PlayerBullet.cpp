#include "PlayerBullet.h"

using namespace KamataEngine;

void PlayerBullet::Initialize(Model* model, Camera* camera, const Vector3& position, const Vector3& velocity) {
	model_ = model;
	camera_ = camera;
	velocity_ = velocity;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
}

void PlayerBullet::Update() {
	if (isDead_)
		return;

	// 移動処理
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// 時間経過で消滅
	if (--lifeTimer_ <= 0) {
		isDead_ = true;
	}

	wtu.WorldTransformUpdate(worldTransform_);
}

void PlayerBullet::Draw() {
	if (isDead_)
		return;
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

AABB PlayerBullet::GetAABB() const {
	Vector3 pos = worldTransform_.translation_;

	float halfWidth = kWidth * 0.5f; // 例: kWidth=0.3f なら halfWidth=0.15f
	float halfHeight = kHeight * 0.5f;

	AABB aabb;
	aabb.min = {pos.x - halfWidth, pos.y - halfHeight, pos.z - halfWidth};
	aabb.max = {pos.x + halfWidth, pos.y + halfHeight, pos.z + halfWidth};
	return aabb;
}