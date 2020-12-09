#include<stdio.h>
#include<string.h>
#include"CModelX.h"
#include"CMaterial.h"
#include"CVertex.h"
void CModelX::Load(char* file){
	//
	//�t�@�C���T�C�Y���擾����
	//
	FILE *fp;   //�t�@�C���|�C���^�ϐ��̍쐬
	fp = fopen(file, "rb");//�t�@�C���I�[�v��
	if (fp == NULL){       //�G���[�`�F�b�N
		printf("fopen error:%s\n", file);
		return;
	}
	//�t�@�C���̍Ō�ֈړ�
	fseek(fp, 0L, SEEK_END);
	//�t�@�C���T�C�Y�̎擾
	int size = ftell(fp);
	//�t�@�C���T�C�Y+1�o�C�g���̗̈���m��
	char *buf = mpPointer = new char[size + 1];
	//
	//�t�@�C������3D���f���̃f�[�^��ǂݍ���
	//
	//�t�@�C���̐擪�ֈړ�
	fseek(fp, 0L, SEEK_SET);
	//�m�ۂ����̈�Ƀt�@�C���T�C�Y���f�[�^��ǂݍ���
	fread(buf, size, 1, fp);
	//�Ō��\0��ݒ肷��(������̏I�[)
	buf[size] = '\0';
	fclose(fp);      //�t�@�C�����N���[�Y����

	//�_�~�[���[�g�t���[���̍쐬
	CModelXFrame*p = new CModelXFrame();
	//���O�Ȃ�
	p->mpName = new char[1];
	p->mpName[0] = '\0';
	//�t���[���z��ɒǉ�
	mFrame.push_back(p);

	//���͂����l���R���\�[���ɏo�͂���
	//printf("%s", buf);

	//������̍Ō�܂ŌJ��Ԃ�
	while (*mpPointer != '\0'){
		GetToken();   //�P��̎擾
		//template
		if (strcmp(mToken, "template") == 0){
			SkipNode();
		}
		//Material�̎�
		else if (strcmp(mToken, "Material") == 0){
			new CMaterial(this);
		}
		//GetFloatToken();
		//�P�ꂪFrame�̏ꍇ
		else if (strcmp(mToken, "Frame") == 0){
			//�t���[�����擾
			GetToken();
			if (strchr(mToken, '{')){
				//�t���[�����Ȃ��F�X�L�b�v
				SkipNode();
				GetToken();//}
			}
			else{
				//�t���[�����Ȃ����
				if (FindFrame(mToken) == 0){
					//�t���[�����쐬����
					p->mChild.push_back(new CModelXFrame(this));
				}
			}
			////�ӂ�[�ނ��쐬����
			//new CModelXFrame(this);

			//printf("%s ", mToken);//Frame���o��
			//GetToken();	         //Frame�����擾
			//printf("%s\n", mToken);   //Frame�����o��
		}
		//�P�ꂪAnimationSet�̏ꍇ
		else if (strcmp(mToken, "AnimationSet") == 0){
			new CAnimationSet(this);

		}

		//if (strcmp(mToken, "AnimationSet") == 0){
		//	printf("%s ", mToken);//Frame���o��
		//	GetToken();	         //Frame�����擾
		//	printf("%s\n", mToken);   //Frame�����o��
		//}
	}
	//�X�L���E�F�C�g�̃t���[���ԍ��ݒ�
	SetSkinWeightFrameIndex();
	//���_�o�b�t�@�̍쐬
	for (int i = 0; i < mFrame.size(); i++){
		if (mFrame[i]->mMesh.mFaceNum>0){
			mFrame[i]->mMesh.CreateVertexBuffer();
		}
	}
	//�X�L���}�g���b�N�X�̃G���A�쐬
	mpSkinningMatrix = new CMatrix[mFrame.size()];
	//�V�F�[�_�[�ǂݍ���
	mShader.Load("skinmesh.vert", "skinmesh.flag");
	SAFE_DELETE_ARRAY(buf);


}
void CModelX::GetToken(){
	char* p = mpPointer;
	char* q = mToken;
	//��()�^�u(\t)���s(\r)()\n,;"�ȊO�̕����ɂȂ�܂œǂݔ�΂�
	/*
	strchr(������,����)
	������ɕ������܂܂�Ă����,�������������ւ̃|�C���^��Ԃ�
	������Ȃ�������NULL��Ԃ�
	*/
	while (*p != '\0'&& strchr(" \t\r\n,;\"", *p))p++;
	if (*p == '{' || *p == '}'){
		//{�܂���}�Ȃ�mToken�ɑ�������̕�����
		*q++ = *p++;
	}
	else{
		//��()�^�u(\t)���s(\r)()\n,;"}�̕����ɂȂ�܂�mToken�ɑ������
		while (*p != '\0'&& !strchr(" \t\r\n,;\"}", *p))
			*q++ = *p++;

	}
	*q = '\0';		//mToken�̍Ō��\0����
	mpPointer = p;  //���̓ǂݍ��ރ|�C���g���X�V����

	//����mTokun��//�̏ꍇ�́A�R�����g�Ȃ̂ŉ��s�܂œǂݔ�΂�
	/*
	strchr(������,����)
	������1�ƕ�����2���������ꍇ,0��Ԃ�
	������1�ƕ�����2���������ꍇ,0�ȊO��Ԃ�
	*/
	if (!strcmp("//", mToken)){
		//���s�܂œǂݔ�΂�
		while (*p != '\0'&& !strchr("\r\n", *p))p++;
		//�ǂݍ��݈ʒu�̍X�V
		mpPointer = p;
		//�P����擾����(�ċN�Ăяo��)
		GetToken();
	}


}
void CModelX::SkipNode(){
	//�������I�������I��
	while (*mpPointer != '\0'){
		GetToken();  //���̒P��擾
		//{�����������烋�[�v�I��
		if (strchr(mToken, '{')) break;
	}
	int count = 1;
	//�������I��邩�A�J�E���g��0�ɂȂ�����I��
	while (*mpPointer != '\0'&&count > 0){
		GetToken();    //���̒P��擾
		//{��������ƃJ�E���g�A�b�v
		if (strchr(mToken, '{'))count++;
		//}��������ƃJ�E���g�_�E��
		else if (strchr(mToken, '}'))count--;
	}
}
/*
CmodelXFrame
model:CModelX�C���X�^���X
�t���[�����쐬
�ǂݍ��ݒ���Frame��������΁A�t���[�����쐬��
�q�t���[���ɒǉ�����
*/
CModelXFrame::CModelXFrame(CModelX*model){
	//���݂̃t���[���z��̗v�f�����擾���ݒ肷��
	mIndex = model->mFrame.size();
	//CModelX�̃t���[���z��ɒǉ�����
	model->mFrame.push_back(this);
	//�ϊ��s���P�ʍs��ɂ���
	mTransformMatrix.Identity();
	//���̒P��(�t���[�����̗\��)���擾����
	//�폜	model->GetToken();//frame name
	//�t���[�������G���A���m�ۂ���
	mpName = new char[strlen(model->mToken) + 1];
	//�t���[�������R�s�[����
	strcpy(mpName, model->mToken);
	//���̒P��({�̗\��)���擾����
	model->GetToken();//{
	//�������Ȃ��Ȃ����I���
	while (*model->mpPointer != '\0'){
		//���̒P��擾
		model->GetToken();//Frame
		//}�̏ꍇ�͏I��
		if (strchr(model->mToken, '}'))
			break;
		//�V�ȃt���[���̏ꍇ�͎q�t���[���ɒǉ�
		if (strcmp(model->mToken, "Frame") == 0){
			model->GetToken();
			if (strchr(model->mToken, '{')){
				//�t���[�����Ȃ��F�X�L�b�v
				model->SkipNode();
				model->GetToken();//}
			}
			else{
				//�t���[�����Ȃ����
				if (model->FindFrame(model->mToken) == 0){
					//�ӂ�[�ނ��쐬���A�q�t���[���̔z��ɒǉ�
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
			//��L�ȊO�̗v�f�͓ǂݔ�΂�
			model->SkipNode();
		}
	}
	//�f�o�b�O�o�[�W�����̂ݗL��
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
	//�������float�^�֕ϊ�
	return atof(mToken);


}
/*
GetIntToken
�P��𐮐��^�̃f�[�^�œn��
*/

int CModelX::GetIntToken(){
	GetToken();
	return atoi(mToken);
}
void CMesh::Init(CModelX*model){

	model->GetToken();   //{or���O
	if (!strchr(model->mToken, '{')){
		//���O�̏ꍇ,����{
		model->GetToken();  //{
	}
	//���_���̎擾
	mVertexNum = model->GetIntToken();
	//���_�����G���A�m��
	mpVertex = new CVector[mVertexNum];
	mpAnimateVertex = new CVector[mVertexNum];


	//���_�����f�[�^����荞��
	for (int i = 0; i < mVertexNum; i++){
		mpVertex[i].mX = model->GetFloatToken();
		mpVertex[i].mY = model->GetFloatToken();
		mpVertex[i].mZ = model->GetFloatToken();
	}
	mFaceNum = model->GetIntToken();    //�摜�ǂݍ���
	//���_����,1���3���_
	mpVertexIndex = new int[mFaceNum * 3];
	for (int i = 0; i < mFaceNum * 3; i += 3){
		model->GetIntToken();//���_���ǂݍ���
		mpVertexIndex[i] = model->GetIntToken();
		mpVertexIndex[i + 1] = model->GetIntToken();
		mpVertexIndex[i + 2] = model->GetIntToken();
	}
	//�������Ȃ��Ȃ�����I���
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
			//�@����=�ʐ�*3
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
			//Material�̂���
			mMaterialNum = model->GetIntToken();
			//FaceNum
			mMaterialIndexNum = model->GetIntToken();
			//�}�e���A���C���f�b�N�X�̍쐬
			mpMaterilIndex = new int[mMaterialIndexNum];
			for (int i = 0; i < mMaterialIndexNum; i++){
				mpMaterilIndex[i] = model->GetIntToken();
			}
			//�}�e���A���f�[�^�̍쐬
			for (int i = 0; i < mMaterialNum; i++){
				model->GetToken();     //Material
				if (strcmp(model->mToken, "Material") == 0){
					mMaterial.push_back(new CMaterial(model));
				}
				else{
					//{���o
					model->GetToken();    //MaterilName

					mMaterial.push_back(model->FindMaterial(model->mToken));
					model->GetToken();  //}
				}

			}
			model->GetToken();    //}//End of MeshMaterialList
		}

		else if (strcmp(model->mToken, "SkinWeights") == 0){
			//SKinWeights�N���X�̃C���X�^���X���쐬,�z��ɒǉ�
			mSkinWeights.push_back(new CSkinWeights(model));
		}
		//�e�N�X�`�����W�̎�
		else if (strcmp(model->mToken, "MeshTextureCoords") == 0){
			model->GetToken();  //{
			//�e�N�X�`�����W�����擾
			int textureCoodsNum = model->GetIntToken() * 2;
			//�e�N�X�`�����W�̃f�[�^��z��Ɏ�荞��
			mpTextureCoords = new float[textureCoodsNum];
			for (int i = 0; i < textureCoodsNum; i++){
				mpTextureCoords[i] = model->GetFloatToken();

			}
			model->GetToken();   //}
		}
		else{
			//�ȊO�̃m�[�h�͓ǂݔ�΂�
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
AnimateVertex���_�ɃA�j���[�V������K�p
*/
void CMesh::AnimateVertex(CModelX*model){
	//�A�j���[�V�����p�̒��_�G���A�N���A
	memset(mpAnimateVertex, 0, sizeof(CVector)*mVertexNum);
	memset(mpAnimateNormal, 0, sizeof(CVector)*mNormalNum);
	//�X�L���E�F�C�g���J��Ԃ�
	for (int i = 0; i < mSkinWeights.size(); i++){
		//�t���[���ԍ��擾
		int frameIndex = mSkinWeights[i]->mFrameIndex;
		//�I�t�Z�b�g�s��ƃt���[�������s�������
		CMatrix mSkinningMatrix = mSkinWeights[i]->mOffset*model->mFrame[frameIndex]->mCombinedMatrix;
		//���_�����J��Ԃ�
		for (int j = 0; j < mSkinWeights[i]->mIndexNum; j++){
			//���_�ԍ��擾
			int index = mSkinWeights[i]->mpIndex[j];
			//�d�ݎ擾
			float weight = mSkinWeights[i]->mpWeight[j];
			//���_�Ɩ@�����X�V����
			mpAnimateVertex[index] += mpVertex[index] * mSkinningMatrix*weight;
			mpAnimateNormal[index] += mpNormal[index] * mSkinningMatrix*weight;
		}
	}
	for (int i = 0; i < mNormalNum; i++){
		mpAnimateNormal[i] = mpAnimateNormal[i].Normalize();
	}
}
void CMesh::AnimateVertex(CMatrix*mat){
	//�A�j���[�V�����p�̒��_�G���A�N���A
	memset(mpAnimateVertex, 0, sizeof(CVector)*mVertexNum);
	memset(mpAnimateNormal, 0, sizeof(CVector)*mNormalNum);
	//�X�L���E�F�C�g���J��Ԃ�
	for (int i = 0; i < mSkinWeights.size(); i++){
		//�t���[���ԍ��擾
		int frameIndex = mSkinWeights[i]->mFrameIndex;
		//�I�t�Z�b�g�s��ƃt���[�������s�������
		CMatrix mSkinningMatrix = mSkinWeights[i]->mOffset * mat[frameIndex];
		//���_�����J��Ԃ�
		for (int j = 0; j < mSkinWeights[i]->mIndexNum; j++){
			//���_�ԍ��擾
			int index = mSkinWeights[i]->mpIndex[j];
			//�d�ݎ擾
			float weight = mSkinWeights[i]->mpWeight[j];
			//���_�Ɩ@�����X�V����
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
��ʂɕ`��
*/
void CMesh::Render(){



	/*���_�f�[�^,�@���f�[�^�̔z���L���ɂ���*/
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//�e�N�X�`���܂��҂�O�̔z���L���ɂ���
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	/*���_�f�[�^,�@���f�[�^�̏ꏊ���w�肷��*/
	glVertexPointer(3, GL_FLOAT, 0, mpAnimateVertex);
	glNormalPointer(GL_FLOAT, 0, mpAnimateNormal);
	glTexCoordPointer(2, GL_FLOAT, 0, mpTextureCoords);
	/*���_�̃C���f�b�N�X�̏ꏊ���w�肵�Đ}�`��`�悷��*/
	for (int i = 0; i < mFaceNum; i++){
		//�}�e���A����K�p
		mMaterial[mpMaterilIndex[i]]->Enabled();
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (mpVertexIndex + i * 3));
		mMaterial[mpMaterilIndex[i]]->Disabled();
	}

	///*���_�̃C���f�b�N�X�̏ꏊ���w�肵�Đ}�`��`�悷��*/
	//glDrawElements(GL_TRIANGLES, 3 * mFaceNum, 
	//	GL_UNSIGNED_INT, mpVertexIndex);
	/*���_�f�[�^,�@���f�[�^�̔z��𖳌��ɂ���*/
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);


}
/*
Rnder
���b�V���̉搔��0�ȊO�Ȃ�`�悷��
*/
void CModelXFrame::Render(){
	if (mMesh.mFaceNum != 0)
		mMesh.Render();
}
/*
Render
�S�Ẵt���[���̕`�揈�����ĂыN����
*/
void CModelX::Render(){
	for (int i = 0; i < mFrame.size(); i++){
		mFrame[i]->Render();
	}
}

/*
CSkinWeights
�X�L���E�F�C�g�N���X
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
	//�t���[�����G���A�m��,�ݒ�
	mpFrameName = new char[strlen(model->mToken) + 1];
	strcpy(mpFrameName, model->mToken);
	//���_�ԍ��擾
	mIndexNum = model->GetIntToken();
	//���_�ԍ�����0�𒴂���
	if (mIndexNum > 0){
		//���_�ԍ��ƒ��_�E�F�C�g�̃G���A�m��
		mpIndex = new int[mIndexNum];
		mpWeight = new float[mIndexNum];
		//���_�ԍ��擾
		for (int i = 0; i < mIndexNum; i++){
			mpIndex[i] = model->GetIntToken();
		}
		//���_�E�F�C�g�擾
		for (int i = 0; i < mIndexNum; i++){
			mpWeight[i] = model->GetFloatToken();
		}
	}
	//�I�t�Z�b�g�s��擾
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
	//�A�j���[�V��������ޔ�
	mpName = new char[strlen(model->mToken) + 1];
	strcpy(mpName, model->mToken);
	model->GetToken(); //{
	while (*model->mpPointer != '\0'){
		model->GetToken();//}or Animation
		if (strchr(model->mToken, '}'))break;
		if (strcmp(model->mToken, "Animation") == 0){
			//�Ƃ肠�����ǂݔ�΂�
			//model->SkipNode();
			//Animation�v�f
			mAnimation.push_back(new CAnimation(model));

		}
	}
	//�I�����Ԑݒ�
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

	//�L�[�̔z���ۑ����Ă����z��
	CMatrix*Key[4] = { 0, 0, 0, 0 };
	//���Ԃ̔z���ۑ����Ă����z��
	float*time[4] = { 0, 0, 0, 0 };
	while (*model->mpPointer != '\0'){
		model->GetToken();  //} or AnimationKey
		if (strchr(model->mToken, '}'))break;
		if (strcmp(model->mToken, "AnimationKey") == 0){
			model->GetToken();//{
			//�f�[�^�̃^�C�v�擾
			int type = model->GetIntToken();
			//���Ԑ��擾
			mKeyNum = model->GetIntToken();
			switch (type){
			case 0://Rotation Quaternion
				//�s��̔z������Ԑ����m��
				Key[type] = new CMatrix[mKeyNum];
				//���Ԃ̔z������Ԑ����m��
				time[type] = new float[mKeyNum];
				//���Ԑ����J��Ԃ�
				for (int i = 0; i < mKeyNum; i++){
					//���Ԏ擾
					time[type][i] = model->GetFloatToken();
					model->GetToken();//4��ǂݔ�΂�
					//w,x,y,z���擾
					float w = model->GetFloatToken();
					float x = model->GetFloatToken();
					float y = model->GetFloatToken();
					float z = model->GetFloatToken();
					//�N�I�[�^�j�I�������]���ɕϊ�
					Key[type][i].SetQuaternion(x, y, z, w);
				}
				break;

			case 1://�g��E�k���̍s��쐬
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

			case 2: //�ړ��̍s��쐬
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
			case 4: //�s��f�[�^�̎擾
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
		//while�̏I���
	}
	if (mpKey == 0){
		//���Ԑ����L�[�̍쐬
		mpKey = new CAnimationKey[mKeyNum];
		for (int i = 0; i < mKeyNum; i++){
			//���Ԑݒ�
			mpKey[i].mTime = time[2][i];//Time
			//�s��쐬 Size *Rotation *Postion
			mpKey[i].mMatrix = Key[1][i] * Key[0][i] * Key[2][i];
		}
	}
	//�m�ۂ����G���A���
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
�t���[���̕ϊ��s����A�j���[�V�����f�[�^�ōX�V����
*/

void CModelX::AnimateFrame(){
	//�A�j���[�V�����œK�p�����t���[����
	//�ϊ��s����[���N���A����
	for (int i = 0; i < mAnimationSet.size(); i++){
		CAnimationSet*anim = mAnimationSet[i];
		//�d�݂�0�͔�΂�
		if (anim->mWeight == 0)continue;
		//�t���[����(Animation��)�J��Ԃ�
		for (int j = 0; j < anim->mAnimation.size(); j++){
			CAnimation*animation = anim->mAnimation[j];
			//�Y������t���[���̕ϊ��s����[���N���A����
			memset(&mFrame[animation->mFrameIndex]->mTransformMatrix, 0, sizeof(CMatrix));
		}
	}
	//�A�j���[�V�����ɊY������t���[���̕ϊ��s���
	//�A�j���[�V�����̃f�[�^�Őݒ肷��
	for (int i = 0; i < mAnimationSet.size(); i++){
		CAnimationSet*anim = mAnimationSet[i];
		//�d�݂�0�͔�΂�
		if (anim->mWeight == 0)continue;
		//�t���[����(Animation��)�J��Ԃ�
		for (int j = 0; j < anim->mAnimation.size(); j++){
			//�t���[�����擾����
			CAnimation*animation = anim->mAnimation[j];
			CModelXFrame*frame = mFrame[animation->mFrameIndex];
			//�L�[���Ȃ��ꍇ�͔�΂�
			if (animation->mpKey == 0)continue;
			//���Ԃ��擾
			float time = anim->mTime;
			//�ŏ��̎��Ԃ�菬�����ꍇ
			if (time < animation->mpKey[0].mTime){
				//�ϊ��s���0�R�}�ڂ̍s��ōX�V
				frame->mTransformMatrix += animation->mpKey[0].mMatrix * anim->mWeight;
			}
			else if (time >= animation->mpKey[animation->mKeyNum - 1].mTime){
				//�ϊ��s����Ō�̋�̍s��ōX�V
				frame->mTransformMatrix += animation->mpKey[animation->mKeyNum - 1].mMatrix*anim->mWeight;

			}
			else{
				//���Ԃ̓r���̏ꍇ
				for (int k = 1; k < animation->mKeyNum; k++){
					//�ϊ��s���,���`��ԂɂčX�V
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
�����s��̍쐬
*/
void CModelXFrame::AnimateCombined(CMatrix*parend){
	//�����̕ϊ��s��ɐe����̕ϊ��s���������
	mCombinedMatrix = mTransformMatrix*(*parend);
	//�q�t���[���̍����s����쐬����
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
�X�L���E�F�C�g�Ƀt���[���ԍ��ݒ�
*/
void CModelX::SetSkinWeightFrameIndex(){
	//�t���[�������J��Ԃ�
	for (int i = 0; i < mFrame.size(); i++){
		//���b�V���ɖʂ������
		if (mFrame[i]->mMesh.mFaceNum>0){
			//�X�L���E�F�C�g���J��Ԃ�
			for (int j = 0; j < mFrame[i]->mMesh.mSkinWeights.size(); j++){
				//�t���[�����̃t���[�����擾����
				CModelXFrame*frame = FindFrame(mFrame[i]->mMesh.mSkinWeights[j]->mpFrameName);
				//�t���[���ԍ��������Ă�����
				mFrame[i]->mMesh.mSkinWeights[j]->mFrameIndex = frame->mIndex;
			}
		}
	}
}
void CModelX::AnimateVertex(){
	//�t���[�������J��Ԃ�
	for (int i = 0; i < mFrame.size(); i++){
		//���b�V���ɖʂ������
		if (mFrame[i]->mMesh.mFaceNum>0){
			//���_���A�j���[�V�����ōX�V����
			mFrame[i]->mMesh.AnimateVertex(this);
		}
	}
}
void CModelX::AnimateVertex(CMatrix*mat){
	//�t���[�������J��Ԃ�
	for (int i = 0; i < mFrame.size(); i++){
		//���b�V���ɖʂ������
		if (mFrame[i]->mMesh.mFaceNum>0){
			//���_���A�j���[�V�����ōX�V����
			mFrame[i]->mMesh.AnimateVertex(mat);
		}
	}
}
/*
FindMaterial
�}�e���A�����ɊY������}�e���A����ԋp����
*/
CMaterial*CModelX::FindMaterial(char*name){
	//�}�e���A���z��̃C�e���[�^�쐬
	std::vector<CMaterial*>::iterator itr;
	//�}�e���A���z���擪���珇�Ɍ���
	for (itr = mMaterial.begin(); itr != mMaterial.end(); itr++){
		//���O����v����΃}�e���A���̃|�C���^��ԋp
		if (strcmp(name, (*itr)->mName) == 0){
			return*itr;

		}
	}
	//�Ȃ��Ƃ���NULL��ԋp
	return NULL;
}
/*
�A�j���[�V�����𔲂��o��
idx:�����������A�j���[�V�����̔ԍ�
start:�����������A�j���[�V�����̊J�n����
end:�����������A�j���[�V�����̏I������
name:�ǉ�����A�j���[�V�����Z�b�g�̖��O
*/
void CModelX::SeparateAnimationSet(int idx, int start, int end, char*name)
{
	CAnimationSet*anim = mAnimationSet[idx];//��������A�j���[�V�����Z�b�g���m��
	CAnimationSet*as = new CAnimationSet();//�A�j���[�V�����Z�b�g�̐���
	as->mpName = new char[strlen(name) + 1];
	strcpy(as->mpName, name);
	as->mMaxTime = end - start;
	for (int i = 0; i < anim->mAnimation.size(); i++){//�����̃A�j���[�V�������J��Ԃ�
		CAnimation*animtion = new CAnimation();//�A�j���[�V�����̐���
		animtion->mpFrameName = new char[strlen(anim->mAnimation[i]->mpFrameName) + 1];
		strcpy(animtion->mpFrameName, anim->mAnimation[i]->mpFrameName);
		animtion->mFrameIndex = anim->mAnimation[i]->mFrameIndex;
		animtion->mKeyNum = end - start + 1;
		animtion->mpKey = new CAnimationKey[animtion->mKeyNum];//�A�j���[�V�����L�[�̐���
		animtion->mKeyNum = 0;
		for (int j = start; j <= end&&j < anim->mAnimation[i]->mKeyNum; j++){
			animtion->mpKey[animtion->mKeyNum] = anim->mAnimation[i]->mpKey[j];
			animtion->mpKey[animtion->mKeyNum].mTime = animtion->mKeyNum++;
		}//�A�j���[�V�����̃R�s�[
		as->mAnimation.push_back(animtion);//�A�j���[�V�����̒ǉ�
	}
	mAnimationSet.push_back(as);//�A�j���[�V�����̒ǉ�
}
void CModelX::RenderShader(CMatrix *pCombinedMatrix){
	mShader.Render(this, pCombinedMatrix);
}
void CMesh::CreateVertexBuffer(){
	//���b�V�����Ɉ��쐬����΂���
	if (mMyVertexBufferId > 0)
		return;
	if (mVertexNum > 0){
		//���_�C���f�b�N�X���g�킸�A���ׂĂ̖ʃf�[�^���쐬
		CVertex *pmyVertex, *vec;
		//���_���v�Z
		int myVertexNum = mFaceNum * 3;
		//���_�������_�z��쐬
		pmyVertex = new CVertex[myVertexNum];
		vec = new CVertex[mVertexNum];
		for (int j = 0; j < mVertexNum; j++){
			//���_���W�ݒ�
			vec[j].mPosition = mpVertex[j];
			//�e�N�X�`���}�b�s���O�ݒ�
			if (mpTextureCoords != NULL){
				vec[j].mTextureCoords.mX = mpTextureCoords[j * 2];
				vec[j].mTextureCoords.mY = mpTextureCoords[j * 2 + 1];

			}
			vec[j].mBoneWeight[0] = 1.0f;
		}
		int wi = 0;
		//�X�L���E�F�C�g�ݒ�
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
		//�}�e���A���ԍ��̏����ɖʂ̒��_��ݒ�
		for (int i = 0; i < mMaterial.size(); i++){
			int w = k;
			for (int j = 0; j < mMaterialIndexNum; j++){
				if (mpMaterilIndex[j] == i){
					//���_�z��ɐݒ肵�A�@����ݒ肷��
					pmyVertex[k] = vec[mpVertexIndex[j * 3]];
					pmyVertex[k++].mNormal = mpNormal[j * 3];
					pmyVertex[k] = vec[mpVertexIndex[j * 3 + 1]];
					pmyVertex[k++].mNormal = mpNormal[j * 3 + 1];
					pmyVertex[k] = vec[mpVertexIndex[j * 3 + 2]];
					pmyVertex[k++].mNormal = mpNormal[j * 3 + 2];
				}
			}
			//�}�e���A�����̒��_����ǉ�����
			mMaterialVertexCount.push_back(k - w);
		}
		//���_�o�b�t�@�̍쐬
		glGenBuffers(1, &mMyVertexBufferId);
		//���_�o�b�t�@���o�C���h
		glBindBuffer(GL_ARRAY_BUFFER, mMyVertexBufferId);
		//�o�C���h�����o�b�t�@�Ƀf�[�^��]��
		glBufferData(GL_ARRAY_BUFFER, sizeof(CVertex)*myVertexNum, pmyVertex, GL_STATIC_DRAW);
		//�o�C���h����
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//�z����
		delete[] pmyVertex;
		delete[] vec;
		pmyVertex = NULL;

	}
}
