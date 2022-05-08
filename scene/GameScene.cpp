#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>
#include <Math.h>

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
	
	// x,y,zの位置を設定
	worldTransform_[0].translation_ = {-2.0f, -2.0f, 0};
	worldTransform_[1].translation_ = {2.0f, -2.0f, 0};
	worldTransform_[2].translation_ = {0, 2.0f, 0};

	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		//ワールドトランスフォーム初期化
		worldTransform_[i].Initialize();
	}
	
	//ビュープロジェクション初期化
	viewProjection_.eye = {0, 0, -10};
	viewProjection_.Initialize();
	
}

void GameScene::Update() {
	if (input_->TriggerKey(DIK_SPACE)) targetNum++;
	if (targetNum >= 3) targetNum = 0;
	viewProjection_.target.x = worldTransform_[targetNum].translation_.x;
	viewProjection_.target.y = worldTransform_[targetNum].translation_.y;
	viewProjection_.target.z = worldTransform_[targetNum].translation_.z;
	 viewProjection_.UpdateMatrix();
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