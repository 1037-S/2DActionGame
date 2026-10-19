#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "AABBCo.h"
#include "Player.h"
#include "Door.h"
#include "PlayerBullet.h"
#include "Ladder.h"
#include "Enemy.h"
#include "ShieldEnemy.h"
#include "Skydome.h"
#include <vector>
#include "Fade.h"
#include "HitEffect.h"
#include "GuardEffect.h"
#include "Signboard.h"
#include "WM4.h"
#include "StageManager.h"
#include "PauseScene.h"

using namespace KamataEngine;

//ゲームシーン
class GameScene {
public:
enum class Phase
{
	kFadeIn,	// フェードイン
	kPlay,		// ゲームプレイ
	kDeash,		// デス演出
	kFadeOut,	// フェードアウト
};
//	GameScene();

	~GameScene();

	// エフェクトを生成
	void CreateEffect(Vector3& position);

	// ガードエフェクトを生成
	void CreateGuardEffect(Vector3& position);

	// すべての当たり判定
	void CheckAllCollitions();

	//表示ブロック
	void GenerateFieldObjects();

	// プレイフェーズの処理
	void PlayPhase();

	// プレイヤーの残機を取得
	int32_t GetPlayerLife() const {
		return player_ ? player_->GetLife() : 3;
	}

	// プレイヤーの残機を設定
	void SetPlayerLife(int32_t life) {
		if (player_) {
			player_->SetLife(life);
		}
	}

	// デス演出フェーズの処理
	void DeashPhase();

	// フェーズ切り替え
	void ChangePhase();

	//初期化
	void Initialize(StageManager* stageManager);

	//更新
	void Update();

	//描画
	void Draw();

	// デスフラグのヘッダー
	bool IsFinshed() const { return finished_; }

	// リロード要求フラグのゲッター
	bool GetReloadRequest() { return reloadRequest_; }

	// タイトルへ戻るリクエスト用のゲッター
	bool GetReturnToTitleRequest() const { return returnToTitleRequest_; }

private:

	
	Fade* fade_ = nullptr;

	AABBCo aabb_;
	// ワールド変換
	Rtt* rtt_ = nullptr;
	MM* mm_ = nullptr;
	WM4* wm4_ = nullptr;
	M4* m4_ = nullptr;

	WTU* wtu_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	// カメラ
	CamaraController* PlayerCamera_ = nullptr;
	CamaraController* EnemyCamera_ = nullptr;
	Camera camera_ ;
#ifdef _DEBUG
	// デバッグカメラを有効にする

	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;


#endif // _DEBUG

	// 現在のフェーズ
	Phase phase_;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	uint32_t blockTextureHandle_ = 0;

	// 天球3Dモデル
	Model* modelSkydome_ = nullptr;
	// プレイヤー3Dモデル
	Model* playerModel_ = nullptr;
	Model* playerModelAttack_ = nullptr;
	// デスパーティクル
	DeathParticles* deathParticles_ = nullptr;
	Model* deathParticleModels_ = nullptr;
	// 敵3Dモデル
	Model* enemyModel_ = nullptr;
	Model* shieldEnemyModel_ = nullptr;

	// ブロック3Dモデル
	Model* blockModel_ = nullptr;
	// 天球
	Skydome* skydome_ = nullptr;
	// 自キャラ
	Player* player_ = nullptr;
	Vector3 playerInitialPosition_;

	std::list<PlayerBullet*> playerBullets_;
	Model* bulletModel_ = nullptr;

	// 敵キャラ
	Enemy* enemy_ = nullptr;
	std::list<Enemy*>enemis_;

	// ステージマネージャ参照用のポインタ
	StageManager* stageManager_ = nullptr;

	ShieldEnemy* shieldEnemy_ = nullptr;
	std::list<ShieldEnemy*>shieldEnemis_;

	
	// ステージクリア
	Model* goalBlockModel_ = nullptr; // クリアブロック用モデル
	std::vector<WorldTransform*> goalBlocks_; // ゴールブロックのトランスフォーム配列

	bool isStageClear_ = false; // クリア判定用フラグ

	// ヒットエフェクト
	HitEffect* hitEffect_ = nullptr;
	Model* hitEffectModel_ = nullptr;
	std::list<HitEffect*>hitEffects_;
	
	// ガードエフェクト
	GuardEffect* guardEffect_ = nullptr;
	Model* guardEffectModel_ = nullptr;
	std::list<GuardEffect*>guardEffects_;


	Signboard* signboard_ = nullptr;
	std::vector<Signboard*>signboards_;
	Model* signboardModel_ = nullptr;

	uint32_t numberTextureHandle_ = 0;
	Sprite* numberSprite_ = nullptr;

	uint32_t numberTextureHandles_[10] = {};
	Sprite* numberSprites_[10] = {};

	//Model* doorModel_ = nullptr;
	//std::vector<Door*> doors_;
	//int32_t nextStageIndex_ = -1;
	
	// 梯子ギミック
	Model* ladderModel_ = nullptr;
	std::list<Ladder*> ladders_;

	// ブロック用
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// ポーズ画面
	bool isPaused_ = false;
	PauseScene* pauseScene_ = nullptr;
	bool returnToTitleRequest_ = false;

	// 終了フラグ
	bool finished_ = false;

	// リロード要求フラグ
	bool reloadRequest_ = false;

	// 残機減少時のリスポーン用フェードアウト判定フラグ
	bool isRespawnFadeOut_ = false; 
};