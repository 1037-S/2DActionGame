#define NOMINMAX

#include "Player.h"
#include "MapChipField.h"
#include "WM4.h"

void Player::PlayerMoveInput() {
	
	// 梯子をつかんでいるか否か
	switch (state_) {
	case Player::PlayerState::kNormal:
	if (behavior_ != Behavior::kRoot) {
		return;
	}
	// 左右移動操作

	if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
		// 左右加速
		// 地上と空中で加速度を変える（空中は少しだけ動かせるように小さく設定）
		float currentAcceleration = onGround_ ? kAcceleration : (kAcceleration * 0.5f);
		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			// 左移動中の右入力
			if (velocity_.x < 0.0f) {
				// 速度と逆に入力している間は急ブレーキが入る
				velocity_.x *= (1.0f - currentAcceleration);
			}
			acceleration.x += currentAcceleration;
			if (onGround_ && lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
				// turnFirstRotationY_ = worldTransform_.rotation_.y;
				// turnTimer_ = kTimeTurn_;
			}
		} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 右移動中の左入力
			if (velocity_.x > 0.0f) {
				// 速度と逆に入力している間は急ブレーキが入る
				velocity_.x *= (1.0f - currentAcceleration);
			}
			acceleration.x -= currentAcceleration;
			if (onGround_ && lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;
				// turnFirstRotationY_ = worldTransform_.rotation_.y;
				// turnTimer_ = kTimeTurn_;
			}
		}
		// 加速/減速
		velocity_.x += acceleration.x;
		// 最高速度の制限
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	} else {
		// 但し、非入力時は移動減衰をかける
		velocity_.x *= (1.0f - kAcceleration);
	}

	if (onGround_) {
		// 空中
		if (Input::GetInstance()->PushKey(DIK_Z)) {
			// ジャンプ初速
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
		}
	} else {
		// 空中：壁キック判定
		if (isWallContact_ && Input::GetInstance()->TriggerKey(DIK_Z)) {

			velocity_.y = kWallJumpAccelerationY;

			// 壁と反対方向に跳ね返す
			velocity_.x = -wallDirection_ * kWallJumpAccelerationX;

			// 向きを反転
			if (wallDirection_ == -1) {
				lrDirection_ = LRDirection::kRight;
				worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
			} else {
				lrDirection_ = LRDirection::kLeft;
				worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;
			}

			// 空中攻撃フラグのリセット
			hasAirAttacked_ = false;
		} else {
			// 重力落下
			velocity_.y += -kGravityAcceleration;

			// 壁に張り付いている間は落下速度を緩やかにする（壁スライド）
			if (isWallContact_ && velocity_.y < 0.0f) {
				velocity_.y = std::max(velocity_.y, kWallSlideSpeed);
			} else {
				// 通常の落下速度制限
				velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
			}
		}
	}

	// 梯子をつかんでいる状態にする判定
	if (isTouchingLadder_) {
		if (Input::GetInstance()->PushKey(DIK_UP)||
			Input::GetInstance()->PushKey(DIK_DOWN)) {

			state_ = PlayerState::kLadder;
			velocity_ = {0.0f, 0.0f, 0.0f};
			onGround_ = false; // 梯子移動中は空中扱いにして着地処理による干渉を防ぐ

			if (ladder_) {
			// プレイヤーのX座標を梯子の中心に吸着させる
			worldTransform_.translation_.x = ladder_->GetPosX();
			}
		}
	}


	break;
	// --- 梯子 ---
	case Player::PlayerState::kLadder:
		velocity_ = {0.0f, 0.0f, 0.0f};

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y = kClimbSpeed;
		} else if (Input::GetInstance()->PushKey(DIK_DOWN)) {
			velocity_.y = -kClimbSpeed;
		}

		// 離脱条件その1 ジャンプしたとき
		if (Input::GetInstance()->PushKey(DIK_Z)) {
			state_ = PlayerState::kNormal;
			velocity_.y = kJumpAcceleration;
		}
		// 離脱条件その2 梯子から離れたとき
		if (!isTouchingLadder_) {
			state_ = PlayerState::kNormal;
		}
		
		break;
	}

}



Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom(右下)
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom(左下)
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop(右上)
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop(左上)
	};
	Vector3 result;

	result.x = center.x + offsetTable[static_cast<uint32_t>(corner)].x;
	result.y = center.y + offsetTable[static_cast<uint32_t>(corner)].y;
	result.z = center.z + offsetTable[static_cast<uint32_t>(corner)].z;

	return result;
}

