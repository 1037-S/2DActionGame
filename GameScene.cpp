#include "GameScene.h"
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

GameScene::~GameScene() {

	delete fade_;
	// 数学の解放
	delete rtt_;
	delete mm_;
	delete wm4_;
	delete m4_;
	delete wtu_;
	// 天球の解放
	delete skydome_;
	delete modelSkydome_;

	delete hitEffect_;
	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect;
	}
	// delete skydome_;
	//  マップチップフィールド
	delete mapChipField_;
	// 自キャラの解放
	delete player_;
	// 敵キャラの解放
	// delete enemy_;
	for (Enemy* enemy : enemis_) {
		delete enemy;
	}
	// delete shieldEnemy_;
	for (ShieldEnemy* shieldEnemy : shieldEnemis_) {
		delete shieldEnemy;
	}

	// 看板
	delete signboardModel_;
	for (Signboard* signboard : signboards_) {
		delete signboard;
	}
	signboards_.clear();

	// ゴールブロックのメモリ解放
	delete goalBlockModel_;
	for (WorldTransform* goalTransform : goalBlocks_) {
		delete goalTransform;
	}
	goalBlocks_.clear();

	// ブロックの解放
	delete blockModel_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	// 梯子の開放
	for (Ladder* ladder : ladders_) {
		delete ladder;
	}
	ladders_.clear();
	delete ladderModel_;

	// delete doorModel_;
	// for (Door* door : doors_) {
	//	delete door;
	// }
	// doors_.clear();
	worldTransformBlocks_.clear();

	// カメラの解放
	delete playerModel_;
	delete enemyModel_;
#ifdef _DEBUG
	delete debugCamera_;
#endif // _DEBUG

	delete deathParticles_;

	delete pauseScene_;

	for (int i = 0; i < 10; ++i) {
		delete numberSprites_[i];
		numberSprites_[i] = nullptr;
	}

	for (PlayerBullet* bullet : playerBullets_) {
		delete bullet;
	}
	playerBullets_.clear();
	delete bulletModel_;
}

void GameScene::CreateEffect(Vector3& position) {
	HitEffect* newHitEffect = HitEffect::Create(position);
	hitEffects_.push_back(newHitEffect);
}

void GameScene::CreateGuardEffect(Vector3& position) {
	GuardEffect* newGuardEffect = GuardEffect::Create(position);
	guardEffects_.push_back(newGuardEffect);
}

