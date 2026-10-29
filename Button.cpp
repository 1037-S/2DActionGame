#include "Button.h"

AABB Button::GetAABB() {
	Vector3 worldPos = worldTransform_.translation_;
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Button::Initialize(Model* model, Camera* camera, const Vector3& position, uint8_t buttonID) {
	model_ = model;
	camera_ = camera;
	buttonID_ = buttonID;
	isPressed_ = false;

	// ワールドTransformの初期化処理
	wtu_.WorldTransformInitialize(worldTransform_);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Button::Update() { wtu_.WorldTransformUpdate(worldTransform_); }

void Button::Draw() {
	if ( model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}
