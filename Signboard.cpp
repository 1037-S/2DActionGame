#include "Signboard.h"
#include "Player.h"

using namespace KamataEngine;

void Signboard::Initialize(Model* model, Camera* camera, const Vector3& position, uint32_t textTextureHandle) {
	model_ = model;
	camera_ = camera;
	textTextureHandle_ = textTextureHandle;

	wtu_.WorldTransformInitialize(worldTransform_);
	worldTransform_.translation_ = position;
	wtu_.WorldTransformUpdate(worldTransform_);

	// UI用スプライトの読み込み（例：操作説明画像）
	if (textTextureHandle_ != 0) {
		textSprite_ = Sprite::Create(
		    textTextureHandle_, Vector2(640.0f, 500.0f), // 画面位置 (X, Y)
		    Vector4(1.0f, 1.0f, 1.0f, 1.0f),             // 色 (RGBA)
		    Vector2(0.5f, 0.5f)                          // 基準点 (中央)
		);
	}
	// textTextureHandle_ = TextureManager::Load("UI/signboard_msg.png");
	// textSprite_ = Sprite::Create(textTextureHandle_, { 640.0f, 500.0f }, {1,1,1,1}, {0.5f, 0.5f});
}

void Signboard::Update(const Player* player) {
	if (!player)
		return;

	// プレイヤーと看板の距離を計算（XZ平面上の距離など）
	Vector3 playerPos = player->GetWorldPosition();
	Vector3 diff = {playerPos.x - worldTransform_.translation_.x, playerPos.y - worldTransform_.translation_.y, playerPos.z - worldTransform_.translation_.z};

	float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

	// 距離の2乗で判定（例: 3.0ユニット以内に近づいたら表示）
	const float kInteractDistanceSq = 3.0f * 3.0f;
	isNearPlayer_ = (distanceSq <= kInteractDistanceSq);

	// 範囲外に出たら強制的に読むことはできない
	if (!isNearPlayer_) {
		isReading_ = false;
	} else {
		// 近くにいる状態で Wキー または UPキー を押したら読む/閉じるを切り替え
		Input* input = Input::GetInstance();
		if (input->TriggerKey(DIK_W) || input->TriggerKey(DIK_UP)) {
			isReading_ = !isReading_;
		}
	}

	wtu_.WorldTransformUpdate(worldTransform_);
}

void Signboard::Draw3D() {
	if (model_ && camera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Signboard::DrawUI() {
	// 1. 近くにいる時：「Wキーで読む」プロンプトの表示
	if (isNearPlayer_ && !isReading_) {
#ifdef _DEBUG
		ImGui::SetNextWindowPos(ImVec2(560.0f, 500.0f));
		ImGui::Begin("PromptUI", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
		ImGui::Text("[ W / UP / E ] 読む");
		ImGui::End();
#endif
	}

	// 操作説明メッセージの表示
	if (isReading_ && textSprite_) {

		Sprite::PreDraw();
		textSprite_->Draw();
		Sprite::PostDraw();

#ifdef _DEBUG
		ImGui::SetNextWindowPos(ImVec2(440.0f, 220.0f));
		ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f));
		ImGui::Begin("Signboard UI", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
		ImGui::Text("=== 操作説明 ===");
		ImGui::Separator();
		ImGui::Text("A / D キー : 左右移動");
		ImGui::Text("SPACE キー : ジャンプ");
		ImGui::Text("J キー     : 攻撃");
		ImGui::Spacing();
		ImGui::TextDisabled("(もう一度 W キーを押して閉じる)");
		ImGui::End();
#endif
	}
}