void GameScene::CheckAllCollitions() {
#pragma region プレイヤーと敵の当たり判定
	{
		AABB aabb1; // プレイヤー(プレイヤーと敵の当たり判定)
		AABB aabb2; // 敵キャラ(プレイヤーと敵の当たり判定)
		AABB aabb3; // シールド敵(プレイヤーとシールド敵の当たり判定)

		// プレイヤーキャラの座標
		aabb1 = player_->GetAABB();

		// プレイヤーと敵弾すべての当たり判定
		for (Enemy* enemy : enemis_) {
			if (enemy->IsCollisionDidabled())
				continue;

			// 敵弾の座標
			aabb2 = enemy->GetAABB();
			// AABB同士の交差判定
			if (aabb_.IsCollision(aabb1, aabb2)) {
				// プレイヤーの衝突時コールバックを呼び出す
				player_->OnCollision(enemy);
				// 敵弾の衝突時コールバックを呼び出す
				enemy->OnCollision(player_);
			}
		}

		for (ShieldEnemy* shieldEnemy : shieldEnemis_) {
			if (shieldEnemy->IsCollisionDidabled())
				continue;

			aabb3 = shieldEnemy->GetAABB();
			if (aabb_.IsCollision(aabb1, aabb3)) {
				// ※ Playerクラスに ShieldEnemy 用の OnCollision オーバーロードが
				player_->OnCollision(shieldEnemy);
				shieldEnemy->OnCollision(player_);
			}
		}
	}
#pragma endregion

#pragma region プレイヤーとゴールブロックの当たり判定
	if (player_ && !isStageClear_) {
		AABB playerAABB = player_->GetAABB();

		for (WorldTransform* goalTransform : goalBlocks_) {
			// ゴールブロックの AABB を作成
			AABB goalAABB;
			Vector3 pos = goalTransform->translation_;
			goalAABB.min = {pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f};
			goalAABB.max = {pos.x + 0.5f, pos.y + 0.5f, pos.z + 0.5f};

			if (aabb_.IsCollision(playerAABB, goalAABB)) {
				isStageClear_ = true;

				// フェードアウトなどを開始して次のステージへ移行する処理
				fade_->Start(Fade::Status::FadeOut, 1.5f);
				phase_ = Phase::kFadeOut;
				break;
			}
		}
	}
#pragma endregion

#pragma region プレイヤーの弾と敵の当たり判定
	for (PlayerBullet* bullet : playerBullets_) {
		if (bullet->IsDead())
			continue;

		AABB bulletAABB = bullet->GetAABB();

		for (Enemy* enemy : enemis_) {
			if (enemy->IsCollisionDidabled())
				continue;

			if (aabb_.IsCollision(bulletAABB, enemy->GetAABB())) {
				bullet->OnCollision();
				enemy->OnCollision(bullet);
			}
		}
	}
#pragma endregion

#pragma region 弾と（敵・ブロック）の当たり判定
	for (PlayerBullet* bullet : playerBullets_) {
		if (bullet->IsDead())
			continue;

		AABB bulletAABB = bullet->GetAABB();

		// --- 1. 弾と通常敵の判定 ---
		for (Enemy* enemy : enemis_) {
			if (enemy->IsCollisionDidabled() || enemy->GetIsDead())
				continue;

			if (aabb_.IsCollision(bulletAABB, enemy->GetAABB())) {
				bullet->OnCollision();      // 弾が消える
				enemy->OnCollision(bullet); // 敵が倒れる
				break;                      // 1発の弾で複数処理されるのを防ぐ
			}
		}

		if (bullet->IsDead())
			continue;

		// --- 2. 弾とシールド敵の判定 ---
		for (ShieldEnemy* shieldEnemy : shieldEnemis_) {
			if (shieldEnemy->IsCollisionDidabled() || shieldEnemy->GetIsDead())
				continue;

			if (aabb_.IsCollision(bulletAABB, shieldEnemy->GetAABB())) {
				bullet->OnCollision(); // 弾が消える
				shieldEnemy->OnCollision(bullet);
				break;
			}
		}

		if (bullet->IsDead())
			continue;

		// --- 3. 弾とブロック（地形）の判定 ---
		uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
		uint32_t kNumBlockHorizontal = mapChipField_->GetkNumBlockHorizontal();

		for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				// ブロックが存在するマスのみ判定
				if (mapChipField_->GetMapchipTypeByIndex(j, i) == MapchipType::kBlock) {

					Vector3 blockPos = mapChipField_->GetMapchipPostionByIndex(j, i);

					// ブロックのAABBを作成（1ブロックのサイズ: 2.0f）
					AABB blockAABB;
					blockAABB.min = {blockPos.x - 0.5f, blockPos.y - 0.5f, blockPos.z - 0.5f};
					blockAABB.max = {blockPos.x + 0.5f, blockPos.y + 0.5f, blockPos.z + 0.5f};

					if (aabb_.IsCollision(bulletAABB, blockAABB)) {
						bullet->OnCollision(); // ブロックに当たって消える
						break;
					}
				}
			}
			if (bullet->IsDead())
				break;
		}
	}
#pragma endregion

