#include<stdio.h>
#include<string.h>
#include"CModelX.h"
#include"CMaterial.h"
#include"CVertex.h"
void CModelX::Load(char* file){
	//
	//ファイルサイズを取得する
	//
	FILE *fp;   //ファイルポインタ変数の作成
	fp = fopen(file, "rb");//ファイルオープン
	if (fp == NULL){       //エラーチェック
		printf("fopen error:%s\n", file);
		return;
	}
	//ファイルの最後へ移動
	fseek(fp, 0L, SEEK_END);
	//ファイルサイズの取得
	int size = ftell(fp);
	//ファイルサイズ+1バイト分の領域を確保
	char *buf = mpPointer = new char[size + 1];
	//
	//ファイルから3Dモデルのデータを読み込む
	//
	//ファイルの先頭へ移動
	fseek(fp, 0L, SEEK_SET);
	//確保した領域にファイルサイズ分データを読み込む
	fread(buf, size, 1, fp);
	//最後に\0を設定する(文字列の終端)
	buf[size] = '\0';
	fclose(fp);      //ファイルをクローズする

	//ダミールートフレームの作成
	CModelXFrame*p = new CModelXFrame();
	//名前なし
	p->mpName = new char[1];
	p->mpName[0] = '\0';
	//フレーム配列に追加
	mFrame.push_back(p);

	//入力した値をコンソールに出力する
	//printf("%s", buf);

	//文字列の最後まで繰り返し
	while (*mpPointer != '\0'){
		GetToken();   //単語の取得
		//template
		if (strcmp(mToken, "template") == 0){
			SkipNode();
		}
		//Materialの時
		else if (strcmp(mToken, "Material") == 0){
			new CMaterial(this);
		}
		//GetFloatToken();
		//単語がFrameの場合
		else if (strcmp(mToken, "Frame") == 0){
			//フレーム名取得
			GetToken();
			if (strchr(mToken, '{')){
				//フレーム名なし：スキップ
				SkipNode();
				GetToken();//}
			}
			else{
				//フレームがなければ
				if (FindFrame(mToken) == 0){
					//フレームを作成する
					p->mChild.push_back(new CModelXFrame(this));
				}
			}
			////ふれーむを作成する
			//new CModelXFrame(this);

			//printf("%s ", mToken);//Frameを出力
			//GetToken();	         //Frame名を取得
			//printf("%s\n", mToken);   //Frame名を出力
		}
		//単語がAnimationSetの場合
		else if (strcmp(mToken, "AnimationSet") == 0){
			new CAnimationSet(this);

		}

		//if (strcmp(mToken, "AnimationSet") == 0){
		//	printf("%s ", mToken);//Frameを出力
		//	GetToken();	         //Frame名を取得
		//	printf("%s\n", mToken);   //Frame名を出力
		//}
	}
	//スキンウェイトのフレーム番号設定
	SetSkinWeightFrameIndex();
	//頂点バッファの作成
	for (int i = 0; i < mFrame.size(); i++){
		if (mFrame[i]->mMesh.mFaceNum>0){
			mFrame[i]->mMesh.CreateVertexBuffer();
		}
	}
	//スキンマトリックスのエリア作成
	mpSkinningMatrix = new CMatrix[mFrame.size()];
	//シェーダー読み込み
	mShader.Load("skinmesh.vert", "skinmesh.flag");
	SAFE_DELETE_ARRAY(buf);


}
void CModelX::GetToken(){
	char* p = mpPointer;
	char* q = mToken;
	//空白()タブ(\t)改行(\r)()\n,;"以外の文字になるまで読み飛ばす
	/*
	strchr(文字列,文字)
	文字列に文字が含まれていれば,見つかった文字へのポインタを返す
	見つからなかったらNULLを返す
	*/
	while (*p != '\0'&& strchr(" \t\r\n,;\"", *p))p++;
	if (*p == '{' || *p == '}'){
		//{または}ならmTokenに代入し次の文字へ
		*q++ = *p++;
	}
	else{
		//空白()タブ(\t)改行(\r)()\n,;"}の文字になるまでmTokenに代入する
		while (*p != '\0'&& !strchr(" \t\r\n,;\"}", *p))
			*q++ = *p++;

	}
	*q = '\0';		//mTokenの最後に\0を代入
	mpPointer = p;  //次の読み込むポイントを更新する

	//もしmTokunが//の場合は、コメントなので改行まで読み飛ばす
	/*
	strchr(文字列,文字)
	文字列1と文字列2が等しい場合,0を返す
	文字列1と文字列2が等しい場合,0以外を返す
	*/
	if (!strcmp("//", mToken)){
		//改行まで読み飛ばす
		while (*p != '\0'&& !strchr("\r\n", *p))p++;
		//読み込み位置の更新
		mpPointer = p;
		//単語を取得する(再起呼び出し)
		GetToken();
	}


}
void CModelX::SkipNode(){
	//文字が終わったら終了
	while (*mpPointer != '\0'){
		GetToken();  //次の単語取得
		//{が見つかったらループ終了
		if (strchr(mToken, '{')) break;
	}
	int count = 1;
	//文字が終わるか、カウントが0になったら終了
	while (*mpPointer != '\0'&&count > 0){
		GetToken();    //次の単語取得
		//{を見つけるとカウントアップ
		if (strchr(mToken, '{'))count++;
		//}を見つけるとカウントダウン
		else if (strchr(mToken, '}'))count--;
	}
}
/*
CmodelXFrame
model:CModelXインスタンス
フレームを作成
読み込み中にFrameが見つかれば、フレームを作成し
子フレームに追加する
*/
CModelXFrame::CModelXFrame(CModelX*model){
	//現在のフレーム配列の要素数を取得し設定する
	mIndex = model->mFrame.size();
	//CModelXのフレーム配列に追加する
	model->mFrame.push_back(this);
	//変換行列を単位行列にする
	mTransformMatrix.Identity();
	//次の単語(フレーム名の予定)を取得する
	//削除	model->GetToken();//frame name
	//フレーム名分エリアを確保する
	mpName = new char[strlen(model->mToken) + 1];
	//フレーム名をコピーする
	strcpy(mpName, model->mToken);
	//次の単語({の予定)を取得する
	model->GetToken();//{
	//文字がなくなった終わり
	while (*model->mpPointer != '\0'){
		//次の単語取得
		model->GetToken();//Frame
		//}の場合は終了
		if (strchr(model->mToken, '}'))
			break;
		//新なフレームの場合は子フレームに追加
		if (strcmp(model->mToken, "Frame") == 0){
			model->GetToken();
			if (strchr(model->mToken, '{')){
				//フレーム名なし：スキップ
				model->SkipNode();
				model->GetToken();//}
			}
			else{
				//フレームがなければ
				if (model->FindFrame(model->mToken) == 0){
					//ふれーむを作成し、子フレームの配列に追加
					mChild.push_back(new CModelXFrame(model));
				}
			}
		}
		else if (strcmp(model->mToken, "FrameTransformMatrix") == 0){
			model->GetToken();//{
			for (int i = 0; i < ARRAY_SIZE(mTransformMatrix.mF); i++){
				mTransformMatrix.mF[i] = model->GetFloatToken();
			}
			model->GetToken();//}

		}
		else if (strcmp(model->mToken, "Mesh") == 0){
			mMesh.Init(model);

		}
		else{
			//上記以外の要素は読み飛ばす
			model->SkipNode();
		}
	}
	//デバッグバージョンのみ有効
#ifdef _DEBUG
	//printf("%s \n", mpName);
	//mTransformMatrix.Print();


#endif
}



