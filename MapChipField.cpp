#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>

using namespace KamataEngine;

namespace {
std::map<char, MapchipType> mapChipTypeTable = {
    {'B', MapchipType::kBlock    },
    {'P', MapchipType::kPlayer   },
    {'G', MapchipType::kGoal     },
    {'T', MapchipType::kSignboard},
    {'L', MapchipType::kLadder   },
    {'D', MapchipType::kDoor     },
    {'E', MapchipType::kEnemy    },
};
}

MapChipField::~MapChipField() {}

void MapChipField::ResetMapChipData() {
	// マップチップデータをリセットする
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical_);
	for (std::vector<MapchipDataUnit>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal_);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// マップチップデータをリセットする
	ResetMapChipData();

	// ファイルを開く
	std::ifstream file;
	file.open(filePath);
#ifdef _DEBUG
	assert(file.is_open());
#endif // _DEBUG

	// マップチップCSV
	std::stringstream mapChipCsv;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	// ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical_; ++i) {
		std::string line;
		getline(mapChipCsv, line);

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal_; ++j) {
			std::string word;
			std::getline(lineStream, word, ',');

			// 空白の場合はスキップする
			if (word.empty()) {
				continue;
			}

			// 先頭文字がいずれかのマップチップ種別に該当数か確認
			if (!mapChipTypeTable.contains(word[kChipType])) {
				continue;
			}

			// 先頭文字でマップチップのタイプを判別
			mapChipData_.data[i][j].type = mapChipTypeTable[word[kChipType]];

			// サブIDを含まない場合はスキップする(0番で固定)
			if (word.size() <= kChipSubID) {
				mapChipData_.data[i][j].subID = 0;
				continue;
			}

			// サブIDを設定
			mapChipData_.data[i][j].subID = static_cast<uint8_t>(word[kChipSubID] - '0');
		}
	}
}

IndexSet MapChipField::GetMapchipIndexSetByPosition(const Vector3& position) {
	IndexSet indexSet = {};

	indexSet.xIndex = static_cast<uint32_t>((position.x + kBloakWidth_ / 2) / kBloakWidth_);
	uint32_t inversYIndex = static_cast<uint32_t>((position.y + kBloakHeight_ / 2) / kBloakHeight_);
	indexSet.yIndex = kNumBlockVirtical_ - 1 - inversYIndex;
	;
	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 指定ブロックの中心座標を取得する
	Vector3 center = GetMapchipPostionByIndex(xIndex, yIndex);
	Rect rect;
	rect.left = center.x - kBloakWidth_ / 2.0f;
	rect.right = center.x + kBloakWidth_ / 2.0f;
	rect.bottom = center.y - kBloakHeight_ / 2.0f;
	rect.top = center.y + kBloakHeight_ / 2.0f;

	return rect;
}

MapchipType MapChipField::GetMapchipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal_ - 1 < xIndex) {
		return MapchipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical_ - 1 < yIndex) {
		return MapchipType::kBlock;
	}

	return mapChipData_.data[yIndex][xIndex].type;
}

Vector3 MapChipField::GetMapchipPostionByIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBloakWidth_ * xIndex, kBloakHeight_ * (kNumBlockVirtical_ - 1 - yIndex), 0); }

uint8_t MapChipField::GetMapchipSubIDByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal_ - 1 < xIndex) {
		return 0;
	}
	if (yIndex < 0 || kNumBlockVirtical_ - 1 < yIndex) {
		return 0;
	}

	return mapChipData_.data[yIndex][xIndex].subID;
}