#pragma region プレイヤーと梯子の当たり判定
	if (player_) {
		AABB playerAABB = player_->GetAABB();
		bool isTouching = false;
		Ladder* touchedLadder = nullptr;

		for (Ladder* ladder : ladders_) {
			if (aabb_.IsCollision(playerAABB, ladder->GetAABB())) {
				isTouching = true;
				touchedLadder = ladder;
				break;
			}
		}
		player_->SetTouchingLadder(isTouching, touchedLadder);
	}
#pragma endregion
}

void GameScene::GenerateFieldObjects() {
	// ブロック
	// 要素数
	uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t kNumBlockHorizontal = mapChipField_->GetkNumBlockHorizontal();
	// 一個分の幅
	// const float kBlockWidth = 2.0f;
	// const float kBlockHeight = 2.0f;
	// 要素数の変更(縦方向のブロック数)
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			//if (i == 11 && j == 12) {
			//	int aaa = 10;
			//	(void)aaa;
			//
			//}
			MapchipType mapchipType = mapChipField_->GetMapchipTypeByIndex(j, i);

			switch (mapchipType) {
			case MapchipType::kBlock: {
				// uint8_t subID = mapChipField_->GetMapchipSubIDByIndex(j, i);
				// switch (subID)
				//{
				// case 0:
				//	break;
				// case 1:
				//	break;
				// }

				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;

				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapchipPostionByIndex(j, i);
			} break;
			case MapchipType::kPlayer:
#ifdef _DEBUG
				assert(player_ == nullptr && "プレイヤーを二重に配置しようとしています");
#endif // _DEBUG
       // プレイヤーの生成
       // プレイヤーの座標をマップチップ番号で指定

				{
					player_ = new Player();
					playerInitialPosition_ = mapChipField_->GetMapchipPostionByIndex(j, i);
					player_->Initialize(playerModel_, playerModelAttack_, &camera_, playerInitialPosition_);

					player_->SetMapChipField(mapChipField_);
					deathParticles_ = new DeathParticles;
					deathParticles_->Initialize(deathParticleModels_, &camera_, playerInitialPosition_);
				}
				break;
			case MapchipType::kGoal: {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = mapChipField_->GetMapchipPostionByIndex(j, i);

				// ゴールブロックリストに保持
				goalBlocks_.push_back(worldTransform);
				break;
			}

			case MapchipType::kSignboard: {
				Signboard* newSignboard = new Signboard();
				Vector3 signboardPos = mapChipField_->GetMapchipPostionByIndex(j, i);

				signboardPos.z += 0.5f;

				std::string message = "";

				// サブIDに応じたメッセージを設定
				int8_t subID = mapChipField_->GetMapchipSubIDByIndex(j, i);
				std::string texturePath = "UI/signboard_msg_" + std::to_string(subID) + ".png";
				uint32_t textTextureHandle = TextureManager::Load(texturePath);

				newSignboard->Initialize(signboardModel_, &camera_, signboardPos, textTextureHandle);
				signboards_.push_back(newSignboard);
				break;
			}

			case MapchipType::kLadder: {
				//uint8_t subID = mapChipField_->GetMapchipSubIDByIndex(j, i);
				Ladder* newladder = new Ladder();
				Vector3 ladderPos = mapChipField_->GetMapchipPostionByIndex(j, i);
					newladder->Initialize(ladderModel_, &camera_, ladderPos);
					ladders_.push_back(newladder);
				//switch (subID) {

				//case 0: {
				//	break;
				//}
				//default:
				//	break;
				//}
				    break;
			} 

			// case MapchipType::kDoor:
			//{
			//	Door* newDoor = new Door();
			//	Vector3 doorPos = mapChipField_->GetMapchipPostionByIndex(j, i);
			//	uint8_t subID = mapChipField_->GetMapchipSubIDByIndex(j, i); // サブIDを移動先のステージ番号にする

			//	newDoor->Initialize(doorModel_, doorPos, subID);
			//	doors_.push_back(newDoor);
			//	break;
			//}
			case MapchipType::kEnemy: {
				uint8_t subID = mapChipField_->GetMapchipSubIDByIndex(j, i);
				switch (subID) {
				case 0: {
					// 通常敵の生成
					Enemy* NewEnemy = new Enemy();
					Vector3 enemyPos = mapChipField_->GetMapchipPostionByIndex(j, i);
					NewEnemy->Initialize(enemyModel_, &camera_, enemyPos);
					NewEnemy->SetGameScene(this);

					enemis_.push_back(NewEnemy);
					break;
				}
				case 1: {
					// シールド敵の生成
					ShieldEnemy* NewShieldEnemy = new ShieldEnemy();
					Vector3 shieldEnemyPos = mapChipField_->GetMapchipPostionByIndex(j, i);
					NewShieldEnemy->Initialize(shieldEnemyModel_, &camera_, shieldEnemyPos);
					NewShieldEnemy->SetGameScene(this);

					shieldEnemis_.push_back(NewShieldEnemy);
					break;
				}
				default:
					break;
				}
			} break;

			case MapchipType::kBlank:
			default:
				break;
			}
		}
	}
}

