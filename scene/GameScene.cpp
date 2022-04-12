#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

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
	//スプライト初期化
	/*sprite_ = Sprite::Create(textureHandle_, {100, 50});*/
	//モデル初期化
	model_ = Model::Create();
	// x,y,z方向のスケーリングを設定
	worldTransform_.scale_ = {5.0f, 5.0f, 5.0f};
	// x,y,z軸回りの回転角を設定
	worldTransform_.rotation_ = {XM_PI / 4.0f, XM_PI / 4.0f, 0.0f};
	// x,y,zの位置を設定
	worldTransform_.translation_ = {10.0f, 10.0f, 10.0f};
	//ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	//ビュープロジェクション初期化
	viewProjection_.Initialize();
	////サウンドデータの読み込み
	// soundDataHandle_ = audio_->LoadWave("fanfare.wav");
	////サウンド再生
	// audio_->PlayWave(soundDataHandle_,true);
}

void GameScene::Update() {
	////スプライトの今の座標を取得
	// XMFLOAT2 position = sprite_->GetPosition();
	////座標を{2,0}移動
	// position = {position.x + 2.0f, position.y + 1.0f};
	// sprite_->SetPosition(position);

	////スペースキーを押した瞬間
	// if (input_->TriggerKey(DIK_SPACE))
	//{
	//	//音声停止
	//	audio_->StopWave(soundDataHandle_);
	// }

	//デバックテキストの表示
	/*debugText_->Print("kaizokuou ni oreha naru.", 50, 50, 1.0f);*/

	//書式指定付き表示
	/*debugText_->SetPos(50, 70);
	debugText_->Printf("year%d", 2001);*/

	////変数の値をインクリメント
	// value_++;
	////値を含んだ文字列
	// std::string strDebug = std::string("Value:") + std::to_string(value_);
	////デバッグテキストの表示
	// debugText_->Print(strDebug, 50, 50, 1.0f);

	// x,y,z変換表示
	debugText_->SetPos(50, 0);
	debugText_->Printf(
	  "scale %f,%f,%f\n", worldTransform_.scale_.x, worldTransform_.scale_.y,
	  worldTransform_.scale_.z);
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "rotation %f,%f,%f", worldTransform_.rotation_.x, worldTransform_.rotation_.y,
	  worldTransform_.rotation_.z);
	debugText_->SetPos(50, 100);
	debugText_->Printf(
	  "translation %f,%f,%f", worldTransform_.translation_.x, worldTransform_.translation_.y,
	  worldTransform_.translation_.z);
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
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

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