void Player::Collision(CollisionMapInfo& info) {
	Player::CollisionUp(info);
	Player::CollisionDown(info);
	Player::CollisionLeft(info);
	Player::CollisionRight(info);
}

void Player::CollisionUp(CollisionMapInfo& info) {
	// 上昇あり?
	if (info.moveAmo.y <= 0) {
		return;
	}
	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	Vector3 newPos = {
	    worldTransform_.translation_.x + info.moveAmo.x,
	    worldTransform_.translation_.y + info.moveAmo.y,
	    worldTransform_.translation_.z + info.moveAmo.z,
	};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(newPos, static_cast<Corner>(i));
	}

	MapchipType mapchipType;
	MapchipType mapchipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kLeftTop]);
	mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapchipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapchipType == MapchipType::kBlock && mapchipTypeNext != MapchipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kRightTop]);
	mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapchipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapchipType == MapchipType::kBlock && mapchipTypeNext != MapchipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット?
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kRightTop]);
		// めり込み先ブロックの範囲矩形
		IndexSet indexSetNow;
		std::array<Vector3, kNumCorner> positionsOld;

		for (uint32_t i = 0; i < positionsOld.size(); ++i) {
			positionsOld[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
		}
		indexSetNow = mapChipField_->GetMapchipIndexSetByPosition(positionsOld[kRightTop]);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.moveAmo.y = std::max(0.0f, (rect.bottom - worldTransform_.translation_.y) - (kHeight / 2 + kBlank));
			// 天井に当たったことを記録
			info.cellCollisionFlag = true;
		}
	}
}

void Player::CollisionDown(CollisionMapInfo& info) {
	// 下降あり?
	if (info.moveAmo.y >= 0) {
		return;
	}
	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	Vector3 newPos = {
	    worldTransform_.translation_.x + info.moveAmo.x,
	    worldTransform_.translation_.y + info.moveAmo.y,
	    worldTransform_.translation_.z + info.moveAmo.z,
	};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(newPos, static_cast<Corner>(i));
	}
	MapchipType mapchipType;
	MapchipType mapchipTypeNext;
	// 真下の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapchipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapchipType == MapchipType::kBlock && mapchipTypeNext != MapchipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kRightBottom]);
	mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapchipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapchipType == MapchipType::kBlock && mapchipTypeNext != MapchipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット?
	if (hit) {

		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kLeftBottom]);
		IndexSet indexSetNow;
		std::array<Vector3, kNumCorner> positionsOld;

		for (uint32_t i = 0; i < positionsOld.size(); ++i) {
			positionsOld[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
		}
		indexSetNow = mapChipField_->GetMapchipIndexSetByPosition(positionsOld[kRightBottom]);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kLeftBottom]);
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.moveAmo.y = std::min(0.0f, (rect.top - worldTransform_.translation_.y) + (kHeight / 2 + kBlank));
			// 天井に当たったことを記録
			info.landingFlag = true;
		}
	}
}

