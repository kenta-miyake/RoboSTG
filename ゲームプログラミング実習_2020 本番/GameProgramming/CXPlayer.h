#ifndef CXPLAYER_H
#define CXPLAYER_H
#define G (1.0f/60.0f)
#define JUMPV0 (0.4f)
#include"CEye.h"
#include"CXCharacter.h"
#include"CCollider.h"

class CXPlayer :public CXCharacter{
public:
	float mVelovcityJump;
	CCollider mColSphereBody;  //体
	CCollider mColSphereHead;  //頭
	CCollider mColSphereSword; //剣
	CCollider mAshi1;
	CCollider mAshi2;
	bool mHyuu;
	//デフォルトコンストラクタ
	CXPlayer();
	//更新処理
	void Update();
	void Init(CModelX*model);
	void Collision(CCollider*mp, CCollider*yp);
	static CXPlayer*mPlayer;
	CXPlayer*mpPoint;//目指すポイント
	float kasoku;
	int hayasa;
	float kasokuB;
	float kasokuC;
	static int mLife;
};


#endif
