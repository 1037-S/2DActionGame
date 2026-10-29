#include <Windows.h>
#include "KamataEngine.h"
#include "SceneChange.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	// エンジンの初期化
	Initialize(L"LE2C_03_イトイ_シュウト_REI ACTION");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ゲームシーンのインスタンス生成
	SceneChange* sceneChange = new SceneChange();
	
	sceneChange->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (Update()) {
			break;
		}

		// ゲームシーンの更新
		sceneChange->Update();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		sceneChange->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}
	// ゲームシーンの解放
	delete sceneChange;

	// nullptrの代入
	sceneChange = nullptr;

	// 終了処理
	Finalize();

	return 0;
}
