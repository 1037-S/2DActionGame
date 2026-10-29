#pragma once
#include "AABBCo.h"
#include "Easing.h"
#include "KamataEngine.h"
#include "Object.h"
#include "Rtt.h"
#include "WTU.h"
#include "Ladder.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;

// マップとの当たり判定情報
struct CollisionMapInfo {
	bool cellCollisionFlag = false; // 天井衝突フラグ
	bool landingFlag = false;       // 着地フラグ
	bool wallHit = false;           // 壁衝突フラグ
	KamataEngine::Vector3 moveAmo;  // 移動量
};
// 角
enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上

	kNumCorner // 要素数
};

// 前方宣言
class MapChipField;
class Enemy;
class ShieldEnemy;

class Player {
public:
	enum class PlayerState {
		kNormal,	// 地上か空中にいる状態
		kLadder,	// 梯子につかまっている
	};

	enum class Behavior {
		kUnknown,   // リクエストなし
		kRoot,      // 通常状態
		kAttack,    // 攻撃中
		kKnockBack, // ノックバック中
	};

	// 攻撃フェーズの型
	enum class AttackPhase {
		kCharge, // 溜め
		kRush,   // 突進
		kLing,   // 余韻
	};

	enum class KnockBack {
		kRoot,   // 通常状態
		kInital, // 初速
		kStop,   // 停止
	};

	// 移動入力
	void PlayerMoveInput();

	// マップ衝突
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void Collision(CollisionMapInfo& info);

	void CollisionUp(CollisionMapInfo& info);
	void CollisionDown(CollisionMapInfo& info);
	void CollisionLeft(CollisionMapInfo& info);
	void CollisionRight(CollisionMapInfo& info);

	// 接地状態の切り替え処理
	void GroundSwitching(const CollisionMapInfo& info);

	// 判定結果を反映して移動させる
	void ReflectionMove(const CollisionMapInfo& info);

	// 天井に接触している場合の処理
	void CeilingCollision(const CollisionMapInfo& info);



	// 壁に接触している場合の処理
	void HitWall(CollisionMapInfo& info);

	// ワールド座標を取得
	Vector3 GetWorldPosition() const;

