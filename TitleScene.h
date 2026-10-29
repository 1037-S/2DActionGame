#pragma once
#include "KamataEngine.h"
// #include "TitleFont.h"
// #include "TitleFontPlayer.h"
#include "Fade.h"
#include "WTU.h"

using namespace KamataEngine;

class TitleScene {
public:
	enum class MenuIndex {
		kStart,    // ゲームを始める
		kTutorial, // チュートリアル
		kCount
	};

	enum class Phase {
		kFadein,  // フェードイン
		kMain,    // メイン部
		kTutorial, // 操作説明表示中
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

	MenuIndex GetSelectIndex() const { return selectIndex_; }
	bool IsSelected() const { return isSelected_; }
	void ResetSelected() { isSelected_ = false; }

private:
	MenuIndex selectIndex_ = MenuIndex::kStart;
	bool isSelected_ = false;
	// TitleFont* titleFont_ = nullptr;
	// TitleFontPlayer* PlayerFont_ = nullptr;
	// Input* input = nullptr;
	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadein;

	// ワールド変換データ
	WTU wtu_;

	uint32_t textureHandleTitle_ = 0;    //
	uint32_t textureHandleStart_ = 0;    // ゲームスタート用
	uint32_t textureHandleTutorial_ = 0; // 操作説明用
	uint32_t textureHandleCursor_ = 0;   // 選択カーソル用（任意）
	uint32_t textureHandleHowToPlay_ = 0; // ★操作説明の背景/画像用1
	uint32_t textureHandleHowToPlay1_ = 0; // ★操作説明の背景/画像用2
	uint32_t textureHandleHowToPlay2_ = 0; // ★操作説明の背景/画像用3
	uint32_t textureHandleHowToPlay3_ = 0; // ★操作説明の背景/画像用4
	uint32_t textureHandleHowToPlay4_ = 0; // ★操作説明の背景/画像用5
	
	KamataEngine::Sprite* spriteTitle_ = nullptr;
	KamataEngine::Sprite* spriteStart_ = nullptr;
	KamataEngine::Sprite* spriteTutorial_ = nullptr;
	KamataEngine::Sprite* spriteCursor_ = nullptr;
	KamataEngine::Sprite* spriteHowToPlay_ = nullptr; // ★操作説明画面用スプライト
	KamataEngine::Sprite* spriteHowToPlay1_ = nullptr; // ★操作説明画面用スプライト
	KamataEngine::Sprite* spriteHowToPlay2_ = nullptr; // ★操作説明画面用スプライト
	KamataEngine::Sprite* spriteHowToPlay3_ = nullptr; // ★操作説明画面用スプライト
	KamataEngine::Sprite* spriteHowToPlay4_ = nullptr; // ★操作説明画面用スプライト
	
	KamataEngine::WorldTransform worldTransformTitle_;
	KamataEngine::WorldTransform worldTransformPlayer_;

	KamataEngine::Camera camera_;

	KamataEngine::Model* fontModelA_ = nullptr;
	KamataEngine::Model* fontModelB_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;

	// 終了フラグ
	bool finished_ = false;

	// 音声のデータ自体のハンドル
	uint32_t bgmDataHandle_ = 0;
	// 再生中トラックのハンドル
	uint32_t bgmVoiceHandle_ = 0;
};
