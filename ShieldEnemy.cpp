#include "ShieldEnemy.h"
#include "GameScene.h"
#include "MapChipField.h"
#include "Player.h"
#include "PlayerBullet.h"
#include <cmath>

using namespace KamataEngine;

ShieldEnemy::~ShieldEnemy() {}

KamataEngine::Vector3 ShieldEnemy::GetWorldPosition() const {

	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB ShieldEnemy::GetAABB() {

	Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void ShieldEnemy::OnCollision(const Player* player) {
	(void)player;
	// デスフラグが発生する
	// isDead_ = true;
	if (behavior_ == Behavior::kDeath) {
		return;
	}

	// プレイヤーが攻撃中なら敵が倒れる
	if (player->IsAttack()) {
		// 敵とプレイヤーの中間座標にエフェクトを生成
		Vector3 myPos = GetWorldPosition();
		Vector3 playerPos = player->GetWorldPosition();

		// 前方の場合ガード成功
		if (player->GetLRDirection() == LRDirection::kRight && lrDirection_ == LRDirection::kLeft || player->GetLRDirection() == LRDirection::kLeft && lrDirection_ == LRDirection::kRight) {
			Vector3 effectPos = {(myPos.x + playerPos.x) / 2.0f, (myPos.y + playerPos.y) / 2.0f, (myPos.z + playerPos.z) / 2.0f};

			// GameScene 経由でガードエフェクトを生成
			if (gameScene_) {
				gameScene_->CreateGuardEffect(effectPos);
			}

			const_cast<Player*>(player)->KnockBackRequest();

			behaviorRequest_ = Behavior::kGuard;

			// 早期returnで敵のデスが回避
			return;
		}

		behaviorRequest_ = Behavior::kDeath;

		Vector3 effectPos = {(myPos.x + playerPos.x) / 2.0f, (myPos.y + playerPos.y) / 2.0f, (myPos.z + playerPos.z) / 2.0f};
		gameScene_->CreateEffect(effectPos);
	}
}

void ShieldEnemy::OnCollision(PlayerBullet* bullet) {
	(void)bullet;

	// すでに死亡中・デス演出中なら何もしない
	if (behavior_ == Behavior::kDeath) {
		return;
	}

	// 敵を死亡演出状態へリクエスト
	behaviorRequest_ = Behavior::kDeath;

	// 敵の位置にヒットエフェクトを生成
	Vector3 myPos = GetWorldPosition();
	gameScene_->CreateEffect(myPos);
}

void ShieldEnemy::BehaviorRootUpdate() {
	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// タイマーの加算
	walkTimer_ += 1.0f / 60.0f;
	// 回転アニメーション
	float param = std::sin((walkTimer_ / kWalkMotionTime_) * std::numbers::pi_v<float> * 2.0f);
	float degree = kWalkMotionAngleStart_ + kWalkMotionAngleEnd_ * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.y = degree * (std::numbers::pi_v<float> / 180.0f);
	wtu_.WorldTransformUpdate(worldTransform_);
}

void ShieldEnemy::BehaviorDeathInitialize() {
	deathAnimationTimer_ = 0;
	// 死亡開始時点の状態を保持
	deathStartRotation_ = worldTransform_.rotation_;
	deathStartScale_ = worldTransform_.scale_;
}

void ShieldEnemy::BehaviorDeathUpdate() {
	deathAnimationTimer_++;

	isCollisionDidabled_ = true;

	float t = static_cast<float>(deathAnimationTimer_) / static_cast<float>(kDeathAnimationDuration);
	t = std::clamp(t, 0.0f, 1.0f);

	worldTransform_.rotation_.y = deathStartRotation_.y + ease_.EaceOut(0.0f, std::numbers::pi_v<float> * 2.0f, t);

	// ★ X軸：元の角度から 90度（π / 2）回転（例：パタリと倒れるような変化）
	worldTransform_.rotation_.x = deathStartRotation_.x + ease_.EaceOut(0.0f, std::numbers::pi_v<float> * 0.5f, t);

	wtu_.WorldTransformUpdate(worldTransform_);
	// アニメーション時間が終了したら完全に消去（isDead_ = true）
	if (deathAnimationTimer_ >= kDeathAnimationDuration) {
		isDead_ = true;
	}
}

void ShieldEnemy::BehaviorGuardInitialize() {
	guardAnimationTimer_ = 0;
	// ガード開始時の角度を保持
	guardStartRotation_ = worldTransform_.rotation_;
}

void ShieldEnemy::BehaviorGuardUpdate() {

	guardAnimationTimer_++;

	float t = static_cast<float>(guardAnimationTimer_) / static_cast<float>(kGuardAnimationDuration);
	t = std::clamp(t, 0.0f, 1.0f);

	// sin波を利用して「0 -> -kGuardAngleX -> 0」へイージング（行って戻る動き）
	float angleOffset = std::sin(t * std::numbers::pi_v<float>) * kGuardAngleX;

	// X軸（ピッチ）方向にのけぞらせる
	worldTransform_.rotation_.x = guardStartRotation_.x + angleOffset;

	wtu_.WorldTransformUpdate(worldTransform_);

	// アニメーション終了後に通常のRoot状態へ戻る
	if (guardAnimationTimer_ >= kGuardAnimationDuration) {
		worldTransform_.rotation_.x = guardStartRotation_.x; // 角度の補正
		behaviorRequest_ = Behavior::kRoot;
	}
}

void ShieldEnemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
#ifdef _DEBUG
	assert(model);
	assert(camera);
#endif // _DEBUG

	enemyModel_ = model;
	enemyCamera_ = camera;

	// ワールド変換の初期化
	wtu_.WorldTransformInitialize(worldTransform_);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;

	// 速度の設定
	// velocity_ = { -kWalkSpeed_,0.0f,0.0f };

	// if (lrDirection_ == LRDirection::kRight) {
	//	velocity_ = { kWalkSpeed_, 0.0f, 0.0f };
	//	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 右向き
	// }
	// else {
	//	velocity_ = { -kWalkSpeed_, 0.0f, 0.0f };
	//	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f; // 左向き
	// }

	walkTimer_ = 0.0f;
}

void ShieldEnemy::Update() {
	// BehaviorRootUpdate();
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		case Behavior::kGuard:
			BehaviorGuardInitialize();
		default:
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case ShieldEnemy::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case ShieldEnemy::Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	case ShieldEnemy::Behavior::kGuard:
		BehaviorGuardUpdate();
		break;
	}
}

void ShieldEnemy::Draw() {
	// 3Dモデルを描画する
	if (!isDead_) {
		enemyModel_->Draw(worldTransform_, *enemyCamera_);
	}
}