	// AABBを取得
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Enemy* enemy);

	void OnCollision(const ShieldEnemy* shieldEnemy);

	// 通常行動の初期化関数
	void BehaviorRootInitialize();

	// 通常行動更新
	void BehaviorRootUpdate();

	// 攻撃行動初期化関数
	void BehaviorAttackInitialize();

	// 攻撃行動更新
	void BehaviorAttackUpdate();

	void KnockBackRequest();

	void BehaviorKnockBackInitialize();

	void BehaviorKnockBackUpdate();

	void DecreaseLife();

	// リスポーン（再出撃）時の初期化処理
	void Respawn(const Vector3& position);

	// 初期化
	void Initialize(Model* model, Model* modelAttack, Camera* camera, const Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	void SetLife(int32_t life) { kLife_ = life; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 梯子に接触している場合の関数
	void SetTouchingLadder(bool isTouching, Ladder* ladder) {
		isTouchingLadder_ = isTouching;
		if (isTouching) {
			ladder_ = ladder;
		} else if (state_ != PlayerState::kLadder) {
			ladder_ = nullptr;
		}
	};

	// 接地状態を設定する関数
	void SetOnGround(bool onGround) { onGround_ = onGround; }

	// X軸・Y軸の速度を設定/リセットする関数
	void SetVelocityX(float vx) { velocity_.x = vx; }
	void SetVelocityY(float vy) { velocity_.y = vy; }

	// サイズ定数のゲッター
	static float GetWidth() { return kWidth; }
	static float GetHeight() { return kHeight; }

	void ResetShotRequest() { isShotRequested_ = false; }

	WorldTransform& GetWorldTransform() { return worldTransform_; }
	const Vector3& GetVelocity() { return velocity_; }

	// 攻撃関数
	bool IsAttack() const { return (behavior_ == Behavior::kAttack) && (attackPhase_ == AttackPhase::kRush); }

	LRDirection GetLRDirection() const { return lrDirection_; }

	Behavior GetBehavior() { return behavior_; }

	bool GetIsDead() const { return isDead_; }

	int32_t GetLife() const { return kLife_; }

	bool IsShotRequested() const { return isShotRequested_; }

	bool HasKey() const { return hasKey_; }
	void SetHasKey(bool hasKey) { hasKey_ = hasKey; }

private:
	// 梯子
	PlayerState state_;
	static inline const float kClimbSpeed = 0.08f;
	Ladder* ladder_ = nullptr;

	// ノックバック
	KnockBack knockBack_ = KnockBack::kRoot;

	// ノックバックリクエスト
	KnockBack knockBackRequest_ = KnockBack::kRoot;

	// 振る舞い
	Behavior behavior_ = Behavior::kRoot;

	// 次の振る舞いリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 攻撃ギミックの経過時間カウンター
	uint32_t attackParameter_;

	// 現在の攻撃フェーズ
	AttackPhase attackPhase_;

	Vector3 attackVelocity = {1.0f, 1.0f, 1.0f};

	AABBCo aabb_;

	Easing ease_;
	// CollisionMapInfo collisionMapInfo;

	// ワールド変換
	WTU wtu_;
	Rtt rtt_;
	// ワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform worldTransformAttack_;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// モデル
	Model* playerModel_ = nullptr;
	Model* playerModelAttack_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 移動の基礎数値
	Vector3 velocity_ = {0.0f, 0.0f, 0.0f};

	// 加速度
	static inline const float kAcceleration = 0.01f;

	// 最高速度
	static inline const float kLimitRunSpeed = 0.1f;

	// 左右の初期向き
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間(秒)
	static inline const float kTimeTurn_ = 0.3f;

	// ジャンプの基礎数値
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.65f;
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.04f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 1.0f;

	// ノックバック用パラメータ
	uint32_t knockBackTimer_ = 0;
	static inline const uint32_t kKnockBackTime = 10;     // ノックバック時間(フレーム)
	static inline const uint32_t kKnockBackStopTime = 15; // 停止して体制を立て直す時間(フレーム)

	static inline const float kKnockBackSpeedX = 0.3f; // 横方向の吹き飛び速度
	static inline const float kKnockBackSpeedY = 0.3f; // 上方向の跳ね上がり速度

	// 着地時の速度減衰率
	// 天井と床
	static inline const float kAttenuationLanding_ = 0.1f;
	// 壁
	static inline const float kAttenuationWall_ = 0.1f;
	// 梯子との当たり判定フラグ
	bool isTouchingLadder_ = false;

	// 接地フラグ
	bool onGround_ = false;

	bool hasAirAttacked_ = false;

	// デスフラグ
	bool isDead_ = false;

	// 攻撃フラグ
	bool isAttack_ = false;

	// キャラクターの当たり判定のサイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.01f;

	// 落下判定のY座標
	static inline const float kFallDeathY = -1.0f;

	// 壁キック用パラメータ
	bool isWallContact_ = false; // 壁接触フラグ
	int32_t wallDirection_ = 0;  // 接触している壁の方向 (-1: 左壁, 1: 右壁)

	static inline const float kWallJumpAccelerationY = kJumpAcceleration; // 壁キックの縦初速
	static inline const float kWallJumpAccelerationX = 0.25f;             // 壁キックの横初速（壁から離れる方向）
	static inline const float kWallSlideSpeed = -0.15f;

	// 発射リクエスト
	bool isShotRequested_ = false;

	bool hasKey_ = false; // 鍵の所持フラグ

	// 初期残機
	int32_t kLife_ = 3;

	// 最大残機
	int32_t kMaxLife_ = 99;

	// SEの音声データ用ハンドル
	uint32_t jumpSEHandle_ = 0;
};
