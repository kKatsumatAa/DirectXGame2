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
	std::uniform_real_distribution<float> posDist(-30.0f, 30.0f);

	//モデル初期化
	model_ = Model::Create();
	
	// x,y,zの位置を設定
	worldTransform_.translation_ = {0, 0, 0};
	//ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	
	//ビュープロジェクション初期化
	for (size_t i = 0; i < _countof(viewProjection_); i++) {
		viewProjection_[i].eye = {posDist(engine), posDist(engine), posDist(engine)};
		viewProjection_[i].Initialize();
	}
}

void GameScene::Update() {
	if (input_->TriggerKey(DIK_SPACE)) {
		viewNum++;
	}
	if (viewNum >= 3) {
		viewNum = 0;
	}

	for (size_t i = 0; i < _countof(viewProjection_);i++)
	{
		debugText_->SetPos(50, 50 + 30 * i);
		debugText_->Printf(
		  "camera %d: eye%f,%f,%f target%f,%f,%f up%f,%f,%f",i+1, viewProjection_[i].eye.x,
		  viewProjection_[i].eye.y, viewProjection_[i].eye.z, viewProjection_[i].target.x,
		  viewProjection_[i].target.y, viewProjection_[i].target.z, viewProjection_[i].up.x,
		  viewProjection_[i].up.y, viewProjection_[i].up.z);
	}
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
	model_->Draw(worldTransform_, viewProjection_[viewNum], textureHandle_);
	

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