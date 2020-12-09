#ifndef CXENEMY_H
#define CXENEMY_H


#include"CPoint.h"
#include"CXCharacter.h"
#include"CCollider.h"
class CXEnemy :public CXCharacter{
public:
	int FireCount;
	CCollider mColSphereBody;  //体
	CCollider mColSphereHead;  //頭
	CCollider mColSphereSword0; //剣
	CCollider mColSphereSword1; //剣
	CCollider mColSphereSword2; //剣
	CCollider mSearch;
	void Init(CModelX*model);
	CXEnemy();
	void Collision(CCollider*m, CCollider*y);
	void Update();
	static int mPointSize;//ポイントの数
	CPoint*mpPoint;//目指すポイント
	int mPointCnt;//ポインタのカウンタ
	//誘導ポイント
	static CPoint *mPoint;
	bool mKAKUNIN;
};

class CXEnemy2 :public CCharacter{
public:
	void Update();
	CXEnemy2();
	CCollider mBall;
	int FireCount2;
	CXEnemy2(CModel*model, CVector position, CVector rotation, CVector scale);

};

class CXEnemy3 :public CCharacter{
public:
	void Update();
	CXEnemy3()
	{};
	CCollider mAir;
	CCollider mSearch2;
	int FireCount3;
	CXEnemy3(CModel*model, CVector position, CVector rotation, CVector scale);
	static int mPointSize;//ポイントの数
	CPoint*mpPoint;//目指すポイント
	int mPointCnt;//ポインタのカウンタ
	//誘導ポイント
	static CPoint *mPoint;
	void Collision(CCollider*ms, CCollider*ys);
	static CXEnemy3*mEnemy3;
	static bool Senser;
};


#endif
