#include"CXEnemy.h"
#include"CXPlayer.h"
CPoint *CXEnemy3::mPoint;
int CXEnemy3::mPointSize = 0;
CPoint *CXEnemy::mPoint;
int CXEnemy::mPointSize = 0;

CXEnemy3*CXEnemy3::mEnemy3 = 0;
bool CXEnemy3::Senser = false;
CXEnemy::CXEnemy()
:mColSphereBody(this, CVector(0.5f, -1.0f, 0.0f), CVector(), CVector(1.0f, 1.0f, 1.0f), 1.0f)
, mColSphereHead(this, CVector(0.0f, 1.0f, 0.0f), CVector(), CVector(1.0f, 1.0f, 1.0f), 1.5f)
, mColSphereSword0(this, CVector(0.7f, 3.5f, -0.2f), CVector(), CVector(1.0f, 1.0f, 1.0f), 0.5f)
, mColSphereSword1(this, CVector(0.5f, 2.5f, -0.2f), CVector(), CVector(1.0f, 1.0f, 1.0f), 0.5f)
, mColSphereSword2(this, CVector(0.3f, 1.5f, -0.2f), CVector(), CVector(1.0f, 1.0f, 1.0f), 0.5f)
, mSearch(this, CVector(0.0f, 0.0f, 0.0f), CVector(0.0f, 0.0f, 0.0f), CVector(1.0f, 1.0f, 1.0f), 10.0f)
{
	mScale = CVector(1.0f, 1.0f, 1.0f);
	mColSphereBody.mTag = CCollider::EBODY;
	mSearch.mTag = CCollider::ESEARCH;
	mPointCnt = 0;//最初のポイントを設定
	mpPoint = &mPoint[mPointCnt];//&mPoint[mPointCnt];//目指すポイントのポインタを設定
	mKAKUNIN = false;
}
void CXEnemy::Init(CModelX*model)
{
	CXCharacter::Init(model);
	//合成行列の設定
	mColSphereBody.mpCombinedMatrix = &mpCombinesMatrix[1];
	//頭
	mColSphereHead.mpCombinedMatrix = &mpCombinesMatrix[1];
	//剣
	mColSphereSword0.mpCombinedMatrix = &mpCombinesMatrix[26];
	mColSphereSword1.mpCombinedMatrix = &mpCombinesMatrix[26];
	mColSphereSword2.mpCombinedMatrix = &mpCombinesMatrix[26];
}
void CXEnemy::Update(){
	if (mpPoint == 0)
	{
		return;
	}
	if (mKAKUNIN == false){
		CVector dir = mpPoint->mPosition - mPosition;
		//左方向のベクトルを求める
		CVector left = CVector(1.0f, 0.0f, 0.0f)*
			CMatrix().RotateY(mRotation.mY);
		//CVector right = CVector(-1.0f, 0.0f, 0.0f)*
		//	CMatrix().RotateY(mRotation.mY);
		//上方向のベクトルを求める
		/*CVector up = CVector(0.0f, 1.0f, 0.0f)*
			CMatrix().RotateX(mRotation.mX)*
			CMatrix().RotateY(mRotation.mY);*/
		//左右の回転処理
		if (left.Dot(dir) > 0.0f){
			mRotation.mY -= 1.5f;
		}
		else if (left.Dot(dir) < 0.0f){
			mRotation.mY += 1.5f;
		}
		mPosition = CVector(0.0f, 0.0f, -0.2f)*mMatrix;

	}
	if (mKAKUNIN == true){
		CVector dir = CXPlayer::mPlayer->mPosition - mPosition;
		//左方向のベクトルを求める
		CVector left = CVector(1.0f, 0.0f, 0.0f)*
			CMatrix().RotateY(mRotation.mY);
		//CVector right = CVector(-1.0f, 0.0f, 0.0f)*
		//	CMatrix().RotateY(mRotation.mY);
		//上方向のベクトルを求める
		/*CVector up = CVector(0.0f, 1.0f, 0.0f)*
		CMatrix().RotateX(mRotation.mX)*
		CMatrix().RotateY(mRotation.mY);*/
		//左右の回転処理
		if (left.Dot(dir) > 0.0f){
			mRotation.mY -= 3.0f;
		}
		else if (left.Dot(dir) < 0.0f){
			mRotation.mY += 3.0f;
		}
		mPosition = CVector(0.0f, 0.0f, -0.5f)*mMatrix;

	}

	////上下の回転処理
	//if (up.Dot(dir)>0.0f){
	//	mRotation.mX -= 0.3f;
	//}
	//else if (up.Dot(dir) < 0.0f){
	//	mRotation.mX += 0.3f;
	//}
	//行列を移動
	CXCharacter::Update();
	//CCharacter::Update();
	//位置を移動
}