/*
GetFloatToken
*/
float CModelX::GetFloatToken(){
	GetToken();
	//atof
	//文字列をfloat型へ変換
	return atof(mToken);


}
/*
GetIntToken
単語を整数型のデータで渡す
*/

int CModelX::GetIntToken(){
	GetToken();
	return atoi(mToken);
}
void CMesh::Init(CModelX*model){

	model->GetToken();   //{or名前
	if (!strchr(model->mToken, '{')){
		//名前の場合,次が{
		model->GetToken();  //{
	}
	//頂点数の取得
	mVertexNum = model->GetIntToken();
	//頂点数分エリア確保
	mpVertex = new CVector[mVertexNum];
	mpAnimateVertex = new CVector[mVertexNum];


	//頂点数分データを取り込む
	for (int i = 0; i < mVertexNum; i++){
		mpVertex[i].mX = model->GetFloatToken();
		mpVertex[i].mY = model->GetFloatToken();
		mpVertex[i].mZ = model->GetFloatToken();
	}
	mFaceNum = model->GetIntToken();    //画像読み込み
	//頂点数は,1画に3頂点
	mpVertexIndex = new int[mFaceNum * 3];
	for (int i = 0; i < mFaceNum * 3; i += 3){
		model->GetIntToken();//頂点数読み込み
		mpVertexIndex[i] = model->GetIntToken();
		mpVertexIndex[i + 1] = model->GetIntToken();
		mpVertexIndex[i + 2] = model->GetIntToken();
	}
	//文字がなくなったら終わり
	while (model->mpPointer != '\0'){
		model->GetToken();    //MeshNomals
		//}
		if (strchr(model->mToken, '}'))
			break;
		if (strcmp(model->mToken, "MeshNormals") == 0){
			model->GetToken();    //{
			mNormalNum = model->GetIntToken();
			CVector*pNormal = new CVector[mNormalNum];
			for (int i = 0; i < mNormalNum; i++){
				pNormal[i].mX = model->GetFloatToken();
				pNormal[i].mY = model->GetFloatToken();
				pNormal[i].mZ = model->GetFloatToken();

			}
			//法線数=面積*3
			mNormalNum = model->GetIntToken() * 3;//FaceNum
			int ni;
			mpNormal = new CVector[mNormalNum];
			mpAnimateNormal = new CVector[mNormalNum];

			for (int i = 0; i < mNormalNum; i += 3){
				model->GetToken();
				ni = model->GetIntToken();
				mpNormal[i] = pNormal[ni];

				ni = model->GetIntToken();
				mpNormal[i + 1] = pNormal[ni];

				ni = model->GetIntToken();
				mpNormal[i + 2] = pNormal[ni];
			}
			delete[] pNormal;
			model->GetIntToken();
		}

		else if (strcmp(model->mToken, "MeshMaterialList") == 0){
			model->GetToken();
			//Materialのかず
			mMaterialNum = model->GetIntToken();
			//FaceNum
			mMaterialIndexNum = model->GetIntToken();
			//マテリアルインデックスの作成
			mpMaterilIndex = new int[mMaterialIndexNum];
			for (int i = 0; i < mMaterialIndexNum; i++){
				mpMaterilIndex[i] = model->GetIntToken();
			}
			//マテリアルデータの作成
			for (int i = 0; i < mMaterialNum; i++){
				model->GetToken();     //Material
				if (strcmp(model->mToken, "Material") == 0){
					mMaterial.push_back(new CMaterial(model));
				}
				else{
					//{既出
					model->GetToken();    //MaterilName

					mMaterial.push_back(model->FindMaterial(model->mToken));
					model->GetToken();  //}
				}

			}
			model->GetToken();    //}//End of MeshMaterialList
		}

		else if (strcmp(model->mToken, "SkinWeights") == 0){
			//SKinWeightsクラスのインスタンスを作成,配列に追加
			mSkinWeights.push_back(new CSkinWeights(model));
		}
		//テクスチャ座標の時
		else if (strcmp(model->mToken, "MeshTextureCoords") == 0){
			model->GetToken();  //{
			//テクスチャ座標数を取得
			int textureCoodsNum = model->GetIntToken() * 2;
			//テクスチャ座標のデータを配列に取り込む
			mpTextureCoords = new float[textureCoodsNum];
			for (int i = 0; i < textureCoodsNum; i++){
				mpTextureCoords[i] = model->GetFloatToken();

			}
			model->GetToken();   //}
		}
		else{
			//以外のノードは読み飛ばし
			model->SkipNode();
		}
	}

#ifdef _DEBUG

	/*printf("VertexNum :%d \n", mVertexNum);
	for (int i = 0; i < mVertexNum; i++){
	printf("%10f%10f%10f\n", mpVertex[i].mX, mpVertex[i].mY, mpVertex[i].mZ);
	}*/
	/*printf("FaceNum:%d \n", mFaceNum);
	for (int i = 0; i < mFaceNum * 3; i += 3){
	printf("%10d%10d%10d\n", mpVertexIndex[i], mpVertexIndex[i + 1], mpVertexIndex[i + 2]);
	}*/

	/*printf("NormalNum :%d \n", mNormalNum);
	for (int i = 0; i < mNormalNum; i ++){
	printf("%10f%10f%10f\n", mpNormal[i].mX, mpNormal[i].mY, mpNormal[i].mZ);
	}*/

#endif
}
/*
AnimateVertex頂点にアニメーションを適用
*/
void CMesh::AnimateVertex(CModelX*model){
	//アニメーション用の頂点エリアクリア
	memset(mpAnimateVertex, 0, sizeof(CVector)*mVertexNum);
	memset(mpAnimateNormal, 0, sizeof(CVector)*mNormalNum);
	//スキンウェイト分繰り返し
	for (int i = 0; i < mSkinWeights.size(); i++){
		//フレーム番号取得
		int frameIndex = mSkinWeights[i]->mFrameIndex;
		//オフセット行列とフレーム合成行列を合成
		CMatrix mSkinningMatrix = mSkinWeights[i]->mOffset*model->mFrame[frameIndex]->mCombinedMatrix;
		//頂点数分繰り返し
		for (int j = 0; j < mSkinWeights[i]->mIndexNum; j++){
			//頂点番号取得
			int index = mSkinWeights[i]->mpIndex[j];
			//重み取得
			float weight = mSkinWeights[i]->mpWeight[j];
			//頂点と法線を更新する
			mpAnimateVertex[index] += mpVertex[index] * mSkinningMatrix*weight;
			mpAnimateNormal[index] += mpNormal[index] * mSkinningMatrix*weight;
		}
	}
	for (int i = 0; i < mNormalNum; i++){
		mpAnimateNormal[i] = mpAnimateNormal[i].Normalize();
	}
}
void CMesh::AnimateVertex(CMatrix*mat){
	//アニメーション用の頂点エリアクリア
	memset(mpAnimateVertex, 0, sizeof(CVector)*mVertexNum);
	memset(mpAnimateNormal, 0, sizeof(CVector)*mNormalNum);
	//スキンウェイト分繰り返し
	for (int i = 0; i < mSkinWeights.size(); i++){
		//フレーム番号取得
		int frameIndex = mSkinWeights[i]->mFrameIndex;
		//オフセット行列とフレーム合成行列を合成
		CMatrix mSkinningMatrix = mSkinWeights[i]->mOffset * mat[frameIndex];
		//頂点数分繰り返し
		for (int j = 0; j < mSkinWeights[i]->mIndexNum; j++){
			//頂点番号取得
			int index = mSkinWeights[i]->mpIndex[j];
			//重み取得
			float weight = mSkinWeights[i]->mpWeight[j];
			//頂点と法線を更新する
			mpAnimateVertex[index] += mpVertex[index] * mSkinningMatrix*weight;
			mpAnimateNormal[index] += mpNormal[index] * mSkinningMatrix*weight;
		}
	}
	for (int i = 0; i < mNormalNum; i++){
		mpAnimateNormal[i] = mpAnimateNormal[i].Normalize();
	}
}
/*
Rnder
画面に描画
*/
void CMesh::Render(){



	/*頂点データ,法線データの配列を有効にする*/
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//テクスチャまっぴんグの配列を有効にする
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	/*頂点データ,法線データの場所を指定する*/
	glVertexPointer(3, GL_FLOAT, 0, mpAnimateVertex);
	glNormalPointer(GL_FLOAT, 0, mpAnimateNormal);
	glTexCoordPointer(2, GL_FLOAT, 0, mpTextureCoords);
	/*頂点のインデックスの場所を指定して図形を描画する*/
	for (int i = 0; i < mFaceNum; i++){
		//マテリアルを適用
		mMaterial[mpMaterilIndex[i]]->Enabled();
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (mpVertexIndex + i * 3));
		mMaterial[mpMaterilIndex[i]]->Disabled();
	}

	///*頂点のインデックスの場所を指定して図形を描画する*/
	//glDrawElements(GL_TRIANGLES, 3 * mFaceNum, 
	//	GL_UNSIGNED_INT, mpVertexIndex);
	/*頂点データ,法線データの配列を無効にする*/
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);


}
/*
Rnder
メッシュの画数が0以外なら描画する
*/
void CModelXFrame::Render(){
	if (mMesh.mFaceNum != 0)
		mMesh.Render();
}
/*
Render
全てのフレームの描画処理を呼び起こす
*/
void CModelX::Render(){
	for (int i = 0; i < mFrame.size(); i++){
		mFrame[i]->Render();
	}
}

