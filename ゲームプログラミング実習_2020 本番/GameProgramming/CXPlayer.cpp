#include"CXPlayer.h"
#include"CKey.h"
#include"CSceneGame.h"
CXPlayer *CXPlayer::mPlayer = 0;
int CXPlayer::mLife;
CXPlayer::CXPlayer()
:mColSphereBody(this, CVector(), CVector(), CVector(1.0f, 1.0f, 1.0f), 0.5f)
, mColSphereHead(this, CVector(0.0f, 5.0f, -3.0f), CVector(), CVector(1.0f, 1.0f, 1.0f), 0.5f)
, mColSphereSword(this, CVector(-10.0f, 10.0f, 50.0f), CVector(), CVector(1.0f, 1.0f, 1.0f), 0.3f)
, mAshi1(this, CVector(0.0f, 1.5f, 0.0f), CVector(), CVector(5.0f, 5.0f, 5.0f),0.5f)
, mAshi2(this, CVector(0.0f, 1.5f, 0.0f), CVector(), CVector(5.0f, 5.0f, 5.0f), 0.5f)
{
	mScale = CVector(1.0f, 1.0f, 1.0f);
	//タグにプレイヤーを設定します
	mTag = EPLAYER;
	mColSphereBody.mTag = CCollider::EPBODY;
	mColSphereSword.mTag = CCollider::ESWORD;
	mVelovcityJump = 0;
	kasoku = 0;
	kasokuB = 0;
	kasokuC = 0;
	mHyuu = true;
	mPlayer = this;
	mLife = 3;
}

void CXPlayer::Update(){
	if (mAnimationIndex == 3){
		if (mAnimationFrame >= mAnimationFrameSize){
			ChangeAnimation(4, false, 30);
		}
	}
	if (mAnimationIndex == 4){
		if (mAnimationFrame >= mAnimationFrameSize){
			ChangeAnimation(0, true, 60);
		}
	}
	if (CKey::Once(VK_SPACE)){
		if (mAnimationIndex == 0 || mAnimationIndex == 1){
			ChangeAnimation(3, false, 30);
		}
		CXCharacter::Update();
	}
	else{
		if (CKey::Push('W')){
			ChangeAnimation(1, true, 60);
			mPosition = CVector(0.0f, 0.0f, 0.2f + kasoku)*mMatrix;
			if (hayasa > 0){
				hayasa--;
			}
			else if (CKey::Push('B')&&kasoku<0.5f){
				kasoku += 0.15f;
				hayasa = 10;
			}
			else if (kasoku > 0.0f){
				kasoku -= 0.04f;
			}
		}
		else
		{
			kasoku = 0;
			if (mAnimationIndex == 1){
				ChangeAnimation(0, true, 60);
			}
		}
		if (CKey::Push('S')){
			ChangeAnimation(1, true, 60);
			mPosition = CVector(0.0f, 0.0f, -0.2f)*mMatrix;
		}

		if (CKey::Push('A')){
			kasoku = 0;
			mPosition = CVector(0.2f + kasokuB, 0.0f, 0.0f)*mMatrix;
			if (hayasa > 0){
				hayasa--;
			}
			else if (CKey::Push('B') && kasokuB<0.4f){
				kasokuB += 0.2f;
				hayasa = 10;
			}
			else if (kasokuB > 0.0f){
				kasokuB -= 0.05f;
			}
		}
		else
		{
			kasokuB = 0;
		}
		if (CKey::Push('D')){
			kasoku = 0;
			mPosition = CVector(-0.2f + kasokuC, 0.0f, 0.0f)*mMatrix;
			if (hayasa > 0){
				hayasa--;
			}
			else if (CKey::Push('B') && kasokuC >= -0.4){
				kasokuC -= 0.2f;
				hayasa = 20;
			}
			else if (kasokuC < 0.0f){
				kasokuC += 0.05f;
			}
		}
		else
		{
			kasokuC = 0;
		}
		if (CKey::Push('J')){
			mRotation.mY += 3;
		}
		if (CKey::Push('L')){
			mRotation.mY -= 3;
		}
	}
	if (CKey::Once('T')&&mHyuu==true){
		mVelovcityJump = JUMPV0;
		mHyuu = false;
	}
	if (CKey::Push('F') && mHyuu == false){
		if (mVelovcityJump < 0.4f){
			mVelovcityJump += 0.035f;
		}
	}
	mVelovcityJump = mVelovcityJump - G;
	mPosition.mY = mPosition.mY + mVelovcityJump;
	CXCharacter::Update();


}
void CXPlayer::Init(CModelX*model)
{
	CXCharacter::Init(model);
	//合成行列の設定
	mColSphereBody.mpCombinedMatrix = &mpCombinesMatrix[8];
	//頭
	mColSphereHead.mpCombinedMatrix = &mpCombinesMatrix[11];
	//剣
	mColSphereSword.mpCombinedMatrix = &mpCombinesMatrix[21];

	mAshi1.mpCombinedMatrix = &mpCombinesMatrix[25];

	mAshi2.mpCombinedMatrix = &mpCombinesMatrix[30];
}

void CXPlayer::Collision(CCollider*mp, CCollider*yp){
//自身のコライダの判定タイプ
	switch (mp->mType){
	case CCollider::ESPHERE://球コライダ
		if (yp->mType == CCollider::ETRIANGLE){
			CVector adjust;//調整用ベクトル
			if (CCollider::CollisionTriangleSphere(yp, mp, &adjust)){
				mVelovcityJump = 0;
				//位置の更新
				mPosition = mPosition - adjust*-1;
				mHyuu = true;
				//行列の更新
				CXCharacter::Update();
			}
		}
	}
}