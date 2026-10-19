#pragma once
#include "KamataEngine.h"
// #include "TitleFont.h"
// #include "TitleFontPlayer.h"
#include "Fade.h"
#include "WTU.h"

class TitleScene {
public:
	enum class Phase {
		kFadein,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	~TitleScene();

	// フェーズ切り替え
	void ChangePhase();

	void Initialize();
	void Update();
	void Draw();

	// デスフラグのヘッダー
	bool IsFinshed() const { return finished_; }

private:
	// TitleFont* titleFont_ = nullptr;
	// TitleFontPlayer* PlayerFont_ = nullptr;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadein;

	// ワールド変換データ
	WTU wtu_;

	uint32_t textureHandleTitle_ = 0; // HIRO Action.png
	uint32_t textureHandlePush_ = 0;  // titleFont2.png
	KamataEngine::Sprite* spriteTitle_ = nullptr;
	KamataEngine::Sprite* spritePush_ = nullptr;
	KamataEngine::WorldTransform worldTransformTitle_;
	KamataEngine::WorldTransform worldTransformPlayer_;

	KamataEngine::Camera camera_;

	KamataEngine::Model* fontModelA_ = nullptr;
	KamataEngine::Model* fontModelB_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;

	// 終了フラグ
	bool finished_ = false;
};
