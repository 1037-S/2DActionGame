#include "Enemy.h"
#include "GameScene.h"
#include "MapChipField.h"
#include "Player.h"
#include "PlayerBullet.h"
#include <cmath>

using namespace KamataEngine;

Enemy::~Enemy() {}

KamataEngine::Vector3 Enemy::GetWorldPosition() const {

	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Enemy::GetAABB() {

	Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::OnCollision(const Player* player) {
	(void)player;
	// デスフラグが発生する
	// isDead_ = true;
	if (behavior_ == Behavior::kDeath) {
		return;
	}

	// プレイヤーが攻撃中なら敵が倒れる
	if (player->IsAttack()) {
		behaviorRequest_ = Behavior::kDeath;

		// 敵とプレイヤーの中間座標にエフェクトを生成
		Vector3 myPos = GetWorldPosition();
		Vector3 playerPos = player->GetWorldPosition();

		Vector3 effectPos = {(myPos.x + playerPos.x) / 2.0f, (myPos.y + playerPos.y) / 2.0f, (myPos.z + playerPos.z) / 2.0f};
		gameScene_->CreateEffect(effectPos);
	}
}

void Enemy::OnCollision(PlayerBullet* bullet) {
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

void Enemy::BehaviorRootUpdate() {
	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// タイマーの加算
	walkTimer_ += 1.0f / 60.0f;
	// 回転アニメーション
	float param = std::sin((walkTimer_ / kWalkMotionTime_) * std::numbers::pi_v<float> * 2.0f);
	float degree = kWalkMotionAngleStart_ + kWalkMotionAngleEnd_ * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = degree * (std::numbers::pi_v<float> / 180.0f);
	wtu_.WorldTransformUpdate(worldTransform_);
}

void Enemy::BehaviorDeathInitialize() {
	deathAnimationTimer_ = 0;
	// 死亡開始時点の状態を保持
	deathStartRotation_ = worldTransform_.rotation_;
	deathStartScale_ = worldTransform_.scale_;
}

void Enemy::BehaviorDeathUpdate() {
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

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
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
	velocity_ = {-kWalkSpeed_, 0.0f, 0.0f};

	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	// BehaviorRootUpdate();
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		default:
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Enemy::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Enemy::Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}
}

void Enemy::Draw() {
	// 3Dモデルを描画する
	if (!isDead_) {
		enemyModel_->Draw(worldTransform_, *enemyCamera_);
	}
}
