#define NOMINMAX

#include "CameraController.h"
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

CamaraController::~CamaraController() {}

void CamaraController::Initialize(Camera* camera) {
	camera_ = camera;
	camera_->Initialize();
}

void CamaraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットからカメラの座標を計算する
	// camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	// camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	// camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	cameraTarget_.x = targetWorldTransform.translation_.x + targetOffset_.x + target_->GetVelocity().x * kVelocityBias;
	cameraTarget_.y = targetWorldTransform.translation_.y + targetOffset_.y + target_->GetVelocity().y * kVelocityBias;
	cameraTarget_.z = targetWorldTransform.translation_.z + targetOffset_.z + target_->GetVelocity().z * kVelocityBias;

	// 座標補間によりゆったり追従
	camera_->translation_.x = wtu_.Lerp(camera_->translation_.x, cameraTarget_.x, kInterpolationRate_);
	camera_->translation_.y = wtu_.Lerp(camera_->translation_.y, cameraTarget_.y, kInterpolationRate_);
	camera_->translation_.z = wtu_.Lerp(camera_->translation_.z, cameraTarget_.z, kInterpolationRate_);

	// 追従対象が画面外に出ないように補正する
	camera_->translation_.x = std::clamp(camera_->translation_.x, targetWorldTransform.translation_.x + margin_.left, targetWorldTransform.translation_.x + margin_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, targetWorldTransform.translation_.y + margin_.bottom, targetWorldTransform.translation_.y + margin_.top);

	// 移動範囲の制限
	camera_->translation_.x = std::clamp(camera_->translation_.x, movebleArea_.left, movebleArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movebleArea_.bottom, movebleArea_.top);

	// 行列の更新
	camera_->UpdateMatrix();
}

void CamaraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットからカメラの座標を計算する
	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}
