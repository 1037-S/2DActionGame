#pragma once
#include "KamataEngine.h"

class PauseScene {
public:
	enum class MenuIndex {
		kContinue, // ゲームを続ける
		kTitle,    // タイトルへ戻る
		kCount
	};

	void Initialize();
	void Update();
	void Draw();

	MenuIndex GetSelectIndex() const { return selectIndex_; }
	bool IsSelected() const { return isSelected_; }
	void ResetSelected() { isSelected_ = false; }

private:
	MenuIndex selectIndex_ = MenuIndex::kContinue;
	bool isSelected_ = false;

	uint32_t textureBg_ = 0;       // 背景（黒半透明など）
	uint32_t textureContinue_ = 0; // 「Continue」文字画像
	uint32_t textureTitle_ = 0;    // 「Title」文字画像
	uint32_t textureCursor_ = 0;   // カーソル画像

	// 描画用のスプライト
	KamataEngine::Sprite* spriteBg_ = nullptr;
	KamataEngine::Sprite* spriteContinue_ = nullptr;
	KamataEngine::Sprite* spriteTitle_ = nullptr;
	KamataEngine::Sprite* spriteCursor_ = nullptr;
};
