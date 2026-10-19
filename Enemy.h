#pragma once
#include "AABBCo.h"
#include "Easing.h"
#include "KamataEngine.h"
#include "Object.h"
#include "Rtt.h"
#include "WTU.h"
#include <algorithm>
#include <numbers>

// 前方宣言
class MapChipField;
class Player;
class GameScene;
class PlayerBullet;

class Enemy {
public:
	// 左右
	enum class LRDirection {
		kRight, // 右向き
		kLeft,  // 左向き

	};

	enum class Behavior {
		kUnknown, // リクエストなし
		kRoot,    // 通常状態
		kDeath,   // デス演出
	};

	~Enemy();

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition() const;

	// AABBを取得する
	AABB GetAABB();

	// プレイヤーとの衝突応答
	void OnCollision(const Player* player);

	// 弾との衝突応答
	void OnCollision(class PlayerBullet* bullet);

	// 通常行動更新
	void BehaviorRootUpdate();

	void BehaviorDeathInitialize();
	void BehaviorDeathUpdate();

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	bool GetIsDead() const { return isDead_; }

	bool IsCollisionDidabled() const { return isCollisionDidabled_; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

private:
	AABBCo aabb_;
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;

	Behavior behavior_ = Behavior::kRoot;

	Behavior behaviorRequest_ = Behavior::kUnknown;

	Easing ease_;
	uint32_t deathAnimationTimer_ = 0;
	static inline const uint32_t kDeathAnimationDuration = 30;

	KamataEngine::Vector3 deathStartRotation_{};
	KamataEngine::Vector3 deathStartScale_{};
	// ワールド変換
	WTU wtu_;
	Rtt rtt_;
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// モデル
	KamataEngine::Model* enemyModel_ = nullptr;
	// カメラ
	KamataEngine::Camera* enemyCamera_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 歩く速さ
	static inline const float kWalkSpeed_ = 0.05f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// アニメーション
	// 初期の角度[度]
	static inline const float kWalkMotionAngleStart_ = 0.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd_ = 90.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime_ = 0.5f;
	// アニメーションの経過時間
	float walkTimer_;

	// デスフラグ
	bool isDead_ = false;

	bool isCollisionDidabled_ = false;

	// キャラクターの当たり判定のサイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.01f;
};
