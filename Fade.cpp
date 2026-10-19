#define NOMINMAX

#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

Fade::~Fade() { delete sprite_; }

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinshed() const {
	// フェード状態による分岐
	switch (status_) {
	case Fade::Status::FadeIn:
	case Fade::Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}

	return true;
}

void Fade::Initialize() {
	textureHandle_ = TextureManager::Load("white1x1.png");

	sprite_ = Sprite::Create(textureHandle_, {});
	sprite_->SetSize(Vector2(1280, 720));
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

void Fade::Update() {

	// フェード状態による分岐
	switch (status_) {
	case Status::None:
		break;
	case Status::FadeIn:

		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に経過したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0f～1.0fの間で経過時間がフェード継続時間に近づくほどアルファ値を小さくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)));

		break;
	case Status::FadeOut:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に経過したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0f～1.0fの間で経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));

		break;
	}
}

void Fade::Draw() {
	if (status_ == Status::None) {
		return;
	}

	Sprite::PreDraw();
	sprite_->Draw();
	Sprite::PostDraw();
}