void CXEnemy::Collision(CCollider*m, CCollider*y){
	if (m->mType == CCollider::ESPHERE&&y->mType == CCollider::ESPHERE){
		if (CCollider::Collision(m, y)){
			if (m->mTag == CCollider::ESEARCH){
				switch (y->mpParent->mTag){
				case EPLAYER:

					mKAKUNIN = true;

				}
			}
			else{
				switch (y->mpParent->mTag){
				case EPOINT:
					if (y->mpParent == mpPoint){
						mPointCnt++;//次のポイントにする
						//最後だったら最初にする
						mPointCnt %= mPointSize;
						//次のポイントのポインタを設定
						mpPoint = &mPoint[mPointCnt];
					}
					break;
				}
			}
		}
	}
}
CXEnemy2::CXEnemy2(CModel*model, CVector position, CVector rotation, CVector scale)
:mBall(this, CVector(0.0f, 0.0f, 0.0f), CVector(), CVector(1.0f / scale.mX, 1.0f / scale.mY, 1.0f / scale.mZ), 1.0f)
{
	mpModel = model;//モデルの設定
	mPosition = position;//位置の設定
	mRotation = rotation;//回転の設定
	mScale = scale;//拡縮の設定
	mBall.mTag = CCollider::EBODY;
}
void CXEnemy2::Update(){
	CVector dir = CXPlayer::mPlayer->mPosition - mPosition;
	//左方向のベクトルを求める
	CVector left = CVector(1.0f, 0.0f, 0.0f)*
		CMatrix().RotateY(mRotation.mY);
	CVector up = CVector(0.0f, 1.0f, 0.0f)*
	CMatrix().RotateX(mRotation.mX)*
	CMatrix().RotateY(mRotation.mY);

	//左右の回転処理
	if (left.Dot(dir) > 0.0f){
		mRotation.mY += 1.5f;
	}
	else if (left.Dot(dir) < 0.0f){
		mRotation.mY -= 1.5f;
	}
	if (up.Dot(dir)>0.0f){
		mRotation.mX -= 1.0f;
	}
	else if (up.Dot(dir) < 0.0f){
		mRotation.mX += 1.0f;
	}

	if (FireCount2 > 0) {
		FireCount2--;
	}
	else
	{
		CBullet2*bullet = new CBullet2();
		bullet->Set(1.0f, 1.5f);
		bullet->mPosition = CVector(0.0f, 0.0f, 7.0f)*mMatrix;
		bullet->mRotation = mRotation;
		FireCount2 = 90;
	}
	CCharacter::Update();
}

CXEnemy3::CXEnemy3(CModel*model, CVector position, CVector rotation, CVector scale)
:mAir(this, CVector(0.0f, 0.0f, 0.0f), CVector(), CVector(1.0f / scale.mX, 1.0f / scale.mY, 1.0f / scale.mZ), 1.0f)
, mSearch2(this, CVector(0.0f, -20.0f, 0.0f), CVector(0.0f, 0.0f, 0.0f), CVector(1.0f, 1.0f, 1.0f), 30.0f)
{
	mpModel = model;//モデルの設定
	mPosition = position;//位置の設定
	mRotation = rotation;//回転の設定
	mScale = scale;//拡縮の設定
	mAir.mTag = CCollider::EBODY;
	mSearch2.mTag = CCollider::ESEARCH;
	mPointCnt = 0;//最初のポイントを設定
	mpPoint = &mPoint[mPointCnt];//&mPoint[mPointCnt];//目指すポイントのポインタを設定
}

void CXEnemy3::Update(){
	if (mpPoint == 0){
		return;
	}
	CVector dir = mpPoint->mPosition - mPosition;
	//左方向のベクトルを求める
	CVector left = CVector(1.0f, 0.0f, 0.0f)*
		CMatrix().RotateY(mRotation.mY);
	//左右の回転処理
	if (left.Dot(dir) > 0.0f){
		mRotation.mY += 1.0f;
	}
	else if (left.Dot(dir) < 0.0f){
		mRotation.mY -= 1.0f;
	}
	CCharacter::Update();
	mPosition = CVector(0.0f, 0.0f, 0.2f)*mMatrix;
	

}
void CXEnemy3::Collision(CCollider*ms, CCollider*ys){
	if (ms->mType == CCollider::ESPHERE&&ys->mType == CCollider::ESPHERE){
		if (CCollider::Collision(ms, ys)){
			if (ms->mTag == CCollider::ESEARCH){
				switch (ys->mpParent->mTag){
				case EPLAYER:
					CXEnemy3::Senser = true;
				}
			}
			else{
				CXEnemy3::Senser = false;
				switch (ys->mpParent->mTag){
				case EPOINT:
					if (ys->mpParent == mpPoint){
						mPointCnt++;//次のポイントにする
						//最後だったら最初にする
						mPointCnt %= mPointSize;
						//次のポイントのポインタを設定
						mpPoint = &mPoint[mPointCnt];
					}
					break;
				}
			}
		}
	}
}