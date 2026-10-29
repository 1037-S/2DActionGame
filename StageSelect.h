#pragma once
#include "KamataEngine.h"
#include "Fade.h"

using namespace KamataEngine;

class StageManager;

class StageSelect {
public:
	//enum class MenuIndex {
	//	kContinue, // ゲームを続ける
	//	kTitle,    // タイトルへ戻る
	//	kCount
	//};

enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン（ステージ選択中）
		kFadeOut, // フェードアウト
	};

	~StageSelect();

	void Initialize(StageManager* stageManager);
	void Update();
	void Draw();

	int GetSelectedStageIndex() const { return selectedStageIndex_; }
	bool IsSelected() const { return isSelected_; }

	//MenuIndex GetSelectIndex() const { return selectIndex_; }
	//bool IsSelected() const { return isSelected_; }
	//void ResetSelected() { isSelected_ = false; }

private:
	StageManager* stageManager_ = nullptr;

	Phase phase_ = Phase::kFadeIn;
	Fade* fade_ = nullptr;

	// カーソル関連
	int currentCursor_ = 0;
	bool isSelected_ = false;
	int selectedStageIndex_ = -1;

	// 2DUI用
	KamataEngine::Camera camera_;
	uint32_t textureHandleCursor_ = 0;
	KamataEngine::Sprite* spriteCursor_ = nullptr;
};
