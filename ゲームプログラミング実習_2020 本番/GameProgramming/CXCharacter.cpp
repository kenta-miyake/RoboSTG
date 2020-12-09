#include"CXCharacter.h"
/*
Init
初期化処理
*/
void CXCharacter::Init(CModelX*model){
	mpModel = model;
	//合成行列避難エリアの確保
	mpCombinesMatrix = new CMatrix[model->mFrame.size()];

	//最初のアニメーションにする
	mAnimationIndex = 0;
	//繰り返し再生する
	mAnimationLoopFlg = true;
	//1アニメーション60フレーム
	mAnimationFrameSize = 60.0f;
	//アニメーションのフレームを最初にする
	mAnimationFrame = 0.0f;
	mpModel->mAnimationSet[mAnimationIndex]->mTime = mAnimationFrame;
	//アニメーションの重みを1.0にする
	mpModel->mAnimationSet[mAnimationIndex]->mWeight = 1.0f;
}

/*
ChageAnimation
アニメーションを切り替える
index:アニメーションの番号
loop:true 繰り返す
framesize:最後まで再生するのに使用されるフレーム数
*/
void CXCharacter::ChangeAnimation(int index, bool loop, float framesize){
	//同じ場合は切り換えない
	if (mAnimationIndex == index)return;
	//今のアニメーションの重みを0.0(0%)にする
	mpModel->mAnimationSet[mAnimationIndex]->mWeight = 0.0f;
	//番号,繰り返し,フレーム数設定
	mAnimationIndex = index%mpModel->mAnimationSet.size();
	mAnimationLoopFlg = loop;
	mAnimationFrameSize = framesize;
	//アニメーションのフレームを最初にする
	mAnimationFrame = 0.0f;
	mpModel->mAnimationSet[mAnimationIndex]->mTime = mAnimationFrame;
	//アニメーションの重みを1.0(100%)にする
	mpModel->mAnimationSet[mAnimationIndex]->mWeight = 1.0f;
}

/*
Update
更新する
matrix:移動,回転,拡大縮小の行列
*/
void CXCharacter::Update(CMatrix&matrix){
	mpModel->mAnimationSet[mAnimationIndex]->mWeight = 1.0f;
	//最後まで再生していない
	if (mAnimationFrame < mAnimationFrameSize){
		//アニメーションの時間を計算
		mpModel->mAnimationSet[mAnimationIndex]->mTime =
			mpModel->mAnimationSet[mAnimationIndex]->mMaxTime*
			mAnimationFrame / mAnimationFrameSize;
		//フレームを進める
		mAnimationFrame++;
	}
	else{
		//繰り返しの場合は、フレームを0に戻す
		if (mAnimationLoopFlg){
			//アニメーションのフレームを最初にする
			mAnimationFrame = 0.0f;
			mpModel->mAnimationSet[mAnimationIndex]->mTime = mAnimationFrame;
		}
		else{
			mpModel->mAnimationSet[mAnimationIndex]->mTime = 
				mpModel->mAnimationSet[mAnimationIndex]->mMaxTime;
		}
	}
	//フレームの変更行列をアニメーションで更新する
	mpModel->AnimateFrame();
	//フレームの合成行列を計算する
	mpModel->mFrame[0]->AnimateCombined(&matrix);
	mpModel->mAnimationSet[mAnimationIndex]->mWeight = 0.0f;

	//合成行列の避難
	for (int i = 0; i < mpModel->mFrame.size(); i++){
		mpCombinesMatrix[i] = mpModel->mFrame[i]->mCombinedMatrix;

	}
	//頂点にアニメーションを適用する
	//mpModel->AnimateVertex();
}

/*
描画する
*/
void CXCharacter::Render(){
	//頂点にアニメーションを適応する
	mpModel->RenderShader(mpCombinesMatrix);
}
CXCharacter::CXCharacter()
:mpCombinesMatrix(0)
{

}
CXCharacter::~CXCharacter(){
	SAFE_DELETE_ARRAY(mpCombinesMatrix);
}