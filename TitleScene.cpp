#include "TitleScene.h"

using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete fontModelA_;
	delete playerModel_;

	delete fade_;

	delete spriteTitle_;
	delete spriteStart_;
	delete spriteTutorial_;
	delete spriteCursor_;
	delete spriteHowToPlay_;
	delete spriteHowToPlay1_;
	delete spriteHowToPlay2_;
	delete spriteHowToPlay3_;
	delete spriteHowToPlay4_;
}

void TitleScene::ChangePhase() {

	Input* input = Input::GetInstance();

	switch (phase_) {
	case TitleScene::Phase::kFadein:
		fade_->Update();
		if (fade_->IsFinshed()) {
			phase_ = Phase::kMain;
		}
		break;
	case TitleScene::Phase::kMain:
		// --- 1. 上下キー（または W/S キー）で選択肢の切り替え ---
		if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_W)) {
			int index = static_cast<int>(selectIndex_) - 1;
			if (index < 0) {
				index = static_cast<int>(MenuIndex::kCount) - 1;
			}
			selectIndex_ = static_cast<MenuIndex>(index);
		}
		if (input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_S)) {
			int index = static_cast<int>(selectIndex_) + 1;
			if (index >= static_cast<int>(MenuIndex::kCount)) {
				index = 0;
			}
			selectIndex_ = static_cast<MenuIndex>(index);
		}

		// --- 2. SPACE または ENTER キーで決定 ---
		if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN)) {
			isSelected_ = true; // 決定フラグを立てる
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;

		case TitleScene::Phase::kTutorial:
		// ★ 操作説明表示中に SPACE、ENTER、B (または BACKSPACE, ESCAPE) キー等を押したらメニューに戻る
		if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_ESCAPE)) {
			phase_ = Phase::kMain;
		}
		break;

	case TitleScene::Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinshed()) {
			finished_ = true;
		}
		break;
	}
}

void TitleScene::Initialize() {

	// 背景用モデルの読み込み
	fontModelA_ = Model::CreateFromOBJ("SkyDome", true);
	worldTransformTitle_.Initialize();

	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 2.0f);

	// カメラの生成
	camera_.farZ = 1000.0f;
	camera_.Initialize();

	textureHandleTitle_ = TextureManager::Load("titleFont/REI Action.png");
	 textureHandleStart_ = TextureManager::Load("titleFont/start.png");
	 textureHandleTutorial_ = TextureManager::Load("titleFont/tutorial.png");
	 textureHandleHowToPlay_ = TextureManager::Load("titleFont/howToPlay.png");
	// タイトルロゴ (画面中央上部)
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {640.0f, 200.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	spriteStart_ = Sprite::Create(textureHandleStart_, {640.0f, 450.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	spriteTutorial_ = Sprite::Create(textureHandleTutorial_, {640.0f, 550.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	
	if (textureHandleHowToPlay_) {
		spriteHowToPlay_ = Sprite::Create(textureHandleHowToPlay_, {640.0f, 360.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
	}
}

void TitleScene::Update() {

	ChangePhase();

	wtu_.WorldTransformUpdate(worldTransformTitle_);

	// camera_.TransferMatrix();
	camera_.UpdateMatrix();
}

void TitleScene::Draw() {

	Model::PreDraw();
	if (fontModelA_) {
		fontModelA_->Draw(worldTransformTitle_, camera_);
	}
	Model::PostDraw();

	// 2Dスプライトの描画
	Sprite::PreDraw();
	if (phase_ == Phase::kTutorial) {
		// ★ 操作説明フェーズの場合は操作説明画像を表示
		if (spriteHowToPlay_) {
			spriteHowToPlay_->Draw();
		}
	} else {
		// 通常のメニュー画面の表示
		if (spriteTitle_) {
			spriteTitle_->Draw();
		}
		if (spriteStart_) {
			spriteStart_->Draw();
		}
		if (spriteTutorial_) {
			spriteTutorial_->Draw();
		}
		Sprite::PostDraw();

		fade_->Draw();
	}
}