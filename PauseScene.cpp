#include "PauseScene.h"

using namespace KamataEngine;

void PauseScene::Initialize() {
	selectIndex_ = MenuIndex::kContinue;
	isSelected_ = false;

	// 画像の読み込み（※プロジェクトの画像パスに合わせて変更してください）
	textureBg_ = TextureManager::Load("pause/bg.png");
	textureContinue_ = TextureManager::Load("pause/continue.png");
	textureTitle_ = TextureManager::Load("pause/title.png");
	textureCursor_ = TextureManager::Load("pause/cursor.png");

	// スプライト生成例（中央配置など）
	Vector2 screenCenter = {640.0f, 360.0f};
	spriteContinue_ = Sprite::Create(textureContinue_, {640.0f, 300.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	spriteTitle_ = Sprite::Create(textureTitle_, {640.0f, 420.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	spriteCursor_ = Sprite::Create(textureCursor_, {340.0f, 300.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
}

void PauseScene::Update() {
	Input* input = Input::GetInstance();

	// カーソル移動
	if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_W)) {
		int index = static_cast<int>(selectIndex_) - 1;
		if (index < 0)
			index = static_cast<int>(MenuIndex::kCount) - 1;
		selectIndex_ = static_cast<MenuIndex>(index);
	}
	if (input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_S)) {
		int index = static_cast<int>(selectIndex_) + 1;
		if (index >= static_cast<int>(MenuIndex::kCount))
			index = 0;
		selectIndex_ = static_cast<MenuIndex>(index);
	}

	// 決定
	if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_Z)) {
		isSelected_ = true;
	}

	// カーソル位置の更新（選択インデックスに応じてY座標を切り替え）
	if (spriteCursor_) {
		float cursorY = (selectIndex_ == MenuIndex::kContinue) ? 300.0f : 420.0f;
		spriteCursor_->SetPosition({340.0f, cursorY});
	}
}

void PauseScene::Draw() {
	// スプライト描画の開始
	Sprite::PreDraw();

	// 1. 背景（半透明オーバーレイ）
	if (spriteBg_)
		spriteBg_->Draw();

	// 2. メニュー項目
	if (spriteContinue_)
		spriteContinue_->Draw();
	if (spriteTitle_)
		spriteTitle_->Draw();

	// 3. 選択カーソル
	if (spriteCursor_)
		spriteCursor_->Draw();

	Sprite::PostDraw();

#ifdef _DEBUG
	// 開発用簡易UI（ImGuiで表示・デバッグ用）
	ImGui::Begin("PAUSE MENU");
	ImGui::Text(selectIndex_ == MenuIndex::kContinue ? "> 1. Continue" : "  1. Continue");
	ImGui::Text(selectIndex_ == MenuIndex::kTitle ? "> 2. Return to Title" : "  2. Return to Title");
	ImGui::End();
#endif
}
