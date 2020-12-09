#include"CBullet.h"
#include"CSceneGame.h"
CBullet::CBullet()
:mLife(50), mCollider(this, CVector(0.0f, 0.0f, 0.0f), CVector(0.0f, 0.0f, 0.0f), CVector(1.0f, 1.0f, 1.0f), 0.1f)
{
	mCollider.mTag = CCollider::EBULLET;
	mTag = EEYE;

}
//���Ɖ��s���̐ݒ�
//Set(��,���s)
void CBullet::Set(float w, float d){
	//�X�P�[���ݒ�
	mScale = CVector(1.0f, 1.0f, 1.0f);
	//�O�p�`�̒��_�ݒ�
	mT.SetVertex(CVector(w, 0.0f, 0.0f), CVector(-w, 0.0f, 0.0f), CVector(0.0f, 0.0f, d));
	//�O�p�`�̖@���ݒ�
	mT.SetNormal(CVector(0.0f, 1.0f, 0.0f));
}

//�X�V
void CBullet::Update(){
	//�������Ԃ̔���
	if (mLife-- > 0){
		CCharacter::Update();
		//�ʒu�X�V
		mPosition = CVector(0.0f, 0.0f, 1.0f)*mMatrix;
	}
	else{
		//�����ɂ���
		mEnabled = false;
	}
}
//�Փˏ���
//Collision(�R���C�_1,�R���C�_2)
void CBullet::Collision(CCollider*m, CCollider*y){
	if (m->mType == CCollider::ESPHERE&&y->mType == CCollider::ESPHERE){
		//�R���C�_��m��y���Փ˂��Ă��邩����
		if (CCollider::Collision(m, y)){
			//�@�̂̎�
			if (y->mTag == CCollider::EBODY){
				//�Փ˂��Ă���Ƃ��͖����ɂ���
				mEnabled = false;
			}
		}
	}
}

//�`��
void CBullet::Render(){
	//DIFFUSE���F�ݒ�
	float c[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, c);
	//�O�p�`�`��
	mT.Render(mMatrix);
}

CBullet2::CBullet2()
:mLife(100), mCollider(this, CVector(0.0f, 0.0f, 0.0f), CVector(0.0f, 0.0f, 0.0f), CVector(1.0f, 1.0f, 1.0f), 0.1f)
{
	mpModel = &CSceneGame::mCube;
	mCollider.mTag = CCollider::EBULLET;
}

//���Ɖ��s���̐ݒ�
//Set(��,���s)
void CBullet2::Set(float w, float d){
	//�X�P�[���ݒ�
	mScale = CVector(1.0f, 1.0f, 1.0f);
	//�O�p�`�̒��_�ݒ�
	mT.SetVertex(CVector(w, 0.0f, 0.0f), CVector(-w, 0.0f, 0.0f), CVector(0.0f, 0.0f, d));
	//�O�p�`�̖@���ݒ�
	mT.SetNormal(CVector(0.0f, 1.0f, 0.0f));
}

//�X�V
void CBullet2::Update(){
	//�������Ԃ̔���
	if (mLife-- > 0){
		CCharacter::Update();
		//�ʒu�X�V
		CVector dir = CXPlayer::mPlayer->mPosition - mPosition;
		CVector left = CVector(1.0f, 0.0f, 0.0f)  * CMatrix().RotateY(mRotation.mY);
		CVector up = CVector(0.0f, 1.0f, 0.0f) * mMatrix - CVector(0.0f, 0.0f, 0.0f)*mMatrix;
		//�z�[�~���O
		if (left.Dot(dir) > 0.0f){
			mRotation.mY += 0.5f;
		}
		else if (left.Dot(dir) < 0.0f){
			mRotation.mY -= 0.5f;
		}

		if (up.Dot(dir) > 0.0f){
			mRotation.mX -= 1.0f;
		}
		else if (up.Dot(dir) < 0.0f){
			mRotation.mX += 1.0f;
		}
		mPosition = CVector(0.0f, 0.0f, 1.0f) * mMatrix;
	}
	else{
		//�����ɂ���
		mEnabled = false;
	}
	//if (mPosition.mY > 100){
	//	mPosition.mY = 100;
	//}
	//else if (mPosition.mY < -5){
	//	mEnabled = false;
	//}
	//if (mPosition.mZ > 150){
	//	mPosition.mZ = 150;
	//}
	//else if (mPosition.mZ < -150){
	//	mPosition.mY = -150;
	//}
	//if (mPosition.mX > 150){
	//	mPosition.mX = 150;
	//}
	//else if (mPosition.mX < -150){
	//	mPosition.mX = -150;
	//}
}

//�Փˏ���
//Collision(�R���C�_1,�R���C�_2)
void CBullet2::Collision(CCollider *m, CCollider *y){
	//���ɋ��R���C�_�̎�
	if (m->mType == CCollider::ESPHERE && y->mType == CCollider::ESPHERE){
		//�R���C�_�̂��Ƃ����Փ˂��Ă��邩����
		if (CCollider::Collision(m, y)){

			//�v���C���[�̎�
			if (y->mTag == CCollider::EPBODY){
				//�Փ˂��Ă���Ƃ��͖����ɂ���
				mEnabled = false;
			}
		}
	}
}



//�`��
void CBullet2::Render(){
	//DIFUSE���F�ݒ�
	CCharacter::Render();
	//float c[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, c);
	////�O�p�`�`��
	//mT.Render(mMatrix);
	//�m�F�p���\�b�h
	//mCollider.Render();
}