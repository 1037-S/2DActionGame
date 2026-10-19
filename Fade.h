#pragma once
#include "KamataEngine.h"

class Fade {
public:
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン
		FadeOut, // フェードアウト
	};

	~Fade();

	// フェード開始
	void Start(Status status, float duration);

	// フェード停止
	void Stop();

	// フェード終了判定
	bool IsFinshed() const;

	void Initialize();
	void Update();
	void Draw();

	// 現在のフェードの状態
	Status status_ = Status::None;

private:
	KamataEngine::Sprite* sprite_ = nullptr;

	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;

	uint32_t textureHandle_;
};
