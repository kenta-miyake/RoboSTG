#ifndef CSCENEGAME_H
#define CSCENEGAME_H
#include "CScene.h"
#include"CXPlayer.h"
#include"CXEnemy.h"
#include"CKey.h"
#include"CMap.h"
#include"CEye.h"
/*
�Q�[���̃V�[��
*/
class CSceneGame : public CScene {
public:
	CEye mEye;
	CEye2 mEye2;
	CXPlayer Player;
	CXEnemy*mEnemy;
	CXEnemy3 *mEnemy3;
	static CModel mCube;
	~CSceneGame();
	CSceneGame(){};
	//�����������̃I�[�o�[���C�h
	void Init();
	//�X�V�����̃I�[�o�[���C�h
	void Update();
	//�L�����N�^�̃C���X�^���X
	CMap*mMap;
};

#endif
