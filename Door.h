
#pragma once
#include "KamataEngine.h"
#include "WTU.h"

class Door {
public:
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, uint8_t targetStageIndex);
	void Update(const KamataEngine::Vector3& playerPos);
	void Draw(const KamataEngine::Camera& camera);

	// ドアに入ったかどうかの判定
	bool IsEntered() const { return isEntered_; }
	uint8_t GetTargetStageIndex() const { return targetStageIndex_; }

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	WTU wtu_;

	uint8_t targetStageIndex_ = 0; // 遷移先のステージ番号 (サブIDで指定)
	bool isEntered_ = false;       // 進入フラグ
	float interactRadius_ = 1.0f;  // 当たり判定半径
};
