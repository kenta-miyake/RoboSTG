#ifndef CBULLET_H
#define CBULLET_H
//�L�����N�^�N���X�̃C���N���[�h
#include"CCharacter.h"
//�O�p�`�N���X�̃C���N���[�h
#include"CTriangle.h"
#include"CCollider.h"
#include "CCollisionManager.h"
/*
�e�N���X
�O�p�`���΂�
*/
class CBullet :public CCharacter{
public:
	CCollider mCollider;
	//�O�p�`
	CTriangle mT;
	//���Ɖ��s���̐ݒ�
	//Set(��,���s)
	void Set(float w, float d);
	//�X�V
	void Update();
	//�`��
	void Render();
	//��������
	int mLife;
	CBullet();
	//�Փˏ���
	//Collision(�R���C�_1,�R���C�_2)
	void Collision(CCollider*m, CCollider*y);
};

class CBullet2 :public CCharacter{
public:
	int mLife;
	//�O�p�`
	CTriangle mT;
	//���Ɖ��s���̐ݒ�
	//Set(��,���s)
	void Set(float w, float d);
	//�R���C�_
	CCollider mCollider;
	CBullet2();
	//�Փˏ���
	//Collision(�R���C�_1,�R���C�_2)
	void Collision(CCollider *m, CCollider *y);

	void Update();
	void Render();
};

#endif
