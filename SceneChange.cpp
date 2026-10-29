#include "SceneChange.h"

SceneChange::~SceneChange() { 
	delete titleScene;
	delete stageSelect; 
	delete gameScene;
	delete stageManager; 
}

void SceneChange::LoadDebugSettings() {}

void SceneChange::ChangeScene() {
	switch (scene) {

	case Scene::kTitle:
		if (titleScene->IsFinshed()) {

			scene = Scene::kGeme;
			//delete stageSelect;
			//stageSelect = nullptr;

			gameScene = new GameScene();
			gameScene->Initialize(stageManager);
			// シーン変更
			// scene = Scene::kStageSelect;
			//// 旧シーンの解放
			// delete titleScene;
			// titleScene = nullptr;
			//// 新しいシーンの初期化
			// stageSelect = new StageSelect();
			// stageSelect->Initialize(stageManager);
		}

		break;
		 case Scene::kStageSelect:
			if (stageSelect->IsSelected())
			{
				scene = Scene::kGeme;
				delete stageSelect;
				stageSelect = nullptr;

				gameScene = new GameScene();
				gameScene->Initialize(stageManager);
			}
			break;

	case Scene::kGeme:
		if (gameScene->IsFinshed()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			// 新しいシーンの初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		} else if (gameScene->GetReloadRequest()) {
#ifdef _DEBUG
			// debug設定ファイルを読み込み
			LoadDebugSettings();
#endif // _DEBUG
			int32_t currentLife = gameScene->GetPlayerLife();

			// ゲームシーンのリロード
			delete gameScene;
			gameScene = nullptr;
			// 新しいシーンの初期化
			gameScene = new GameScene;
			gameScene->Initialize(stageManager);

			// 新しいシーンのプレイヤーに残機を引き継ぐ
			gameScene->SetPlayerLife(currentLife);
		}
		break;
	}
}

void SceneChange::Initialize() {

	scene = Scene::kTitle;

// ゲームシーンのインスタンス生成
	titleScene = new TitleScene();
	// ゲームシーンの初期化
	titleScene->Initialize();

	// クラスインスタンスの生成
	stageManager = new StageManager;
	// ステージデータファイルの読み込み
	stageManager->StageDataFileInput();
}

void SceneChange::Update() {
	ChangeScene();

	switch (scene) {

	case Scene::kTitle:
		titleScene->Update();

		break;
	 case Scene::kStageSelect:
		if (stageSelect) stageSelect->Update();
		break;
	case Scene::kGeme:
		gameScene->Update();

		break;
	}
}

void SceneChange::Draw() {

switch (scene) {

	case Scene::kTitle:
		titleScene->Draw();
		break;
	 case Scene::kStageSelect:
		if (stageSelect) stageSelect->Draw();
		break;
	case Scene::kGeme:
		gameScene->Draw();
		break;
	}
}