void GameScene::PlayPhase() {
	skydome_->Update();
	// 梯子の更新
	for (Ladder* ladder : ladders_) {
		ladder->Update();
	}
	player_->Update();

	if (player_->IsShotRequested()) {
		player_->ResetShotRequest();

		// 弾の初速（プレイヤーの向きに応じて飛ばす）
		Vector3 bulletVelocity = {0.5f, 0.0f, 0.0f};
		if (player_->GetLRDirection() == LRDirection::kLeft) {
			bulletVelocity.x = -0.5f;
		}

		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(bulletModel_, &camera_, player_->GetWorldPosition(), bulletVelocity);
		playerBullets_.push_back(newBullet);
	}

	playerBullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Update();
	}

	for (Enemy* enemy : enemis_) {
		enemy->Update();
	}
	// shieldEnemy_->Update();
	for (ShieldEnemy* shieldEnemy : shieldEnemis_) {
		shieldEnemy->Update();
	}
	PlayerCamera_->Update();
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_M)) {
		isDebugCameraActive_ = true;
	}
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}
#endif // _DEBUG
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			rtt_->SetRotate(worldTransformBlock->rotation_);
			rtt_->Update();
			// アフィン変換行列
			Matrix4x4 scale = mm_->MakeScaleMatrix(worldTransformBlock->scale_);
			Matrix4x4 rotation = rtt_->GetRotateXYZMatrix();
			Matrix4x4 translation = mm_->MakeTransLateMatrix(worldTransformBlock->translation_);

			worldTransformBlock->matWorld_ = m4_->Multiply(scale, m4_->Multiply(rotation, translation));

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}
	// ゴールブロックの行列更新
	for (WorldTransform* goalTransform : goalBlocks_) {
		if (!goalTransform)
			continue;

		rtt_->SetRotate(goalTransform->rotation_);
		rtt_->Update();

		Matrix4x4 scale = mm_->MakeScaleMatrix(goalTransform->scale_);
		Matrix4x4 rotation = rtt_->GetRotateXYZMatrix();
		Matrix4x4 translation = mm_->MakeTransLateMatrix(goalTransform->translation_);

		goalTransform->matWorld_ = m4_->Multiply(scale, m4_->Multiply(rotation, translation));
		goalTransform->TransferMatrix();
	}
	// すべての当たり判定
	CheckAllCollitions();

	if (player_->GetIsDead()) {
		// デス演出フェーズに切り替える
		phase_ = Phase::kDeash;
		// プレイヤーキャラの座標を取得する
		const Vector3& deathParticlesPosition = player_->GetWorldPosition();

		deathParticles_->Initialize(deathParticleModels_, &camera_, deathParticlesPosition);
	}
}

