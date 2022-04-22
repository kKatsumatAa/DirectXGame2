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

	//モデル初期化
	model_ = Model::Create();

	//親
	//ワールドトランスフォーム初期化
	worldTransform_[0].Initialize();

	//子
	// x,y,zの位置を設定
	worldTransform_[1].translation_ = {0, 4.5f, 0};//ローカル座標
	worldTransform_[1].parent_ = &worldTransform_[0];
	worldTransform_[1].Initialize();

	//ビュープロジェクション初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
	
	//移動系
	XMFLOAT3 move = {0, 0, 0};

	const float kCharacyerSpeed = 0.2f;

	if (input_->PushKey(DIK_LEFT)) {
		move = {-kCharacyerSpeed, 0, 0};
	}
	else if (input_->PushKey(DIK_RIGHT)){
		move = {kCharacyerSpeed, 0, 0};
	}

	//注視点移動(ベクトルの加算)
	worldTransform_[PartId::Root].

	//行列の再計算
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