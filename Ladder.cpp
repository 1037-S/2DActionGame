#include "Ladder.h"

AABB Ladder::GetAABB() { 
	Vector3 worldPos = worldTransform_.translation_;
	AABB aabb;
	
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb; }

void Ladder::Initialize(Model* model, Camera* camera, const Vector3& position) { 
	model_ = model;
	camera_ = camera;

	// ワールドTransformの初期化処理
	wtu_.WorldTransformInitialize(worldTransform_);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Ladder::Update() {
	wtu_.WorldTransformUpdate(worldTransform_);

}

void Ladder::Draw() { model_->Draw(worldTransform_, *camera_); }
