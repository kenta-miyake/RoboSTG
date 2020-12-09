#include "CMaterial.h"
//memsetのインクルード
#include <string.h>
#include"glew.h"
#include "glut.h"

//デフォルトコンストラクタ
CMaterial::CMaterial()
: mpTexture(0), mVertexNum(0)
, mpTexterFilename(0)
{
	//名前を0で埋め
	memset(mName, 0, sizeof(mName));
	//0で埋める
	memset(mDiffuse, 0, sizeof(mDiffuse));
}

//マテリアルを有効にする
void CMaterial::Enabled() {
	//アルファブレンドを有効にする
	glEnable(GL_BLEND);
	//ブレンド方法を指定
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//拡散光の設定
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiffuse);
	//テクスチャが無い時、戻る
	if (mpTexture == 0)
		return;
	//テクスチャ有り
	if (mpTexture->mId) {
		//テクスチャを使用可能にする
		glEnable(GL_TEXTURE_2D);
		//テクスチャをバインドする
		glBindTexture(GL_TEXTURE_2D, mpTexture->mId);
	}
}
//マテリアルを無効にする
void CMaterial::Disabled() {
	//アルファブレンドを無効
	glDisable(GL_BLEND);
	//テクスチャが無い時、戻る
	if (mpTexture == 0)
		return;
	//テクスチャ有り
	if (mpTexture->mId) {
		//テクスチャのバインドを解く
		glBindTexture(GL_TEXTURE_2D, 0);
		//テクスチャを無効にする
		glDisable(GL_TEXTURE_2D);
	}
}
/*
Materialデータの読み込みと設定
*/
CMaterial::CMaterial(CModelX*model)
:mpTexterFilename(0)
{
	//CModelXにマテリアルを追加する
	model->mMaterial.push_back(this);

	model->GetToken();//{?Name
	if (strcmp(model->mToken, "{") != 0){
		//{出ないときはマテリアル名
		strcpy(mName, model->mToken);
		model->GetToken();//{
	}
	mDiffuse[0] = model->GetFloatToken();
	mDiffuse[1] = model->GetFloatToken();
	mDiffuse[2] = model->GetFloatToken();
	mDiffuse[3] = model->GetFloatToken();

	mPower = model->GetFloatToken();

	mSpeculer[0] = model->GetFloatToken();
	mSpeculer[1] = model->GetFloatToken();
	mSpeculer[2] = model->GetFloatToken();

	mEmissive[0] = model->GetFloatToken();
	mEmissive[1] = model->GetFloatToken();
	mEmissive[2] = model->GetFloatToken();

	model->GetToken();//TextureFilename or }

	if (strcmp(model->mToken, "TextureFilename") == 0){
		//テクスチャありの場合、テクスチャファイル名取得
		model->GetToken();//{
		model->GetToken();//filename
		mpTexterFilename = new char[strlen(model->mToken) + 1];
		strcpy(mpTexterFilename, model->mToken);
		//スマートポインタの生成
		std::shared_ptr<CTexture>t(new CTexture());
		//スマートポインタの代入
		mpTexture = t;
		//テクスチャファイルの読み込み
		t->Load(mpTexterFilename);

		model->GetToken();//{
		model->GetToken();//{
	}
	//printf("%s\n", mName);
	//printf("Diffuse:%10f%10f%10f%10f\n", mDiffuse[0], mDiffuse[1], mDiffuse[2], mDiffuse[3]);
	//printf("Power:%10f\n", mPower);
	//printf("Specular:%10f%10f%10f\n", mSpeculer[0], mSpeculer[1], mSpeculer[2]);
	//printf("Emissive:%10f%10f%10f\n",mEmissive[0],mEmissive[1],mEmissive[2]);
}
