﻿#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
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

	//world
	for (size_t i = 1; i < _countof(worldTransform_); i++) {

		// x,y,z方向のスケーリングを設定
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		// x,y,z軸回りの回転角を設定
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
		// x,y,zの位置を設定
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};
		//ワールドトランスフォーム初期化
		worldTransform_[i].Initialize();
	}
	
	viewProjection_.eye = {0, 20, -30};
	viewProjection_.target = {0, 0, 0};

	//ビュープロジェクション初期化
	viewProjection_.Initialize();

	//ワールドトランスフォーム初期化
	worldTransform_[0].Initialize();
}

void GameScene::Update() {
	//終点
	XMFLOAT3 endPoint = {0, 0, 0};
	//正面ベクトル
	XMFLOAT3 vecFront = {0, 0, 0};
	//大きさ
	const float frontLength = 10.0f;
	//正規化したベクトル
	XMFLOAT3 normalFLength = {0, 0, 0};

	//オブジェからカメラまでの距離
	XMFLOAT3 cameraVec = {0, 20, -30};

	const float cameraLength =
	  sqrtf(cameraVec.y * cameraVec.y + cameraVec.z * cameraVec.z); // xは0なので今回は省略！！

	const float frontSpeed = 0.5f;

	if (input_->PushKey(DIK_LEFT)) {
		worldTransform_[0].rotation_.y -= frontSpeed / 10;
	}if (input_->PushKey(DIK_RIGHT)) {
		worldTransform_[0].rotation_.y += frontSpeed / 10;
	}

	//正面ベクトルの終点を、オブジェクトの回転に合わせて一緒に回転
	endPoint = {
	  worldTransform_[0].translation_.x + sinf(worldTransform_[0].rotation_.y) * frontLength,
	  worldTransform_[0].translation_.y,
	  worldTransform_[0].translation_.z + cosf(worldTransform_[0].rotation_.y) * frontLength};

	//正面ベクトルの成分を計算
	vecFront = {
	  endPoint.x - worldTransform_[0].translation_.x,
	  endPoint.y - worldTransform_[0].translation_.y,
	  endPoint.z - worldTransform_[0].translation_.z};

	//成分を正規化
	normalFLength = {vecFront.x / frontLength, vecFront.y / frontLength, vecFront.z / frontLength};


	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_UP)) {
		worldTransform_[0].translation_ = {
		  worldTransform_[0].translation_.x + normalFLength.x * 0.1f,
		  worldTransform_[0].translation_.y + normalFLength.y * 0.1f,
		  worldTransform_[0].translation_.z + normalFLength.z * 0.1f};
	}if (input_->PushKey(DIK_DOWN)) {
		worldTransform_[0].translation_ = {
		  worldTransform_[0].translation_.x - normalFLength.x * 0.1f,
		  worldTransform_[0].translation_.y - normalFLength.y * 0.1f,
		  worldTransform_[0].translation_.z - normalFLength.z * 0.1f};
	}


	//カメラ追従
	viewProjection_.eye = {
	  worldTransform_[0].translation_.x - sinf(worldTransform_[0].rotation_.y) * cameraLength,
	  worldTransform_[0].translation_.y + cosf(worldTransform_[0].rotation_.x) * cameraLength,
	  worldTransform_[0].translation_.z - cosf(worldTransform_[0].rotation_.y) * cameraLength};
	//正面ベクトルとは逆方向のベクトルなのでマイナスの値にする！？

	viewProjection_.target = worldTransform_->translation_;
	
	//行列の再計算
	worldTransform_[0].UpdateMatrix();
	viewProjection_.UpdateMatrix();

	/*debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ: %f\n", viewProjection_.nearZ);*/
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