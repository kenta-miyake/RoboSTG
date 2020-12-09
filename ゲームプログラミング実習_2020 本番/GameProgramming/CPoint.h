#ifndef CPOINT_H
#define CPOINT_H
#include"CCollider.h"
#include"CCharacter.h"
#include"CXCharacter.h"
/*
ポイントクラス
敵機などを目標にする
*/
class CPoint :public CCharacter{
public:
	CCollider mCollider;//コライダ追加
	CPoint();
	static CPoint*mpPoint;
	//設定
	//Set(位置、半径)
	void Set(const CVector&pos, float r);
};
#endif