void Player::CollisionLeft(CollisionMapInfo& info) {
	// 左移動あり?
	// if (info.moveAmo.x >= 0)
	//{
	//	return;
	//}
	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	// 移動量がない場合でも、わずかに左（-kBlank）をチェックする
	float checkX = std::min(info.moveAmo.x, -kBlank);

	Vector3 newPos = {
	    worldTransform_.translation_.x + checkX,
	    worldTransform_.translation_.y + info.moveAmo.y,
	    worldTransform_.translation_.z + info.moveAmo.z,
	};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(newPos, static_cast<Corner>(i));
	}
	MapchipType mapchipType;
	// MapchipType mapchipTypeNext;
	//  真左の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kLeftTop]);
	mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// mapchipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex-1, indexSet.yIndex);
	if (mapchipType == MapchipType::kBlock) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	// mapchipTypeNext = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapchipType == MapchipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット?
	if (hit) {
		// 実際に左移動していた場合のみ押し戻し処理を行う
		if (info.moveAmo.x < 0.0f) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kLeftBottom]);
			// めり込み先ブロックの範囲矩形
			IndexSet indexSetNow;
			std::array<Vector3, kNumCorner> positionsOld;

			for (uint32_t i = 0; i < positionsOld.size(); ++i) {
				positionsOld[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
			}
			indexSetNow = mapChipField_->GetMapchipIndexSetByPosition(positionsOld[kLeftTop]);
			if (indexSetNow.xIndex != indexSet.xIndex) {
				// めり込み先ブロックの範囲矩形
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				info.moveAmo.x = std::min((rect.right - worldTransform_.translation_.x) + (kWidth / 2 + kBlank), 0.0f);
				// 壁に当たったことを記録
				info.wallHit = true;
			}
		}
		// --- 壁接触を記録 ---
		isWallContact_ = true;
		wallDirection_ = -1;
	}
}

void Player::CollisionRight(CollisionMapInfo& info) {
	// 右移動あり?
	// if (info.moveAmo.x <= 0)
	//{
	//	return;
	//}
	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	// 移動量がない場合でも、わずかに右（+kBlank）をチェックする
	float checkX = std::max(info.moveAmo.x, kBlank);

	Vector3 newPos = {
	    worldTransform_.translation_.x + checkX,
	    worldTransform_.translation_.y + info.moveAmo.y,
	    worldTransform_.translation_.z + info.moveAmo.z,
	};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(newPos, static_cast<Corner>(i));
	}
	MapchipType mapchipType;
	// MapchipType mapchipTypeNext;
	//  真右の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	IndexSet indexSet;
	indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kRightTop]);
	mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapchipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kRightBottom]);
	mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapchipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット?
	if (hit) {
		// 実際に右移動していた場合のみ押し戻し処理を行う
		if (info.moveAmo.x > 0.0f) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kRightBottom]);
			std::array<Vector3, kNumCorner> positionsOld;

			for (uint32_t i = 0; i < positionsOld.size(); ++i) {
				positionsOld[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
			}
			IndexSet indexSetNow;
			indexSetNow = mapChipField_->GetMapchipIndexSetByPosition(positionsOld[kRightBottom]);
			if (indexSetNow.xIndex != indexSet.xIndex) {

				// めり込み先ブロックの範囲矩形
				MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
				info.moveAmo.x = std::max((rect.left - worldTransform_.translation_.x) - (kWidth / 2 + kBlank), 0.0f);
				// 壁に当たったことを記録
				info.wallHit = true;
			}
		}
		// --- 壁接触を記録 ---
		isWallContact_ = true;
		wallDirection_ = 1;
	}
}

void Player::GroundSwitching(const CollisionMapInfo& info) {
	// 自キャラが接地状態か否か?
	if (onGround_) {

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 移動後の4つの角の座標
			std::array<Vector3, kNumCorner> positionsNew;

			Vector3 newPos = {
			    worldTransform_.translation_.x + info.moveAmo.x,
			    worldTransform_.translation_.y + info.moveAmo.y,
			    worldTransform_.translation_.z + info.moveAmo.z,
			};

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(newPos, static_cast<Corner>(i));
			}
			// 落下判定
			MapchipType mapchipType;
			// 真下の当たり判定を行う
			bool hit = false;
			// 左下点の判定
			IndexSet indexSet;
			indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0.0f, -kGravityAcceleration - 0.01f, 0.0f));
			mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapchipType == MapchipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapchipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0.0f, -kGravityAcceleration - 0.01f, 0.0f));
			mapchipType = mapChipField_->GetMapchipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapchipType == MapchipType::kBlock) {
				hit = true;
			}
			// 落下開始
			if (!hit) {
				onGround_ = false;
			}

			// 落下なら空中状態に切り替える
		}
	} else {
		// 空中状態

		// 着地フラグ
		if (info.landingFlag) {
			// 着地状態に切り替える(落下しなくさせる)
			onGround_ = true;

			hasAirAttacked_ = false;

			// 着地時にXの速度を減衰させる
			velocity_.x *= (1.0f - kAttenuationLanding_);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}

void Player::ReflectionMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_.x += info.moveAmo.x;
	worldTransform_.translation_.y += info.moveAmo.y;
	worldTransform_.translation_.z += info.moveAmo.z;
}

