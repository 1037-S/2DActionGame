#pragma once
#include "KamataEngine.h"
#include <vector>

enum class MapchipType {
	kBlank,			// 空白
	kBlock,			// ブロック
	kPlayer,		// プレイヤー
	kSignboard,		// 看板
	kLadder,		// 梯子
	kDoor,			// ドア
	kKeyLockDoor,	// 鍵付き扉
	kKey,			// 鍵
	kSwitchDoor,	// スイッチで開く扉
	kSwitch,		// スイッチ
	kGoal,			// ステージクリア
	kEnemy,			// 敵
};

struct IndexSet {
	uint32_t xIndex;
	uint32_t yIndex;
};

struct MapchipDataUnit {
	MapchipType type; // マップチップの種別
	uint8_t subID;    // サブID
};

struct MapChipData {
	std::vector<std::vector<MapchipDataUnit>> data;
};

class MapChipField {
public:
	// 矩形範囲
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下端
		float top;    // 上端
	};

	// マップチップCSVの文字番号
	enum MapChipCharInex {
		kChipType = 0,  // マップチップタイプ
		kChipSubID = 1, // サブID
	};

	~MapChipField();

	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	IndexSet GetMapchipIndexSetByPosition(const KamataEngine::Vector3& position);
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	MapchipType GetMapchipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	KamataEngine::Vector3 GetMapchipPostionByIndex(uint32_t xIndex, uint32_t yIndex);

	uint8_t GetMapchipSubIDByIndex(uint32_t xIndex, uint32_t yIndex);
	void SetkNumBlockVirtical(uint32_t kNumBlockVirtical) { kNumBlockVirtical = kNumBlockVirtical_; }
	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical_; }
	void SetNumBlockHorizontal(uint32_t kNumBlockHorizontal) { kNumBlockHorizontal = kNumBlockHorizontal_; }
	uint32_t GetkNumBlockHorizontal() { return kNumBlockHorizontal_; }

private:
	MapChipData mapChipData_;

	MapchipType mapChipType_;

	// 1ブロックのサイズ
	static inline const float kBloakWidth_ = 1.0f;
	static inline const float kBloakHeight_ = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical_ = 20;
	static inline const uint32_t kNumBlockHorizontal_ = 500;
};
