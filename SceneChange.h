#pragma once
#include "GameScene.h"
#include "StageManager.h"
#include "StageSelect.h"
#include "TitleScene.h"
#include <fstream>
#include <sstream>
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

enum class Scene {
	kUnknowm = 0,

	kTitle,
	kStageSelect,
	kGeme,
};

class SceneChange {
public:
	~SceneChange();


	void LoadDebugSettings();
	void ChangeScene();
	void Initialize();
	void Update();
	void Draw();

private:
	TitleScene* titleScene = nullptr;
	StageSelect* stageSelect = nullptr;
	GameScene* gameScene = nullptr;
	StageManager* stageManager = nullptr;

	Scene scene = Scene::kUnknowm;

};
