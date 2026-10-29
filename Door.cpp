#include "Door.h"

AABB Door::GetAABB() {
	Vector3 worldPos = worldTransform_.translation_;
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
	;
}

void Door::Initialize(Model* model, const Vector3& position, uint8_t doorID, uint8_t pairID) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	model_ = model;
	doorID_ = doorID;
	pairID_ = pairID;
	isEntered_ = false;
}

void Door::Update(const Vector3& playerPos) {
	(void)playerPos;
	wtu_.WorldTransformUpdate(worldTransform_);

}

void Door::Draw(const Camera& camera) {
	if (model_) {
		model_->Draw(worldTransform_, camera);
	}
}