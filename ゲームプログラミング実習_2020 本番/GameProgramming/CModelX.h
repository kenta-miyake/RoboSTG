#ifndef CMODELX_H  //インクルードガード
#define CMODELX_H
#define MODEL_FILE "ラグナ.x"//入力ファイル名
#define MODEL_FILE2 "knight//knight_low.x"
//領域解放をマクロ化
#define SAFE_DELETE_ARRAY(a){if(a)delete[]a;a=0;}
//配列のサイズ取得をマクロ化
#define ARRAY_SIZE(a)(sizeof(a)/sizeof(a[0]))
#include"CMyShader.h"//シェーダーのインクルード

#include <vector>
#include"CMatrix.h"
#include"CVector.h"

class CModelX;

class CMaterial;//クラスの宣言

/*
CAnimationKey
アニメーションキークラス
*/
class CAnimationKey{
public:
	//時間
	float mTime;
	//行列
	CMatrix mMatrix;

};
/*
CAnimation
*/
class CAnimation{
public:
	char*mpFrameName;//フレーム名
	int mFrameIndex; //フレーム番号
	int mKeyNum;  //キー数(時間数)
	CAnimationKey*mpKey; //キー配列
	CAnimation();
	CAnimation(CModelX *model);

	~CAnimation(){
		SAFE_DELETE_ARRAY(mpFrameName);
	}
};
/*
CAnimationSet
アニメーション
*/
class CAnimationSet{
public:
	//アニメーション
	char *mpName;
	float mTime;       //現在時間
	float mWeight;     //重み
	float mMaxTime;    //最大時間
	CAnimationSet();
	CAnimationSet(CModelX *model);
	std::vector<CAnimation*>mAnimation;
	~CAnimationSet(){
		SAFE_DELETE_ARRAY(mpName);
		//アニメーション要素の削除
		for (int i = 0; i < mAnimation.size(); i++){
			delete mAnimation[i];
		}
	}
};
/*
CSkinWeights
スキンウェイトクラス
*/
class CSkinWeights {
public:
	char *mpFrameName;   //フレーム名
	int mFrameIndex;  //フレーム番号
	int mIndexNum;    //頂点番号数
	int *mpIndex;    //頂点番号配列
	float *mpWeight;
	CMatrix mOffset; //オフセットマトリックス

	CSkinWeights(CModelX *model);

	~CSkinWeights(){
		SAFE_DELETE_ARRAY(mpFrameName);
		SAFE_DELETE_ARRAY(mpIndex);
		SAFE_DELETE_ARRAY(mpWeight);
	}
};
//CMeshクラスの定義
class CMesh{
public:

	int mVertexNum;    //頂点数
	CVector*mpVertex;  //頂点データ
	int mFaceNum;      //画像
	int *mpVertexIndex;//画を構成する
	int mNormalNum;
	//テクスチャ座標データ
	float*mpTextureCoords;
	CVector *mpNormal;

	CVector*mpAnimateVertex;  //アニメーション用頂点
	CVector*mpAnimateNormal;  //アニメーション用法線


	int mMaterialNum;  //マテリアル数
	int mMaterialIndexNum;//マテリアル番号数(面数)
	int *mpMaterilIndex;  //マテリアル番号
	std::vector<CMaterial*>mMaterial;//マテリアルデータ
	std::vector<CSkinWeights*>mSkinWeights;
	//マテリアル毎の面数
	std::vector<int>mMaterialVertexCount;
	//頂点バッファ識別し
	GLuint mMyVertexBufferId;
	//頂点バッファの作成
	void CreateVertexBuffer();
	//コンストラクタ
	CMesh()
		:mVertexNum(0)
		, mpVertex(0)
		, mFaceNum(0)
		, mpVertexIndex(0)
		, mNormalNum(0)
		, mpNormal(0)
		, mMaterialNum(0)
		, mMaterialIndexNum(0)
		, mpMaterilIndex(0)
		, mpAnimateVertex(0)
		, mpAnimateNormal(0)
		, mpTextureCoords(0)
		, mMyVertexBufferId(0)
	{}
	//デストラクタ
	~CMesh(){

		SAFE_DELETE_ARRAY(mpVertex);
		SAFE_DELETE_ARRAY(mpVertexIndex);
		SAFE_DELETE_ARRAY(mpNormal);
		SAFE_DELETE_ARRAY(mpMaterilIndex);
		SAFE_DELETE_ARRAY(mpAnimateVertex);
		SAFE_DELETE_ARRAY(mpAnimateNormal);
		SAFE_DELETE_ARRAY(mpTextureCoords);
		//スキンウェイトの削除
		for (int i = 0; i < mSkinWeights.size(); i++){
			delete mSkinWeights[i];
		}


	}

