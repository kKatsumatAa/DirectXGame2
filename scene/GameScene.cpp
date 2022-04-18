#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	/*delete sprite_;*/
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	textureHandle_ = TextureManager::Load("kasuga.png");

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲（回転角用）
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲（座標用）
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);


	//モデル初期化
	model_ = Model::Create();

	for (size_t i = 0; i < _countof(worldTransform_); i++) {

		// x,y,z方向のスケーリングを設定
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		// x,y,z軸回りの回転角を設定
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
		// x,y,zの位置を設定
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};
		//ワールドトランスフォーム初期化
		worldTransform_[i].Initialize();
	}

	////カメラ垂直方向視野角を設定
	//viewProjection_.fovAngleY = XMConvertToRadians(10.0f);

	//アスペクト比を設定
	viewProjection_.aspectRatio = 1.0f;

	//ニアクリップ距離を設定
	viewProjection_.nearZ = 52.0f;

	//ファークリップ距離を設定
	viewProjection_.farZ = 100.0f;

	viewProjection_.eye = {0, 20, 30.0f};

	//ビュープロジェクション初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
	
	//視点の移動ベクトル
	XMFLOAT3 move1 = {0, 0, 0};
	//視点の移動速さ
	const float kEyeSpeed = 0.2f;
	
	//注視点の移動ベクトル
	XMFLOAT3 move = {0, 0, 0};
	//注視点の移動速さ
	const float kTargetSpeed = 0.2f;

	//上方向の回転移動速さ[ラジアン/frame]
	const float kUpRotSpeed = 0.05f;

	//押した方向で移動ベクトルを変更
	/*if (input_->PushKey(DIK_W)) {
		move1 = {0, 0, kEyeSpeed};
	} else if (input_->PushKey(DIK_S)) {
		move1 = {0, 0, -kEyeSpeed};
	}*/
	if (input_->PushKey(DIK_LEFT)) {
		move = {-kTargetSpeed, 0, 0};
	} else if (input_->PushKey(DIK_RIGHT)) {
		move = {kTargetSpeed, 0, 0};
	}
	if (input_->PushKey(DIK_SPACE)) {
		viewAngle += kUpRotSpeed;
		//2π超えたら0に戻す
		viewAngle = fmodf(viewAngle, XM_2PI);
	}
	////視点移動（ベクトルの加算）
	//viewProjection_.eye.x += move1.x;
	//viewProjection_.eye.y += move1.y;
	//viewProjection_.eye.z += move1.z;
	////視点移動（ベクトルの加算）
	//viewProjection_.target.x += move.x;
	//viewProjection_.target.y += move.y;
	//viewProjection_.target.z += move.z;
	////上方向ベクトルを計算（半径1の円周上の座標）
	//viewProjection_.up = {cosf(viewAngle), 
	//	sinf(viewAngle), 0.0f};
	
	////デバック用表示
	//debugText_->SetPos(50, 50);
	//debugText_->Printf(
	//  "eye: %f,%f,%f\n", viewProjection_.eye.x, 
	//	viewProjection_.eye.y, viewProjection_.eye.z);
	//debugText_->SetPos(50, 70);
	//debugText_->Printf(
	//  "target: %f,%f,%f\n", viewProjection_.target.x,
	//	viewProjection_.target.y, viewProjection_.target.z);
	//debugText_->SetPos(50, 90);
	//debugText_->Printf(
	//  "up: %f,%f,%f\n", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
	

	//fov変更処理
	//上キーで視野角が広がる
	if (input_->PushKey(DIK_W)) {
		viewProjection_.fovAngleY += 0.01f;
		viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
	}
	if (input_->PushKey(DIK_S)) {
		viewProjection_.fovAngleY -= 0.01f;
		viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
	}

	debugText_->SetPos(50, 110);
	debugText_->Printf(
	  "fovAngleY(Degree): %f\n", XMConvertToDegrees(viewProjection_.fovAngleY));

	//上下キーでニアクリップ距離を増減
	if (input_->PushKey(DIK_UP)) {
		viewProjection_.nearZ += 0.1f;
	}
	if (input_->PushKey(DIK_DOWN)) {
		viewProjection_.nearZ -= 0.1f;
	}

	//行列の再計算
	viewProjection_.UpdateMatrix();

	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ: %f\n", viewProjection_.nearZ);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	/*sprite_->Draw();*/

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}