#ifndef CMAP_H
#define CMAP_H

#include"CCharacter.h"
#include"CCollider.h"

class CMap :public CCharacter{
public:
	//�m�F�p
	CCollider mCollider[14];
	//�f�t�H���g�R���X�g���N�^
	CMap();
};
#endif