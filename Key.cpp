#include "Key.h"

AABB Key::GetAABB() {
	Vector3 worldPos = worldTransform_.translation_;
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Key::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	camera_ = camera;
	isGot_ = false;

	// ワールドTransformの初期化処理
	wtu_.WorldTransformInitialize(worldTransform_);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Key::Update() { wtu_.WorldTransformUpdate(worldTransform_); }

void Key::Draw() {
	if (!isGot_ && model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}
