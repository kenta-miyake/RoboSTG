#ifndef CMAP_H
#define CMAP_H

#include"CCharacter.h"
#include"CCollider.h"

class CMap :public CCharacter{
public:
	//確認用
	CCollider mCollider[14];
	//デフォルトコンストラクタ
	CMap();
};
#endif