#pragma once
#include <cmath>
#include <string>
#include <vector>
// #include <numbers>
// #include <algorithm>
#include "KamataEngine.h"

struct StageData {
	std::string name;  // ステージの名前
	int32_t timeLimit; // 制限時間
};

class StageManager {
public:
	void StageDataFileInput();

	const StageData& GetStageData(int32_t index) const {
#ifdef _DEBUG
		assert(index >= 0 && index < static_cast<int32_t>(stageDatas_.size()) && "indexが正常な範囲外です");
#endif // _DEBUG
		return stageDatas_[index];
	}

	int32_t GetStageCount() const { return static_cast<int32_t>(stageDatas_.size()); }

	void SetCurrentStageIndex(int32_t index) {
#ifdef _DEBUG
		assert(index >= 0 && index < static_cast<int32_t>(stageDatas_.size()) && "indexが正常な範囲外です");
#endif // _DEBUG
		currentStageIndex_ = index;
	}

	int32_t GetCurrentStageIndex() const { return currentStageIndex_; }

	const StageData& GetCurrentStageData() const { return GetStageData(currentStageIndex_); }

	void SetCurrentStageIndexByName(const std::string& name);

private:
	// 全てのステージデータ
	std::vector<StageData> stageDatas_;

	// 現在のステージ番号
	int32_t currentStageIndex_ = 0;
};
