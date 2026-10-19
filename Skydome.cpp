#include "Skydome.h"

Skydome::~Skydome() {}

void Skydome::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	// NULLポインタチェック
#ifdef _DEBUG
	assert(model);
	assert(camera);
#endif // _DEBUG
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void Skydome::Update() {}

void Skydome::Draw() {
	// if (!model_||!camera_)return;
	//  3Dモデル描画
	model_->Draw(worldTransform_, *camera_);
}