void GameScene::DeashPhase() {
	skydome_->Update();
	for (Enemy* enemy : enemis_) {
		enemy->Update();
	}
	for (ShieldEnemy* shieldEnemy : shieldEnemis_) {
		shieldEnemy->Update();
	}
	if (deathParticles_ != nullptr) {
		deathParticles_->Update();
	}
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}
#endif // _DEBUG
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			rtt_->SetRotate(worldTransformBlock->rotation_);
			rtt_->Update();
			// アフィン変換行列
			Matrix4x4 scale = mm_->MakeScaleMatrix(worldTransformBlock->scale_);
			Matrix4x4 rotation = rtt_->GetRotateXYZMatrix();
			Matrix4x4 translation = mm_->MakeTransLateMatrix(worldTransformBlock->translation_);

			worldTransformBlock->matWorld_ = m4_->Multiply(scale, m4_->Multiply(rotation, translation));

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}
	for (WorldTransform* goalTransform : goalBlocks_) {
		if (!goalTransform)
			continue;

		rtt_->SetRotate(goalTransform->rotation_);
		rtt_->Update();

		Matrix4x4 scale = mm_->MakeScaleMatrix(goalTransform->scale_);
		Matrix4x4 rotation = rtt_->GetRotateXYZMatrix();
		Matrix4x4 translation = mm_->MakeTransLateMatrix(goalTransform->translation_);

		goalTransform->matWorld_ = m4_->Multiply(scale, m4_->Multiply(rotation, translation));
		goalTransform->TransferMatrix();
	}
	if (deathParticles_ && deathParticles_->IsFinshed()) {
		// 残機を1減らす
		player_->DecreaseLife();

		// 残機が残っている場合：リスポーン処理
		if (player_->GetLife() > 0) {
			isRespawnFadeOut_ = true;                  // やり直し用のフェードアウトを開始
			fade_->Start(Fade::Status::FadeOut, 1.0f); // 1.0秒でフェードアウト（秒数は調整可能）
			phase_ = Phase::kFadeOut;
		} else {
			// 残機が 0 の場合：ゲームオーバー（フェードアウトへ移行）
			fade_->Start(Fade::Status::FadeOut, 2.0f);
			phase_ = Phase::kFadeOut;
		}
	}
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case GameScene::Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinshed()) {
			phase_ = Phase::kPlay;
		}
	case GameScene::Phase::kPlay:
		PlayPhase();
		break;
	case GameScene::Phase::kDeash:
		DeashPhase();
		break;
	case GameScene::Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinshed()) {

			if (isRespawnFadeOut_) {
				// 残機が残っている場合のやり直し（リロード要求を立てる）
				reloadRequest_ = true;
			} else if (isStageClear_) {
				// 次のステージ番号を取得して更新
				int32_t nextIndex = stageManager_->GetCurrentStageIndex() + 1;
				if (nextIndex < stageManager_->GetStageCount()) {
					stageManager_->SetCurrentStageIndex(nextIndex);
				}
				finished_ = true; // シーン終了
			} else {
				// ゲームオーバー時の処理
				finished_ = true;
			}
		}
		break;
	}
}