void Player::CeilingCollision(const CollisionMapInfo& info) {
	// 天井に当たったか?
	if (info.cellCollisionFlag) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

void Player::HitWall(CollisionMapInfo& info) {
	// 壁に接触したことによる減速
	if (info.wallHit) {
		velocity_.x *= (1.0f - kAttenuationWall_);
	}
}

Vector3 Player::GetWorldPosition() const {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	if (IsAttack()) {
		return;
	}
	(void)enemy;
	// デスフラグが発生する
	isDead_ = true;
}

void Player::OnCollision(const ShieldEnemy* shieldEnemy) {
	if (IsAttack() || behavior_ == Behavior::kKnockBack) {
		return;
	}
	(void)shieldEnemy;
	// デスフラグが発生する
	isDead_ = true;
}

void Player::BehaviorRootInitialize() {
	// スケールを元に戻す
	// worldTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
}

void Player::BehaviorRootUpdate() {

	
	// 移動入力
	PlayerMoveInput();
	


	if (!isDead_ && Input::GetInstance()->TriggerKey(DIK_C)) {
		isShotRequested_ = true;
	}

	// 旋回防御
	if (turnTimer_ > 0.0f) {
		// 毎フレームで旋回タイマーをカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;
		if (turnTimer_ < 0.0f) {
			turnTimer_ = 0.0f;
		}
		// 左右のプレイヤーの角度テーブル
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,       // 右向きの場合
		    std::numbers::pi_v<float> * 3.0f / 2.0f // 左向きの場合
		};
		// 状態に応じた角度を取得
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = wtu_.Lerp(turnFirstRotationY_, destinationRotationY, kTimeTurn_, turnTimer_);
	}

	// 攻撃キーを押したら
	if (Input::GetInstance()->PushKey(DIK_X)) {
		if (onGround_ || !hasAirAttacked_) {
			if (!onGround_) {

				hasAirAttacked_ = true;
			}

			// 攻撃ビヘイビアをリクエスト
			behaviorRequest_ = Behavior::kAttack;
		}
	}
}

void Player::BehaviorAttackInitialize() {
	attackParameter_ = 0;

	attackPhase_ = AttackPhase::kCharge;
}

void Player::BehaviorAttackUpdate() {
	// 予備動作
	attackParameter_++;

	Vector3 velocity{};

	switch (attackPhase_) {
	case Player::AttackPhase::kCharge:
	default: {
		float t = static_cast<float>(attackParameter_) / 10;
		worldTransform_.scale_.z = ease_.EaceOut(1.0f, 0.3f, t);
		worldTransform_.scale_.y = ease_.EaceOut(1.0f, 1.6f, t);
		if (attackParameter_ >= 10) {
			attackPhase_ = AttackPhase::kRush;
			attackParameter_ = 0;
		}
		break;
	}
	case Player::AttackPhase::kRush: {
		float t = static_cast<float>(attackParameter_) / 10;
		worldTransform_.scale_.z = ease_.EaceOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = ease_.EaceIn(1.6f, 0.7f, t);
		if (attackParameter_ >= 10) {
			attackPhase_ = AttackPhase::kLing;
			attackParameter_ = 0;
		}
		if (lrDirection_ == LRDirection::kRight) {
			velocity.x += attackVelocity.x;
		} else {
			velocity.x -= attackVelocity.x;
		}
		break;
	}
	case Player::AttackPhase::kLing: {
		float t = static_cast<float>(attackParameter_) / 10;
		worldTransform_.scale_.z = ease_.EaceOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = ease_.EaceOut(0.7f, 1.0f, t);
		if (attackParameter_ >= 10) {
			behaviorRequest_ = Behavior::kRoot;
		}
		break;
	}
	}
	velocity_ = velocity;

	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;
	worldTransformAttack_.scale_ = {1.0f, 1.0f, 1.0f};

	wtu_.WorldTransformUpdate(worldTransformAttack_);
}

void Player::KnockBackRequest() { behaviorRequest_ = Behavior::kKnockBack; }

