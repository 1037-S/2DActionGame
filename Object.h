#pragma once
#include "KamataEngine.h"

struct AABB {                  // 軸平行境界箱(Axis-Allgned Bounding-Box)
	KamataEngine::Vector3 min; //!< 最小点
	KamataEngine::Vector3 max; //!< 最大点
};

// 左右
enum class LRDirection {
	kRight, // 右向き
	kLeft,  // 左向き
};