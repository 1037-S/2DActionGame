#define NOMINMAX

#include "DeathParticles.h"

using namespace KamataEngine;

void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// rtt_ = new Rtt();
	// mm_ = new MM();
	// wm4_ = new WM4();
	// m4_ = new M4();
	//  NULLポインタチェック
#ifdef _DEBUG
	assert(model);
	assert(camera);
#endif // _DEBUG

	deathParticlesModel_ = model;
	camera_ = camera;

	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	// ワールド変換の初期化
	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
}

void DeathParticles::Update() {
	for (uint32_t i = 0; i < 8; ++i) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kspeed, 0, 0};
		// 回転角を計算
		float angle = kAngleUnit * i;
		// Z軸まわり回転行列
		Matrix4x4 matrixRotation = rtt_.MakeRotateZMatrix(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = mm_.Transform(velocity, matrixRotation);
		// 移動処理
		worldTransforms_[i].translation_.x += velocity.x;
		worldTransforms_[i].translation_.y += velocity.y;
		worldTransforms_[i].translation_.z += velocity.z;
	}

	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 存続時間の上限に達した場合
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		color_.w = 0.0f;
		// 終了扱いにする
		isFinished_ = true;
	}

	// 終了した場合何もしない
	if (isFinished_) {
		return;
	}

	color_.w = std::clamp(1.0f - (counter_ / kDuration), 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	// color_ = { 1.0f,1.0f,1.0f,0.0f };
	objectColor_.SetColor(color_);

	// ワールド変換の更新
	for (WorldTransform& worldTransform : worldTransforms_) {
		wtu_.WorldTransformUpdate(worldTransform);
	}
}

void DeathParticles::Draw() {
	// 終了した場合何もしない
	if (isFinished_) {
		return;
	}
	// 3Dモデルを描画する
	for (WorldTransform& worldTransform : worldTransforms_) {
		deathParticlesModel_->Draw(worldTransform, *camera_, &objectColor_);
	}
}
