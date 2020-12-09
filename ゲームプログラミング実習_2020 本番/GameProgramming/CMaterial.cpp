#include "CMaterial.h"
//memset�̃C���N���[�h
#include <string.h>
#include"glew.h"
#include "glut.h"

//�f�t�H���g�R���X�g���N�^
CMaterial::CMaterial()
: mpTexture(0), mVertexNum(0)
, mpTexterFilename(0)
{
	//���O��0�Ŗ���
	memset(mName, 0, sizeof(mName));
	//0�Ŗ��߂�
	memset(mDiffuse, 0, sizeof(mDiffuse));
}

//�}�e���A����L���ɂ���
void CMaterial::Enabled() {
	//�A���t�@�u�����h��L���ɂ���
	glEnable(GL_BLEND);
	//�u�����h���@���w��
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//�g�U���̐ݒ�
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiffuse);
	//�e�N�X�`�����������A�߂�
	if (mpTexture == 0)
		return;
	//�e�N�X�`���L��
	if (mpTexture->mId) {
		//�e�N�X�`�����g�p�\�ɂ���
		glEnable(GL_TEXTURE_2D);
		//�e�N�X�`�����o�C���h����
		glBindTexture(GL_TEXTURE_2D, mpTexture->mId);
	}
}
//�}�e���A���𖳌��ɂ���
void CMaterial::Disabled() {
	//�A���t�@�u�����h�𖳌�
	glDisable(GL_BLEND);
	//�e�N�X�`�����������A�߂�
	if (mpTexture == 0)
		return;
	//�e�N�X�`���L��
	if (mpTexture->mId) {
		//�e�N�X�`���̃o�C���h������
		glBindTexture(GL_TEXTURE_2D, 0);
		//�e�N�X�`���𖳌��ɂ���
		glDisable(GL_TEXTURE_2D);
	}
}
/*
Material�f�[�^�̓ǂݍ��݂Ɛݒ�
*/
CMaterial::CMaterial(CModelX*model)
:mpTexterFilename(0)
{
	//CModelX�Ƀ}�e���A����ǉ�����
	model->mMaterial.push_back(this);

	model->GetToken();//{?Name
	if (strcmp(model->mToken, "{") != 0){
		//{�o�Ȃ��Ƃ��̓}�e���A����
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
		//�e�N�X�`������̏ꍇ�A�e�N�X�`���t�@�C�����擾
		model->GetToken();//{
		model->GetToken();//filename
		mpTexterFilename = new char[strlen(model->mToken) + 1];
		strcpy(mpTexterFilename, model->mToken);
		//�X�}�[�g�|�C���^�̐���
		std::shared_ptr<CTexture>t(new CTexture());
		//�X�}�[�g�|�C���^�̑��
		mpTexture = t;
		//�e�N�X�`���t�@�C���̓ǂݍ���
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
