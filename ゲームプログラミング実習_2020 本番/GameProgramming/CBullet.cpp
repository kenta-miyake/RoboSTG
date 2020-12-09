#include"CBullet.h"
#include"CSceneGame.h"
CBullet::CBullet()
:mLife(50), mCollider(this, CVector(0.0f, 0.0f, 0.0f), CVector(0.0f, 0.0f, 0.0f), CVector(1.0f, 1.0f, 1.0f), 0.1f)
{
	mCollider.mTag = CCollider::EBULLET;
	mTag = EEYE;

}
//幅と奥行きの設定
//Set(幅,奥行)
void CBullet::Set(float w, float d){
	//スケール設定
	mScale = CVector(1.0f, 1.0f, 1.0f);
	//三角形の頂点設定
	mT.SetVertex(CVector(w, 0.0f, 0.0f), CVector(-w, 0.0f, 0.0f), CVector(0.0f, 0.0f, d));
	//三角形の法線設定
	mT.SetNormal(CVector(0.0f, 1.0f, 0.0f));
}

//更新
void CBullet::Update(){
	//生存時間の判定
	if (mLife-- > 0){
		CCharacter::Update();
		//位置更新
		mPosition = CVector(0.0f, 0.0f, 1.0f)*mMatrix;
	}
	else{
		//無効にする
		mEnabled = false;
	}
}
//衝突処理
//Collision(コライダ1,コライダ2)
void CBullet::Collision(CCollider*m, CCollider*y){
	if (m->mType == CCollider::ESPHERE&&y->mType == CCollider::ESPHERE){
		//コライダのmとyが衝突しているか判定
		if (CCollider::Collision(m, y)){
			//機体の時
			if (y->mTag == CCollider::EBODY){
				//衝突しているときは無効にする
				mEnabled = false;
			}
		}
	}
}

//描画
void CBullet::Render(){
	//DIFFUSE黄色設定
	float c[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, c);
	//三角形描画
	mT.Render(mMatrix);
}

CBullet2::CBullet2()
:mLife(100), mCollider(this, CVector(0.0f, 0.0f, 0.0f), CVector(0.0f, 0.0f, 0.0f), CVector(1.0f, 1.0f, 1.0f), 0.1f)
{
	mpModel = &CSceneGame::mCube;
	mCollider.mTag = CCollider::EBULLET;
}

//幅と奥行きの設定
//Set(幅,奥行)
void CBullet2::Set(float w, float d){
	//スケール設定
	mScale = CVector(1.0f, 1.0f, 1.0f);
	//三角形の頂点設定
	mT.SetVertex(CVector(w, 0.0f, 0.0f), CVector(-w, 0.0f, 0.0f), CVector(0.0f, 0.0f, d));
	//三角形の法線設定
	mT.SetNormal(CVector(0.0f, 1.0f, 0.0f));
}

//更新
void CBullet2::Update(){
	//生存時間の判定
	if (mLife-- > 0){
		CCharacter::Update();
		//位置更新
		CVector dir = CXPlayer::mPlayer->mPosition - mPosition;
		CVector left = CVector(1.0f, 0.0f, 0.0f)  * CMatrix().RotateY(mRotation.mY);
		CVector up = CVector(0.0f, 1.0f, 0.0f) * mMatrix - CVector(0.0f, 0.0f, 0.0f)*mMatrix;
		//ホーミング
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
		//無効にする
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

//衝突処理
//Collision(コライダ1,コライダ2)
void CBullet2::Collision(CCollider *m, CCollider *y){
	//共に球コライダの時
	if (m->mType == CCollider::ESPHERE && y->mType == CCollider::ESPHERE){
		//コライダのｍとｙが衝突しているか判定
		if (CCollider::Collision(m, y)){

			//プレイヤーの時
			if (y->mTag == CCollider::EPBODY){
				//衝突しているときは無効にする
				mEnabled = false;
			}
		}
	}
}



//描画
void CBullet2::Render(){
	//DIFUSE黄色設定
	CCharacter::Render();
	//float c[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, c);
	////三角形描画
	//mT.Render(mMatrix);
	//確認用メソッド
	//mCollider.Render();
}