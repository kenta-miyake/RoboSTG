#ifndef CEYE_H
#define CEYE_H
#include"CBullet.h"
#include"CXCharacter.h"
#include"CCollider.h"
class CEye :public CCharacter{
public:
	int FireCount;
	//�f�t�H���g�R���X�g���N�^
	CEye(CModel*model, CVector position, CVector rotation, CVector scale);
	//�X�V����
	void Update();
	CEye(){};

};

class CEye2 :public CCharacter{
public:
	int FireCount3;
	//�f�t�H���g�R���X�g���N�^
	CEye2(CModel*model, CVector position, CVector rotation, CVector scale);
	//�X�V����
	void Update();
	CEye2(){};

};

#endif