/*
CSkinWeights
スキンウェイトクラス
*/
CSkinWeights::CSkinWeights(CModelX *model)
: mpFrameName(0)
, mFrameIndex(0)
, mIndexNum(0)
, mpIndex(0)
, mpWeight(0)

{
	model->GetToken();       //{
	model->GetToken();       //FrameName
	//フレーム名エリア確保,設定
	mpFrameName = new char[strlen(model->mToken) + 1];
	strcpy(mpFrameName, model->mToken);
	//頂点番号取得
	mIndexNum = model->GetIntToken();
	//頂点番号数が0を超える
	if (mIndexNum > 0){
		//頂点番号と頂点ウェイトのエリア確保
		mpIndex = new int[mIndexNum];
		mpWeight = new float[mIndexNum];
		//頂点番号取得
		for (int i = 0; i < mIndexNum; i++){
			mpIndex[i] = model->GetIntToken();
		}
		//頂点ウェイト取得
		for (int i = 0; i < mIndexNum; i++){
			mpWeight[i] = model->GetFloatToken();
		}
	}
	//オフセット行列取得
	for (int i = 0; i < 16; i++){
		mOffset.mF[i] = model->GetFloatToken();
	}
	model->GetToken();



#ifdef _DEBUG


	//printf("SkinWeights %s \n",mpFrameName);
	//for (int i = 0; i < mIndexNum; i++){
	//	printf("%d %f  \n", mpIndex[i], mpWeight[i]);
	//
	//}
	//for (int i = 0; i < 4*4; i+=4){
	//	printf(" %f ", mOffset.mF[i]);
	//	printf(" %f ", mOffset.mF[i + 1]);
	//	printf(" %f ", mOffset.mF[i + 2]);
	//	printf(" %f \n", mOffset.mF[i + 3]);
	//}


#endif
}
CAnimationSet::CAnimationSet()
: mpName(0)
, mTime(0)
, mWeight(0)
, mMaxTime(0)
{

}
/*
CAnimationSet
*/
CAnimationSet::CAnimationSet(CModelX *model)
: mpName(0)
, mTime(0)
, mWeight(0)
, mMaxTime(0)
{
	model->mAnimationSet.push_back(this);
	model->GetToken();       //Animation Name
	//アニメーション名を退避
	mpName = new char[strlen(model->mToken) + 1];
	strcpy(mpName, model->mToken);
	model->GetToken(); //{
	while (*model->mpPointer != '\0'){
		model->GetToken();//}or Animation
		if (strchr(model->mToken, '}'))break;
		if (strcmp(model->mToken, "Animation") == 0){
			//とりあえず読み飛ばし
			//model->SkipNode();
			//Animation要素
			mAnimation.push_back(new CAnimation(model));

		}
	}
	//終了時間設定
	mMaxTime = mAnimation[0]->mpKey[mAnimation[0]->mKeyNum - 1].mTime;
#ifdef _DEBUG
	/*printf("Animation %s \n", mpName);*/
#endif
}
CAnimation::CAnimation()
: mpFrameName(0)
, mFrameIndex(0)
, mKeyNum(0)
, mpKey(0)
{

}
CAnimation::CAnimation(CModelX*model)
: mpFrameName(0)
, mFrameIndex(0)
, mKeyNum(0)
, mpKey(0)
{
	model->GetToken();  //{ or Animation Name
	if (strchr(model->mToken, '{')){
		model->GetToken(); //{
	}
	else{
		model->GetToken();  //{
		model->GetToken();  //{
	}
	model->GetToken();  //FrameName
	mpFrameName = new char[strlen(model->mToken) + 1];
	strcpy(mpFrameName, model->mToken);
	mFrameIndex = model->FindFrame(model->mToken)->mIndex;
	model->GetToken(); //}

	//キーの配列を保存しておく配列
	CMatrix*Key[4] = { 0, 0, 0, 0 };
	//時間の配列を保存しておく配列
	float*time[4] = { 0, 0, 0, 0 };
	while (*model->mpPointer != '\0'){
		model->GetToken();  //} or AnimationKey
		if (strchr(model->mToken, '}'))break;
		if (strcmp(model->mToken, "AnimationKey") == 0){
			model->GetToken();//{
			//データのタイプ取得
			int type = model->GetIntToken();
			//時間数取得
			mKeyNum = model->GetIntToken();
			switch (type){
			case 0://Rotation Quaternion
				//行列の配列を時間数分確保
				Key[type] = new CMatrix[mKeyNum];
				//時間の配列を時間数分確保
				time[type] = new float[mKeyNum];
				//時間数分繰り返す
				for (int i = 0; i < mKeyNum; i++){
					//時間取得
					time[type][i] = model->GetFloatToken();
					model->GetToken();//4を読み飛ばす
					//w,x,y,zを取得
					float w = model->GetFloatToken();
					float x = model->GetFloatToken();
					float y = model->GetFloatToken();
					float z = model->GetFloatToken();
					//クオータニオンから回転上列に変換
					Key[type][i].SetQuaternion(x, y, z, w);
				}
				break;

			case 1://拡大・縮小の行列作成
				Key[type] = new CMatrix[mKeyNum];
				time[type] = new float[mKeyNum];
				for (int i = 0; i < mKeyNum; i++){
					time[type][i] = model->GetFloatToken();
					model->GetToken();//3
					float x = model->GetFloatToken();
					float y = model->GetFloatToken();
					float z = model->GetFloatToken();
					Key[type][i].mM[0][0] = x;
					Key[type][i].mM[1][1] = y;
					Key[type][i].mM[2][2] = z;
				}
				break;

			case 2: //移動の行列作成
				Key[type] = new CMatrix[mKeyNum];
				time[type] = new float[mKeyNum];
				for (int i = 0; i < mKeyNum; i++){
					time[type][i] = model->GetFloatToken();
					model->GetToken();//3
					float x = model->GetFloatToken();
					float y = model->GetFloatToken();
					float z = model->GetFloatToken();
					Key[type][i].Translate(x, y, z);
				}
				break;
			case 4: //行列データの取得
				mpKey = new CAnimationKey[mKeyNum];
				for (int i = 0; i < mKeyNum; i++){
					mpKey[i].mTime = model->GetFloatToken();//Time
					model->GetToken();//16
					for (int j = 0; j < 16; j++){
						mpKey[i].mMatrix.mF[j] = model->GetFloatToken();
					}
				}
				break;
			}
			model->GetToken();
		}
		else{
			model->SkipNode();
		}
		//whileの終わり
	}
	if (mpKey == 0){
		//時間数分キーの作成
		mpKey = new CAnimationKey[mKeyNum];
		for (int i = 0; i < mKeyNum; i++){
			//時間設定
			mpKey[i].mTime = time[2][i];//Time
			//行列作成 Size *Rotation *Postion
			mpKey[i].mMatrix = Key[1][i] * Key[0][i] * Key[2][i];
		}
	}
	//確保したエリア解放
	for (int i = 0; i < ARRAY_SIZE(Key); i++){
		SAFE_DELETE_ARRAY(time[i]);
		SAFE_DELETE_ARRAY(Key[i]);
	}

#ifdef _DEBUG

	/*printf("Animation %s \n", mpFrameName);
	for (int i = 0; i < 4 * 4; i += 4){
	printf(" %f ", mpKey[0].mMatrix.mF[i]);
	printf(" %f ", mpKey[0].mMatrix.mF[i + 1]);
	printf(" %f ", mpKey[0].mMatrix.mF[i + 2]);
	printf(" %f \n", mpKey[0].mMatrix.mF[i + 3]);
	}*/

#endif
}
/*
AnimationFrame
フレームの変換行列をアニメーションデータで更新する
*/

