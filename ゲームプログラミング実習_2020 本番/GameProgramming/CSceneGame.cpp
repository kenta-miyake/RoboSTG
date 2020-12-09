#include "CSceneGame.h"
//
#include "CCamera.h"
//
#include "CText.h"

#include"CRes.h"


#include<Windows.h>


#include"CXCharacter.h"

#include"CCollisionManager.h"

CMatrix Matrix;
CModel BackGround;
CModel CSceneGame::mCube;

void CSceneGame::Init() {

	CXEnemy3::mPointSize = 3;
	CXEnemy3::mPoint = new CPoint[CXEnemy3::mPointSize];
	CXEnemy3::mPoint[0].Set(CVector(35.0f, 35.0f, 100.0f), 10.0f);
	CXEnemy3::mPoint[1].Set(CVector(-35.0f, 35.0f, 50.0f), 10.0f);
	CXEnemy3::mPoint[2].Set(CVector(35.0f, 35.0f, 0.0f), 10.0f);

	CXEnemy::mPointSize = 3;
	CXEnemy::mPoint = new CPoint[CXEnemy3::mPointSize];
	CXEnemy::mPoint[0].Set(CVector(35.0f, 0.0f, 100.0f), 10.0f);
	CXEnemy::mPoint[1].Set(CVector(-35.0f, 0.0f, 50.0f), 10.0f);
	CXEnemy::mPoint[2].Set(CVector(35.0f, 0.0f, 0.0f), 10.0f);

	mCube.Load("cube.obj", "cube.mtl");

	CRes::sModelX.Load(MODEL_FILE);
	CRes::sKnight.Load(MODEL_FILE2);
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//1:移動
	CRes::sKnight.SeparateAnimationSet(0, 1530, 1830, "idle1");//2:待機
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//3:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//4:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//5:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//6:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 440, 520, "attack1");//7:Attack1
	CRes::sKnight.SeparateAnimationSet(0, 520, 615, "attack2");//8:Attack2
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//9:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 10, 80, "walk");//10:ダミー
	CRes::sKnight.SeparateAnimationSet(0, 1160, 1260, "death1");//11:ダウン
	BackGround.Load("sky.obj", "sky.mtl");
	CEye(&mCube, CVector(), CVector(), CVector(1.0f, 1.0f, 1.0f));
	CEye2(&mCube, CVector(), CVector(), CVector(1.0f, 1.0f, 1.0f));
	//キャラクターにモデルを設定
	Player.Init(&CRes::sModelX);
	mEnemy = new CXEnemy();
	mEnemy->Init(&CRes::sKnight);
	//テキストフォントの読み込みと設定
	CText::mFont.Load("FontG.tga");
	CText::mFont.SetRowCol(1, 4096 / 64);
	//敵の初期設定
	//敵の配置]
	Player.mPosition = CVector(0.0f, 2.0f, -100.0f);
	new CXEnemy2(&mCube, CVector(100.0f, 20.0f, 100.0f), CVector(), CVector(1.5f, 1.5f, 1.5f));
	mEnemy3 = new CXEnemy3(&mCube, CVector(100.0f, 10.0f, 100.0f), CVector(), CVector(1.5f, 1.5f, 1.5f));
	mEnemy3->mPosition = (CVector(100.0f, 35.0f, 10.0f));
	mEnemy->mPosition = CVector(0.0f, 0.0f, 100.0f);
	mEnemy->mRotation = CVector(0.0f, 90.0f, 0.0f);
	mEnemy->mAnimationFrameSize = 1024;
	mEnemy->ChangeAnimation(1, true, 60);
	mMap = new CMap();
}

void CSceneGame::Update() {
	
	//キャラクタークラスの更新0
	TaskManager.Update();
	//最初のアニメーションの現在時間を45にする
	////最初のアニメーションの重みを1.0(100%)にする
	//CRes::sModelX.mAnimationSet[0]->mWeight = 1.0f;
	////フレームの変換行列をアニメーションで更新する
	//CRes::sModelX.AnimateFrame();
	////フレームの合成行列を計算する
	//CRes::sModelX.mFrame[0]->AnimateCombined(&Matrix);
	mEye.mPosition = Player.mPosition;
	mEye2.mPosition = mEnemy3->mPosition;
	//カメラのパラメータを作成する
	CVector e, c, u;//視点、注視点、上方向
	e = CVector(0.0, 5.5,  -6.0)*mEye.mMatrix;
	//注視点を求める
	c = CVector(0.0, 0.0, 30.0)*mEye.mMatrix; //Player.mPosition;
	//上方向を求める
	u = CVector(0.0, 0.5, 0.0)*mEye.mMatrixRotate;
	//カメラの設定
	Camera3D(e.mX, e.mY, e.mZ, c.mX, c.mY, c.mZ, u.mX, u.mY, u.mZ);
	////x軸＋回転
	//if (GetKeyState('K') & 0x8000){
	//	Matrix = Matrix*CMatrix().RotateX(1);
	//}
	////x軸＋回転
	//if (GetKeyState('I') & 0x8000){
	//	Matrix = Matrix*CMatrix().RotateX(-1);
	//}
	//y軸＋回転
	//if (GetKeyState('L') & 0x8000){
	//	Matrix = Matrix*CMatrix().RotateY(1);
	//}
	////y軸＋回転
	//if (GetKeyState('J') & 0x8000){
	//	Matrix = Matrix*CMatrix().RotateY(-1);
	//}
	//行列設定
	glMultMatrixf(Matrix.mF);
	//モデル描画
	BackGround.Render();
	//コライダの描画
	CollisionManager.Collision();
	TaskManager.Delete();
	TaskManager.Render();
	//  CRes::sModelX.Render();
	CollisionManager.Render();

	//テクスチャテスト
	//CRes::sModelX.mMaterial[0]->mpTexture->DrawImage
	//	(-5, 5, -5, 5, 0, 128, 128, 0);
	//2D描画開始
	Start2D(0, 800, 0, 600);


	//2D描画終了
	End2D();
	return;
}

CSceneGame::~CSceneGame() {
	delete[]CXEnemy3::mPoint;
}
