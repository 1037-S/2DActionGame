#include "StageSelect.h"
#include "StageManager.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

StageSelect::~StageSelect() {
	delete fade_;
	delete spriteCursor_;
}

void StageSelect::Initialize(StageManager* stageManager) {
	stageManager_ = stageManager;
	currentCursor_ = 0;
	isSelected_ = false;
	selectedStageIndex_ = -1;

	camera_.Initialize();

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	phase_ = Phase::kFadeIn;

	// カーソル用スプライト（画像がある場合）
	// textureHandleCursor_ = TextureManager::Load("UI/cursor.png");
	// spriteCursor_ = Sprite::Create(textureHandleCursor_, { 100.0f, 200.0f });
}

void StageSelect::Update() {
	// フェード処理
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinshed()) {
			phase_ = Phase::kMain;
		}
		break;

	case Phase::kMain: {
		Input* input = Input::GetInstance();
		if (!stageManager_)
			return;

		int maxStages = stageManager_->GetStageCount();
		if (maxStages <= 0)
			return;

		// 上移動
		if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_W)) {
			if (currentCursor_ > 0) {
				currentCursor_--;
			}
		}

		// 下移動
		if (input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_S)) {
			if (currentCursor_ < maxStages - 1) {
				currentCursor_++;
			}
		}

		// 決定（Z、Space、Enter）
		if (input->TriggerKey(DIK_SPACE) || input->TriggerKey(DIK_Z) || input->TriggerKey(DIK_RETURN)) {
			selectedStageIndex_ = currentCursor_;
			stageManager_->SetCurrentStageIndex(selectedStageIndex_);

			// フェードアウト開始
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;
	}

	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinshed()) {
			isSelected_ = true; // シーン切り替え要求を通知
		}
		break;
	}

	// カーソル位置の更新
	if (spriteCursor_) {
		float cursorY = 200.0f + (currentCursor_ * 50.0f);
		spriteCursor_->SetPosition({150.0f, cursorY});
	}
}

void StageSelect::Draw() {
#ifdef _DEBUG
	// デバッグ用表示 (ImGui)
	ImGuiManager::GetInstance()->Begin();
	ImGui::SetNextWindowPos(ImVec2(100.0f, 100.0f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(400.0f, 300.0f), ImGuiCond_Always);

	ImGui::Begin("Stage Select");
	if (stageManager_) {
		int maxStages = stageManager_->GetStageCount();
		for (int i = 0; i < maxStages; ++i) {
			const StageData& data = stageManager_->GetStageData(i);
			if (i == currentCursor_) {
				ImGui::Text("> %s <", data.name.c_str());
			} else {
				ImGui::Text("  %s  ", data.name.c_str());
			}
		}
	}
	ImGui::End();
	ImGuiManager::GetInstance()->End();
#endif

	// スプライト等の描画
	Sprite::PreDraw();
	if (spriteCursor_) {
		spriteCursor_->Draw();
	}
	Sprite::PostDraw();

	// フェードの描画
	if (fade_) {
		fade_->Draw();
	}
}
