#include "Randomize.h"

void Randomize::Initialize() {
	// メルセンヌ・ツイスターエンジンの初期化
	randomEngine.seed(seedGenerator_());
}

void Randomize::Update() {}

float Randomize::GetRandomFloat(float min, float max) {
	// 指定範囲の乱数生成器(浮動小数点数用)
	rotationDistribution = std::uniform_real_distribution<float>(min, max);

	return rotationDistribution(randomEngine);
}