void GameScene::Initialize(StageManager* stageManager) {

	// 引数をメンバ変数に記録する
	stageManager_ = stageManager;

	// プレイフェーズから開始する
	phase_ = Phase::kFadeIn;

	fade_ = new Fade;
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	rtt_ = new Rtt();
	mm_ = new MM();
	wm4_ = new WM4();
	m4_ = new M4();
	wtu_ = new WTU;
	// カメラの生成
	camera_.farZ = 1000.0f;
	camera_.Initialize();
#ifdef _DEBUG

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
#endif // _DEBUG

	// 3Dモデルの生成
	playerModel_ = Model::CreateFromOBJ("player", true);
	playerModelAttack_ = Model::CreateFromOBJ("hit_effect", true);
	bulletModel_ = Model::CreateFromOBJ("bullet", true);
	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	shieldEnemyModel_ = Model::CreateFromOBJ("shieldEnemy", true);
	blockModel_ = Model::CreateFromOBJ("block", true);

	goalBlockModel_ = Model::CreateFromOBJ("goal", true);

	ladderModel_ = Model::CreateFromOBJ("ladder", true);

	modelSkydome_ = Model::CreateFromOBJ("SkyDome", true);
	deathParticleModels_ = Model::CreateFromOBJ("deathParticle", true);
	hitEffectModel_ = Model::CreateFromOBJ("particle", true);
	guardEffectModel_ = Model::CreateFromOBJ("ring", true);

	// doorModel_ = Model::CreateFromOBJ("door", true);

	// 天球の生成
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);
	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;

	// 現在のステージデータを取得
	const StageData& stageData = stageManager_->GetCurrentStageData();

	// ステージファイルパスの生成
	std::string stageFilename = "Resources/stages/" + stageData.name + ".csv";

	// ステージファイルの読み込み
	mapChipField_->LoadMapChipCsv(stageFilename);

	signboardModel_ = Model::CreateFromOBJ("signboard", true);

	// 0〜9が横に並んだ数字画像などを読み込む
	numberTextureHandle_ = TextureManager::Load("numbers/number.png");

	numberSprite_ = Sprite::Create(numberTextureHandle_, {50.0f, 50.0f});

	for (int i = 0; i < 10; ++i) {
		// ファイルパスを動的に生成 (例: "numbers/number0.png")
		std::string filePath = "numbers/number" + std::to_string(i) + ".png";

		numberTextureHandles_[i] = TextureManager::Load(filePath);
		numberSprites_[i] = Sprite::Create(numberTextureHandles_[i], {50.0f, 50.0f});
	}

	// ヒットエフェクトの生成
	hitEffect_ = new HitEffect;
	HitEffect::SetModel(hitEffectModel_);
	HitEffect::SetCamera(&camera_);

	// ガードエフェクトの生成
	guardEffect_ = new GuardEffect;
	GuardEffect::SetModel(guardEffectModel_);
	GuardEffect::SetCamera(&camera_);

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player/player.png");
	blockTextureHandle_ = TextureManager::Load("block/block.png");
	GenerateFieldObjects();

	// プレイヤー用のカメラコントローラ
	PlayerCamera_ = new CamaraController();
	PlayerCamera_->Initialize(&camera_);
	PlayerCamera_->SetTarget(player_);
	PlayerCamera_->Reset();

	PlayerCamera_->SetMovebleArea(CamaraController::Rect{0, 100, 5, 100});

	pauseScene_ = new PauseScene();
	pauseScene_->Initialize();
}