void CModelX::AnimateFrame(){
	//アニメーションで適用されるフレームの
	//変換行列をゼロクリアする
	for (int i = 0; i < mAnimationSet.size(); i++){
		CAnimationSet*anim = mAnimationSet[i];
		//重みが0は飛ばす
		if (anim->mWeight == 0)continue;
		//フレーム分(Animation分)繰り返す
		for (int j = 0; j < anim->mAnimation.size(); j++){
			CAnimation*animation = anim->mAnimation[j];
			//該当するフレームの変換行列をゼロクリアする
			memset(&mFrame[animation->mFrameIndex]->mTransformMatrix, 0, sizeof(CMatrix));
		}
	}
	//アニメーションに該当するフレームの変換行列を
	//アニメーションのデータで設定する
	for (int i = 0; i < mAnimationSet.size(); i++){
		CAnimationSet*anim = mAnimationSet[i];
		//重みが0は飛ばす
		if (anim->mWeight == 0)continue;
		//フレーム分(Animation分)繰り返す
		for (int j = 0; j < anim->mAnimation.size(); j++){
			//フレームを取得する
			CAnimation*animation = anim->mAnimation[j];
			CModelXFrame*frame = mFrame[animation->mFrameIndex];
			//キーがない場合は飛ばす
			if (animation->mpKey == 0)continue;
			//時間を取得
			float time = anim->mTime;
			//最初の時間より小さい場合
			if (time < animation->mpKey[0].mTime){
				//変換行列を0コマ目の行列で更新
				frame->mTransformMatrix += animation->mpKey[0].mMatrix * anim->mWeight;
			}
			else if (time >= animation->mpKey[animation->mKeyNum - 1].mTime){
				//変換行列を最後の駒の行列で更新
				frame->mTransformMatrix += animation->mpKey[animation->mKeyNum - 1].mMatrix*anim->mWeight;

			}
			else{
				//時間の途中の場合
				for (int k = 1; k < animation->mKeyNum; k++){
					//変換行列を,線形補間にて更新
					if (time < animation->mpKey[k].mTime&&
						animation->mpKey[k - 1].mTime != animation->mpKey[k].mTime){
						float r = (animation->mpKey[k].mTime - time) /
							(animation->mpKey[k].mTime - animation->mpKey[k - 1].mTime);
						frame->mTransformMatrix +=
							(animation->mpKey[k - 1].mMatrix * r + animation->mpKey[k].mMatrix*(1 - r))*anim->mWeight;
						break;
					}
				}
			}
#ifdef _DEBUG

			/*	printf("Frame:%s \n", frame->mpName);
			frame->mTransformMatrix.Print();
			*/
#endif
		}
	}

}

