#pragma once
#include "AABBCo.h"
#include "Easing.h"
#include "GuardEffect.h"
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

class ShieldEnemy {
public:
	enum class Behavior {
		kUnknown, // リクエストなし
		kRoot,    // 通常状態
		kDeath,   // デス演出
		kGuard,   // ガード中
	};

	~ShieldEnemy();

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

	void BehaviorGuardInitialize();
	void BehaviorGuardUpdate();

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	void SetLRDirection(LRDirection direction) { lrDirection_ = direction; }

	bool GetIsDead() const { return isDead_; }

	bool IsCollisionDidabled() const { return isCollisionDidabled_; }

	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

	LRDirection GetLRDirection() const { return lrDirection_; }

private:
	AABBCo aabb_;
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
	GuardEffect* guardEffect_ = nullptr;
	KamataEngine::Model* guardEffectModel_ = nullptr;
	std::list<GuardEffect*> guardEffects_;

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

	LRDirection lrDirection_ = LRDirection::kLeft;

	// アニメーション
	// 初期の角度[度]
	static inline const float kWalkMotionAngleStart_ = -120.0f;

	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd_ = 60.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime_ = 0.5f;
	// アニメーションの経過時間
	float walkTimer_;

	uint32_t guardAnimationTimer_ = 0;
	static inline const uint32_t kGuardAnimationDuration = 20;                   // のけぞって戻るフレーム数
	static inline const float kGuardAngleX = -std::numbers::pi_v<float> * 0.25f; // X軸ののけぞり角度（

	KamataEngine::Vector3 guardStartRotation_{};

	// デスフラグ
	bool isDead_ = false;

	bool isCollisionDidabled_ = false;

	// キャラクターの当たり判定のサイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.01f;
};