	//読み込み処理
	void Init(CModelX*model);
	//頂点にアニメーション適用
	void AnimateVertex(CModelX*model);
	void AnimateVertex(CMatrix*);
	void Render();

};
/*
CModelX
Xファイル形式の3Dモデルデータをプログラムで認識する
*/
//CModelXFrameクラスの定義
class CModelXFrame{
public:

	std::vector<CModelXFrame*>mChild; //子フレーム
	CMatrix mTransformMatrix;  //変換行列
	CMatrix mCombinedMatrix;   //合成行列
	CMesh mMesh;    //Meshデータ
	char*mpName;
	int mIndex;

	//コンストラクタ
	CModelXFrame(CModelX*model);
	CModelXFrame()
		:mpName(0)
		, mIndex(0){}

	//デストラクタ
	~CModelXFrame(){
		//子フレームをすべて開放する
		std::vector<CModelXFrame*>::iterator itr;
		for (itr = mChild.begin(); itr != mChild.end(); itr++){
			delete*itr;
		}
		//名前のエリアを開放する
		SAFE_DELETE_ARRAY(mpName);

	}
	void AnimateCombined(CMatrix*parent);
	void Render();
};

class CModelX{
public:

	int GetIntToken();
	CMatrix mTransformMatrix;  //変換行列
	//シェーダー用のスキンマトリックス
	CMatrix *mpSkinningMatrix;
	CMyShader mShader;//シェーダーのインスタンス

	char* mpPointer;
	char mToken[1024];

	//浮動小数点
	float GetFloatToken();
	std::vector<CModelXFrame*>mFrame;  //フレームの配列
	std::vector<CAnimationSet*>mAnimationSet;
	std::vector<CMaterial*>mMaterial;  //マテリアルの配列
	CModelX()
		:mpPointer(0)
		, mpSkinningMatrix(0)

	{}
	~CModelX(){
		delete mFrame[0];
		for (int i = 0; i < mAnimationSet.size(); i++){
			delete mAnimationSet[i];
		}
		for (int i = 0; i < mMaterial.size(); i++){
			delete mMaterial[i];
		}
		SAFE_DELETE_ARRAY(mpSkinningMatrix);

	}

	//ファイル読み込み
	void Load(char*file);
	void GetToken();
	void AnimateFrame();
	void SetSkinWeightFrameIndex();
	void AnimateVertex();
	void AnimateVertex(CMatrix*);
	void SeparateAnimationSet(int idx, int start, int end, char*name);
	//ノードの読み込み
	void SkipNode();
	void Render();
	//シェーダーを使った描画
	void RenderShader(CMatrix*m);

	//マテリアルの検索
	CMaterial*FindMaterial(char*name);

	//単位行列の作成
	CMatrix Identity();
	/*
	FindFrame
	フレーム名に該当するフレームのアドレスを返す
	*/
	CModelXFrame*CModelX::FindFrame(char*name){
		//イテレータの作成
		std::vector<CModelXFrame*>::iterator itr;
		//先頭から最後まで繰り返す
		for (itr = mFrame.begin(); itr != mFrame.end(); itr++){
			//名前が一致したか？
			if (strcmp(name, (*itr)->mpName) == 0){
				//一致したらそのアドレスを返す
				return *itr;
			}
		}
		//一致するフレーム無い場合NULLを返す
		return NULL;
	}

};

#endif
