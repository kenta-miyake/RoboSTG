#ifndef CEYE_H
#define CEYE_H
#include"CBullet.h"
#include"CXCharacter.h"
#include"CCollider.h"
class CEye :public CCharacter{
public:
	int FireCount;
	//デフォルトコンストラクタ
	CEye(CModel*model, CVector position, CVector rotation, CVector scale);
	//更新処理
	void Update();
	CEye(){};

};

class CEye2 :public CCharacter{
public:
	int FireCount3;
	//デフォルトコンストラクタ
	CEye2(CModel*model, CVector position, CVector rotation, CVector scale);
	//更新処理
	void Update();
	CEye2(){};

};

#endif
