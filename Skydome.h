#pragma once
#include "KamataEngine.h"
#include "WM4.h"

class Skydome {
public:
	// Skydome();
	~Skydome();

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);
	void Update();
	void Draw();

private:
	// ワールド変換
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// モデル
	KamataEngine::Model* model_ = nullptr;
};
