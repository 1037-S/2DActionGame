
#pragma once
#include "KamataEngine.h"
#include "Object.h"
#include "WTU.h"

using namespace KamataEngine;

class Door {
public:

	// 当たり判定
	AABB GetAABB();

	void Initialize(Model* model, const Vector3& position, uint8_t doorID, uint8_t pairID);
	void Update(const Vector3& playerPos);
	void Draw(const Camera& camera);

	bool IsEntered() const { return isEntered_; }
	void ResetEntered() { isEntered_ = false; } // 移動後にフラグをリセットする用

	// ドアに入ったかどうかの判定
	uint8_t GetDoorID() const { return doorID_; }
	uint8_t GetPairID() const { return pairID_; }
	const Vector3& GetPosition() const { return worldTransform_.translation_; }

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	WTU wtu_;

	uint8_t doorID_ = 0;           // 自分のドアID (例: D0)
	uint8_t pairID_ = 0;           // ペアとなる移動先のドアID (例: D1)
	uint8_t targetStageIndex_ = 0; // 遷移先のステージ番号 (サブIDで指定)
	bool isEntered_ = false;       // 進入フラグ
	// 当たり判定のサイズ
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;
};