void Player::BehaviorKnockBackInitialize() {

	// 攻撃で変化したスケールを元に戻す
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	knockBack_ = KnockBack::kInital;
	knockBackTimer_ = kKnockBackTime;

	// 向いている方向と逆に吹き飛ばす（左向きなら右へ、右向きなら左へ）
	if (lrDirection_ == LRDirection::kRight) {
		velocity_.x = -kKnockBackSpeedX;
	} else {
		velocity_.x = kKnockBackSpeedX;
	}

	// 上方向への初速を与えて跳ねさせる
	// velocity_.y = kKnockBackSpeedY;
	// onGround_ = false;
}

void Player::BehaviorKnockBackUpdate() {
	velocity_.y += -kGravityAcceleration;
	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

	switch (knockBack_) {

	case Player::KnockBack::kInital:
		// タイマーのカウントダウン
		if (knockBackTimer_ > 0) {
			knockBackTimer_--;
		} else {
			// タイマー終了していたら通常状態へ復帰
			knockBack_ = KnockBack::kStop;
			knockBackTimer_ = kKnockBackStopTime;

			velocity_.x = 0.0f;
		}
		break;
	case Player::KnockBack::kStop:
		// 停止フェーズ：硬直して体制を立て直す
		velocity_.x = 0.0f; // 完全に移動停止

		if (knockBackTimer_ > 0) {
			knockBackTimer_--;
		} else {
			// ノックバック終了、通常状態へ戻る
			knockBack_ = KnockBack::kRoot;
			behaviorRequest_ = Behavior::kRoot;
		}
		break;
	default:
		break;
	}
}

void Player::DecreaseLife() {
	if (kLife_ > 0) {
		kLife_--;
	}
}

void Player::Respawn(const Vector3& position) {
	worldTransform_.translation_ = position;
	velocity_ = {0.0f, 0.0f, 0.0f};
	behavior_ = Behavior::kRoot;
	behaviorRequest_ = Behavior::kUnknown;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	isDead_ = false;
}

void Player::Initialize(Model* model, Model* modelAttack, Camera* camera, const Vector3& position) {

	// NULLポインタチェック
#ifdef _DEBUG
	assert(model);
	assert(camera);
#endif // _DEBUG
	playerModel_ = model;
	playerModelAttack_ = modelAttack;
	camera_ = camera;

	behavior_ = Behavior::kRoot;
	behaviorRequest_ = Behavior::kUnknown;
	isDead_ = false;
	isShotRequested_ = false;
	velocity_ = {0.0f, 0.0f, 0.0f};

	// ワールド変換の初期化
	wtu_.WorldTransformInitialize(worldTransform_);
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	wtu_.WorldTransformInitialize(worldTransformAttack_);
	worldTransformAttack_.Initialize();
}

void Player::Update() {

	if (!isDead_ && behavior_ == Behavior::kRoot) {
		if (Input::GetInstance()->TriggerKey(DIK_C)) {
			isShotRequested_ = true;
		}
	}

	// 移動入力

	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		default:
			break;
			// ルートビヘイビアの初期化
			// BehaviorRootInitialize();
		case Player::Behavior::kAttack:
			// 攻撃ビヘイビアの初期化
			BehaviorAttackInitialize();
			break;
		case Player::Behavior::kKnockBack:
			// ノックバックビヘイビアの初期化
			BehaviorKnockBackInitialize();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {

	case Player::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Player::Behavior::kKnockBack:
		BehaviorKnockBackUpdate();
		break;
	};

	isWallContact_ = false;
	wallDirection_ = 0;

	// 移動量に速度の値をコピー
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.moveAmo = velocity_;

	// マップ衝突をチェック
	Collision(collisionMapInfo);
	ReflectionMove(collisionMapInfo);
	CeilingCollision(collisionMapInfo);
	GroundSwitching(collisionMapInfo);
	HitWall(collisionMapInfo);

	if (worldTransform_.translation_.y < kFallDeathY) {
		isDead_ = true;
	}

	wtu_.WorldTransformUpdate(worldTransform_);
}

void Player::Draw() {
	if (!isDead_) {
		// 3Dモデルを描画する
		playerModel_->Draw(worldTransform_, *camera_);

		
	}
}
