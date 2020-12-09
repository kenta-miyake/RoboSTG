#include"CPoint.h"
CPoint*CPoint::mpPoint = 0;
CPoint::CPoint()
:mCollider(this, CVector(), CVector(), CVector(1.0f, 1.0f, 1.0f), 1.0f)
{
	mTag = EPOINT;//タグをポイントにする
	mScale = CVector(1.0f, 1.0f, 1.0f);
}

void CPoint::Set(const CVector&pos, float r){
	mPosition = pos;
	mCollider.mRadius = r;
	CCharacter::Update();
	
}