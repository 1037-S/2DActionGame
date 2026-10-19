#include "TitleScene.h"

using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete fontModelA_;
	delete playerModel_;

	delete fade_;

	delete spriteTitle_;
	delete spritePush_;
}

void TitleScene::ChangePhase() {
	switch (phase_) {
	case TitleScene::Phase::kFadein:
		fade_->Update();
		if (fade_->IsFinshed()) {
			phase_ = Phase::kMain;
		}
		break;
	case TitleScene::Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
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

	// プレイヤーモデルの読み込みと位置・回転の設定
	playerModel_ = Model::CreateFromOBJ("player", true);
	worldTransformPlayer_.Initialize();

	// --- 3Dプレイヤーの位置・回転設定 ---
	worldTransformPlayer_.translation_ = {-2.0f, -1.0f, 0.0f}; // 画面左側に配置
	worldTransformPlayer_.rotation_ = {0.0f, 1.57f, 0.0f};
	worldTransformPlayer_.scale_ = {5.0f, 5.0f, 5.0f};

	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 2.0f);

	// カメラの生成
	camera_.farZ = 1000.0f;
	camera_.Initialize();

	textureHandleTitle_ = TextureManager::Load("titleFont/HIRO Action.png");
	textureHandlePush_ = TextureManager::Load("titleFont/titleFont2.png");

	// タイトルロゴ (画面中央上部)
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {640.0f, 200.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});

	// PUSH Space テキスト (画面下部)
	spritePush_ = Sprite::Create(textureHandlePush_, {640.0f, 550.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f});
}

void TitleScene::Update() {

	ChangePhase();

	wtu_.WorldTransformUpdate(worldTransformTitle_);
	wtu_.WorldTransformUpdate(worldTransformPlayer_);

	// camera_.TransferMatrix();
	camera_.UpdateMatrix();
}

void TitleScene::Draw() {
	Model::PreDraw();
	if (fontModelA_)
		fontModelA_->Draw(worldTransformTitle_, camera_);
	if (playerModel_)
		playerModel_->Draw(worldTransformPlayer_, camera_);
	Model::PostDraw();

	// 2Dスプライトの描画
	Sprite::PreDraw();
	if (spriteTitle_) {
		spriteTitle_->Draw();
	}
	if (spritePush_) {
		spritePush_->Draw();
	}
	Sprite::PostDraw();

	fade_->Draw();
}
