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
	CCollider mColSphereBody;  //��
	CCollider mColSphereHead;  //��
	CCollider mColSphereSword; //��
	CCollider mAshi1;
	CCollider mAshi2;
	bool mHyuu;
	//�f�t�H���g�R���X�g���N�^
	CXPlayer();
	//�X�V����
	void Update();
	void Init(CModelX*model);
	void Collision(CCollider*mp, CCollider*yp);
	static CXPlayer*mPlayer;
	CXPlayer*mpPoint;//�ڎw���|�C���g
	float kasoku;
	int hayasa;
	float kasokuB;
	float kasokuC;
	static int mLife;
};


#endif