void GameScene::Update() {

	Input* input = Input::GetInstance();

	// Pキーでポーズ切替
	if (input->TriggerKey(DIK_P)) {
		isPaused_ = !isPaused_;
		if (isPaused_) {
			pauseScene_->Initialize();
		}
	}

	// ポーズ中の更新処理
	if (isPaused_) {
		pauseScene_->Update();

		if (pauseScene_->IsSelected()) {
			if (pauseScene_->GetSelectIndex() == PauseScene::MenuIndex::kContinue) {
				isPaused_ = false; // ゲーム再開
			} else if (pauseScene_->GetSelectIndex() == PauseScene::MenuIndex::kTitle) {
				isPaused_ = false; // ポーズ解除

				returnToTitleRequest_ = true;
				fade_->Start(Fade::Status::FadeOut, 1.0f);
				phase_ = Phase::kFadeOut;
			}
			pauseScene_->ResetSelected();
		}
		return; // ゲーム自体の更新をストップ
	}

	camera_.TransferMatrix();

	// 各看板の更新処理
	bool isAnySignboardReading = false;
	for (Signboard* signboard : signboards_) {
		signboard->Update(player_);
		// いずれかの看板が読まれているかチェック[cite: 37]
		if (signboard->IsReading()) {
			isAnySignboardReading = true;
		}
	}

	// 看板メッセージ表示中は、これ以降のゲーム更新処理を行わずにスキップする
	if (isAnySignboardReading) {
		return;
	}

	// #ifdef _DEBUG
	//	ImGuiManager::GetInstance()->Begin();
	//	ImGui::Begin("Scene");
	//	if (ImGui::Button("reload"))
	//	{
	//		reloadRequest_ = true;
	//	}
	//	ImGui::End();
	//	ImGuiManager::GetInstance()->End();
	//
	// #endif // _DEBUG

	enemis_.remove_if([](Enemy* enemy) {
		if (enemy->GetIsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
	shieldEnemis_.remove_if([](ShieldEnemy* shieldEnemy) {
		if (shieldEnemy->GetIsDead()) {
			delete shieldEnemy;
			return true;
		}
		return false;
	});

	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Update();
	}

	for (GuardEffect* guardEffect : guardEffects_) {
		guardEffect->Update();
	}

	ChangePhase();
}

void GameScene::Draw() {

	Model::PreDraw();
	skydome_->Draw();

	// 梯子の描画
	for (Ladder* ladder : ladders_) {
		ladder->Draw();
	}

	// 看板
	for (Signboard* signboard : signboards_) {
		signboard->Draw3D();
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			blockModel_->Draw(*worldTransformBlock, camera_);
		}
	}
	// ゴールブロックの描画
	for (WorldTransform* goalTransform : goalBlocks_) {
		if (goalTransform && goalBlockModel_) {
			goalBlockModel_->Draw(*goalTransform, camera_);
		}
	}

	// 弾の描画
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw();
	}

	if (deathParticles_ != nullptr) {
		deathParticles_->Draw();
	}
	player_->Draw();
	for (Enemy* enemy : enemis_) {
		enemy->Draw();
	}
	// shieldEnemy_->Draw();
	for (ShieldEnemy* shieldEnemy : shieldEnemis_) {
		shieldEnemy->Draw();
	}
	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Draw();
	}
	for (GuardEffect* guardEffect : guardEffects_) {
		guardEffect->Draw();
		;
	}

	Model::PostDraw();

	// 看板のメッセージ
	for (Signboard* signboard : signboards_) {
		signboard->DrawUI();
	}

	// ポーズ画面の
	if (isPaused_ && pauseScene_) {
		pauseScene_->Draw();
	}

#ifdef _DEBUG
	ImGuiManager::GetInstance()->Begin();
	// ウィンドウの位置・サイズ・背景などを固定設定
	ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.35f); // 背景を少し透明にする

	ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);

	// 残機数の表示
	// int32_t life = GetPlayerLife();
	// ImGui::Text("LIFE : %d", life);

	ImGui::End();

	ImGui::Begin("Scene");
	if (ImGui::Button("reload")) {
		reloadRequest_ = true;
	}
	ImGui::End();
	ImGuiManager::GetInstance()->End();
#endif

	Sprite::PreDraw();

	// 残機数を取得（0〜99の範囲に抑える）
	int32_t life = std::clamp(GetPlayerLife(), 0, 99);

	// 10の位と1の位を計算
	int32_t tens = life / 10;
	int32_t ones = life % 10;

	// 表示設定
	Vector2 basePosition = {50.0f, 50.0f}; // 表示開始位置（左上）
	float digitOffset = 16.0f;             // 1文字あたりの表示間隔（px）

	// --- 10の位の描画 ---
	if (numberSprites_[tens]) {
		numberSprites_[tens]->SetPosition(basePosition);
		numberSprites_[tens]->Draw();
	}

	// --- 1の位の描画 ---
	if (numberSprites_[ones]) {
		Vector2 onesPosition = {basePosition.x + digitOffset, basePosition.y};
		numberSprites_[ones]->SetPosition(onesPosition);
		numberSprites_[ones]->Draw();
	}

	Sprite::PostDraw();

	fade_->Draw();
}
