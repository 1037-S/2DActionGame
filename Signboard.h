#pragma once
#include "KamataEngine.h"
#include "WTU.h"
#include <string>

// 前方宣言
class Player;

class Signboard {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, uint32_t textTextureHandle);
	void Update(const Player* player);
	void Draw3D(); // 3Dモデルの描画
	void DrawUI(); // 2D操作説明テキスト/スプライトの描画

	// 個別メッセージのゲッター・セッター
	void SetMessage(const std::string& message) { message_ = message; }
	const std::string& GetMessage() const { return message_; }
	// 看板メッセージ表示中かどうかを取得
	bool IsReading() const { return isReading_; }

private:
	KamataEngine::WorldTransform worldTransform_;
	WTU wtu_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	// プレイヤーが近づいているかどうかのフラグ
	bool isNearPlayer_ = false; // 近くにいるか
	bool isReading_ = false;    // 看板を読んでいる状態か

	// 看板ごとのメッセージテキスト
	std::string message_;

	// UI用スプライト等
	uint32_t textTextureHandle_ = 0;
	KamataEngine::Sprite* textSprite_ = nullptr;
};