/*
AnimateCombined
合成行列の作成
*/
void CModelXFrame::AnimateCombined(CMatrix*parend){
	//自分の変換行列に親からの変換行列をかける
	mCombinedMatrix = mTransformMatrix*(*parend);
	//子フレームの合成行列を作成する
	for (int i = 0; i < mChild.size(); i++){
		mChild[i]->AnimateCombined(&mCombinedMatrix);
	}

#ifdef _DEBUG
	/*printf("Frame:%s \n", mpName);
	mCombinedMatrix.Print();*/

#endif


}
/*
SetSkinWeightFrameIndex
スキンウェイトにフレーム番号設定
*/
void CModelX::SetSkinWeightFrameIndex(){
	//フレーム数分繰り返す
	for (int i = 0; i < mFrame.size(); i++){
		//メッシュに面があれば
		if (mFrame[i]->mMesh.mFaceNum>0){
			//スキンウェイト分繰り返し
			for (int j = 0; j < mFrame[i]->mMesh.mSkinWeights.size(); j++){
				//フレーム名のフレームを取得する
				CModelXFrame*frame = FindFrame(mFrame[i]->mMesh.mSkinWeights[j]->mpFrameName);
				//フレーム番号をせっていする
				mFrame[i]->mMesh.mSkinWeights[j]->mFrameIndex = frame->mIndex;
			}
		}
	}
}
void CModelX::AnimateVertex(){
	//フレーム数分繰り返し
	for (int i = 0; i < mFrame.size(); i++){
		//メッシュに面があれば
		if (mFrame[i]->mMesh.mFaceNum>0){
			//頂点をアニメーションで更新する
			mFrame[i]->mMesh.AnimateVertex(this);
		}
	}
}
void CModelX::AnimateVertex(CMatrix*mat){
	//フレーム数分繰り返し
	for (int i = 0; i < mFrame.size(); i++){
		//メッシュに面があれば
		if (mFrame[i]->mMesh.mFaceNum>0){
			//頂点をアニメーションで更新する
			mFrame[i]->mMesh.AnimateVertex(mat);
		}
	}
}
/*
FindMaterial
マテリアル名に該当するマテリアルを返却する
*/
CMaterial*CModelX::FindMaterial(char*name){
	//マテリアル配列のイテレータ作成
	std::vector<CMaterial*>::iterator itr;
	//マテリアル配列を先頭から順に検索
	for (itr = mMaterial.begin(); itr != mMaterial.end(); itr++){
		//名前が一致すればマテリアルのポインタを返却
		if (strcmp(name, (*itr)->mName) == 0){
			return*itr;

		}
	}
	//ないときはNULLを返却
	return NULL;
}
/*
アニメーションを抜き出す
idx:分割したいアニメーションの番号
start:分割したいアニメーションの開始時間
end:分割したいアニメーションの終了時間
name:追加するアニメーションセットの名前
*/
void CModelX::SeparateAnimationSet(int idx, int start, int end, char*name)
{
	CAnimationSet*anim = mAnimationSet[idx];//分割するアニメーションセットを確定
	CAnimationSet*as = new CAnimationSet();//アニメーションセットの生成
	as->mpName = new char[strlen(name) + 1];
	strcpy(as->mpName, name);
	as->mMaxTime = end - start;
	for (int i = 0; i < anim->mAnimation.size(); i++){//既存のアニメーション分繰り返し
		CAnimation*animtion = new CAnimation();//アニメーションの生成
		animtion->mpFrameName = new char[strlen(anim->mAnimation[i]->mpFrameName) + 1];
		strcpy(animtion->mpFrameName, anim->mAnimation[i]->mpFrameName);
		animtion->mFrameIndex = anim->mAnimation[i]->mFrameIndex;
		animtion->mKeyNum = end - start + 1;
		animtion->mpKey = new CAnimationKey[animtion->mKeyNum];//アニメーションキーの生成
		animtion->mKeyNum = 0;
		for (int j = start; j <= end&&j < anim->mAnimation[i]->mKeyNum; j++){
			animtion->mpKey[animtion->mKeyNum] = anim->mAnimation[i]->mpKey[j];
			animtion->mpKey[animtion->mKeyNum].mTime = animtion->mKeyNum++;
		}//アニメーションのコピー
		as->mAnimation.push_back(animtion);//アニメーションの追加
	}
	mAnimationSet.push_back(as);//アニメーションの追加
}
void CModelX::RenderShader(CMatrix *pCombinedMatrix){
	mShader.Render(this, pCombinedMatrix);
}
void CMesh::CreateVertexBuffer(){
	//メッシュ毎に一回作成すればいい
	if (mMyVertexBufferId > 0)
		return;
	if (mVertexNum > 0){
		//頂点インデックスを使わず、すべての面データを作成
		CVertex *pmyVertex, *vec;
		//頂点数計算
		int myVertexNum = mFaceNum * 3;
		//頂点数分頂点配列作成
		pmyVertex = new CVertex[myVertexNum];
		vec = new CVertex[mVertexNum];
		for (int j = 0; j < mVertexNum; j++){
			//頂点座標設定
			vec[j].mPosition = mpVertex[j];
			//テクスチャマッピング設定
			if (mpTextureCoords != NULL){
				vec[j].mTextureCoords.mX = mpTextureCoords[j * 2];
				vec[j].mTextureCoords.mY = mpTextureCoords[j * 2 + 1];

			}
			vec[j].mBoneWeight[0] = 1.0f;
		}
		int wi = 0;
		//スキンウェイト設定
		for (int k = 0; k < mSkinWeights.size(); k++){
			for (int l = 0; l < mSkinWeights[k]->mIndexNum; l++){
				int idx = mSkinWeights[k]->mpIndex[l];
				for (int m = 0; m < 4; m++){
					if (vec[idx].mBoneIndex[m] == 0){
						vec[idx].mBoneIndex[m] =
							mSkinWeights[k]->mFrameIndex;
						vec[idx].mBoneWeight[m] =
							mSkinWeights[k]->mpWeight[l];
						break;
					}
				}
			}
		}
		int k = 0;
		//マテリアル番号の昇順に面の頂点を設定
		for (int i = 0; i < mMaterial.size(); i++){
			int w = k;
			for (int j = 0; j < mMaterialIndexNum; j++){
				if (mpMaterilIndex[j] == i){
					//頂点配列に設定し、法線を設定する
					pmyVertex[k] = vec[mpVertexIndex[j * 3]];
					pmyVertex[k++].mNormal = mpNormal[j * 3];
					pmyVertex[k] = vec[mpVertexIndex[j * 3 + 1]];
					pmyVertex[k++].mNormal = mpNormal[j * 3 + 1];
					pmyVertex[k] = vec[mpVertexIndex[j * 3 + 2]];
					pmyVertex[k++].mNormal = mpNormal[j * 3 + 2];
				}
			}
			//マテリアル毎の頂点数を追加する
			mMaterialVertexCount.push_back(k - w);
		}
		//頂点バッファの作成
		glGenBuffers(1, &mMyVertexBufferId);
		//頂点バッファをバインド
		glBindBuffer(GL_ARRAY_BUFFER, mMyVertexBufferId);
		//バインドしたバッファにデータを転送
		glBufferData(GL_ARRAY_BUFFER, sizeof(CVertex)*myVertexNum, pmyVertex, GL_STATIC_DRAW);
		//バインド解除
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//配列解放
		delete[] pmyVertex;
		delete[] vec;
		pmyVertex = NULL;

	}
}
