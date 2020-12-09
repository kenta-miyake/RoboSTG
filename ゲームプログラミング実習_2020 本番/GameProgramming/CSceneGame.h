#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include"CXPlayer.h"
#include"CXEnemy.h"
#include"CKey.h"
#include"CMap.h"
#include"CEye.h"
/*
ゲームのシーン
*/
class CSceneGame : public CScene {
public:
	CEye mEye;
	CEye2 mEye2;
	CXPlayer Player;
	CXEnemy*mEnemy;
	CXEnemy3 *mEnemy3;
	static CModel mCube;
	~CSceneGame();
	CSceneGame(){};
	//初期化処理のオーバーライド
	void Init();
	//更新処理のオーバーライド
	void Update();
	//キャラクタのインスタンス
	CMap*mMap;
};

#endif
