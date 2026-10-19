#include "StageManager.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

void StageManager::StageDataFileInput() {
	// ステージデータファイルのパス
	const std::string filePath = "Resources/stageDatas.csv";

	// 既存データのクリア
	stageDatas_.clear();

	// ifstreamでステージデータファイルを開く
	std::ifstream file(filePath);

#ifdef _DEBUG
	assert(file.is_open() && "ステージデータファイルが存在しません");
#endif // _DEBUG
	if (!file.is_open()) {
		return;
	}

	// ファイルの内容を格納するstringstreamの宣言
	std::stringstream stageDataStream;

	// ファイルの内容をstringstreamのコピー
	stageDataStream << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// ステージデータを最終行まで1行ずつ読み込む
	// １行分の内容を格納するstringの宣言
	std::string line;
	while (std::getline(stageDataStream, line)) {
		// １行分の内容を格納するstringstreamを宣言して、stringから変換する
		std::stringstream lineStream(line);

		// ステージデータを格納する構造体
		StageData stageData;
		// カンマ区切りの１つ分を格納するstringの宣言
		std::string token;

		// カンマ区切りで次のデータを取得する
		if (std::getline(lineStream, token, ',')) {
			// ステージ名を格納する
			stageData.name = token;
		}
		// カンマ区切りで次のデータを取得する
		if (std::getline(lineStream, token, ',')) {
			// 整数に変換して制限時間を格納する
			stageData.timeLimit = std::stoi(token);
		}
		// ステージデータテーブルに格納する
		stageDatas_.push_back(stageData);
	}
}

void StageManager::SetCurrentStageIndexByName(const std::string& name) {
	// 全ステージデータを検索
	for (size_t i = 0; i < stageDatas_.size(); ++i) {
		// ステージ名が一致したら現在ステージ番号を設定
		if (stageDatas_[i].name == name) {
			currentStageIndex_ = static_cast<int32_t>(i);
			// 目的を達成したので関数を抜ける
			return;
		}
	}
#ifdef _DEBUG
	assert(false && "指定したステージ名は存在しません");
#endif // _DEBUG
}
