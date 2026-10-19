#pragma once
#include <random>

class Randomize {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// float型の乱数を取得 (min ～ max の範囲)
	float GetRandomFloat(float min, float max);

private:
	// 乱数生成エンジン
	std::random_device seedGenerator_;
	// メルセンヌ・ツイスタエンジン(64bit版)
	std::mt19937_64 randomEngine;
	// 指定範囲の乱数生成器(浮動小数点数用)
	std::uniform_real_distribution<float> rotationDistribution;
};
