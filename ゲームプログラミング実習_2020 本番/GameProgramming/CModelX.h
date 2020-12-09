#ifndef CMODELX_H  //�C���N���[�h�K�[�h
#define CMODELX_H
#define MODEL_FILE "���O�i.x"//���̓t�@�C����
#define MODEL_FILE2 "knight//knight_low.x"
//�̈������}�N����
#define SAFE_DELETE_ARRAY(a){if(a)delete[]a;a=0;}
//�z��̃T�C�Y�擾���}�N����
#define ARRAY_SIZE(a)(sizeof(a)/sizeof(a[0]))
#include"CMyShader.h"//�V�F�[�_�[�̃C���N���[�h

#include <vector>
#include"CMatrix.h"
#include"CVector.h"

class CModelX;

class CMaterial;//�N���X�̐錾

/*
CAnimationKey
�A�j���[�V�����L�[�N���X
*/
class CAnimationKey{
public:
	//����
	float mTime;
	//�s��
	CMatrix mMatrix;

};
/*
CAnimation
*/
class CAnimation{
public:
	char*mpFrameName;//�t���[����
	int mFrameIndex; //�t���[���ԍ�
	int mKeyNum;  //�L�[��(���Ԑ�)
	CAnimationKey*mpKey; //�L�[�z��
	CAnimation();
	CAnimation(CModelX *model);

	~CAnimation(){
		SAFE_DELETE_ARRAY(mpFrameName);
	}
};
/*
CAnimationSet
�A�j���[�V����
*/
class CAnimationSet{
public:
	//�A�j���[�V����
	char *mpName;
	float mTime;       //���ݎ���
	float mWeight;     //�d��
	float mMaxTime;    //�ő厞��
	CAnimationSet();
	CAnimationSet(CModelX *model);
	std::vector<CAnimation*>mAnimation;
	~CAnimationSet(){
		SAFE_DELETE_ARRAY(mpName);
		//�A�j���[�V�����v�f�̍폜
		for (int i = 0; i < mAnimation.size(); i++){
			delete mAnimation[i];
		}
	}
};
/*
CSkinWeights
�X�L���E�F�C�g�N���X
*/
class CSkinWeights {
public:
	char *mpFrameName;   //�t���[����
	int mFrameIndex;  //�t���[���ԍ�
	int mIndexNum;    //���_�ԍ���
	int *mpIndex;    //���_�ԍ��z��
	float *mpWeight;
	CMatrix mOffset; //�I�t�Z�b�g�}�g���b�N�X

	CSkinWeights(CModelX *model);

	~CSkinWeights(){
		SAFE_DELETE_ARRAY(mpFrameName);
		SAFE_DELETE_ARRAY(mpIndex);
		SAFE_DELETE_ARRAY(mpWeight);
	}
};
//CMesh�N���X�̒�`
class CMesh{
public:

	int mVertexNum;    //���_��
	CVector*mpVertex;  //���_�f�[�^
	int mFaceNum;      //�摜
	int *mpVertexIndex;//����\������
	int mNormalNum;
	//�e�N�X�`�����W�f�[�^
	float*mpTextureCoords;
	CVector *mpNormal;

	CVector*mpAnimateVertex;  //�A�j���[�V�����p���_
	CVector*mpAnimateNormal;  //�A�j���[�V�����p�@��


	int mMaterialNum;  //�}�e���A����
	int mMaterialIndexNum;//�}�e���A���ԍ���(�ʐ�)
	int *mpMaterilIndex;  //�}�e���A���ԍ�
	std::vector<CMaterial*>mMaterial;//�}�e���A���f�[�^
	std::vector<CSkinWeights*>mSkinWeights;
	//�}�e���A�����̖ʐ�
	std::vector<int>mMaterialVertexCount;
	//���_�o�b�t�@���ʂ�
	GLuint mMyVertexBufferId;
	//���_�o�b�t�@�̍쐬
	void CreateVertexBuffer();
	//�R���X�g���N�^
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
	//�f�X�g���N�^
	~CMesh(){

		SAFE_DELETE_ARRAY(mpVertex);
		SAFE_DELETE_ARRAY(mpVertexIndex);
		SAFE_DELETE_ARRAY(mpNormal);
		SAFE_DELETE_ARRAY(mpMaterilIndex);
		SAFE_DELETE_ARRAY(mpAnimateVertex);
		SAFE_DELETE_ARRAY(mpAnimateNormal);
		SAFE_DELETE_ARRAY(mpTextureCoords);
		//�X�L���E�F�C�g�̍폜
		for (int i = 0; i < mSkinWeights.size(); i++){
			delete mSkinWeights[i];
		}


	}

	//�ǂݍ��ݏ���
	void Init(CModelX*model);
	//���_�ɃA�j���[�V�����K�p
	void AnimateVertex(CModelX*model);
	void AnimateVertex(CMatrix*);
	void Render();

};
/*
CModelX
X�t�@�C���`����3D���f���f�[�^���v���O�����ŔF������
*/
//CModelXFrame�N���X�̒�`
class CModelXFrame{
public:

	std::vector<CModelXFrame*>mChild; //�q�t���[��
	CMatrix mTransformMatrix;  //�ϊ��s��
	CMatrix mCombinedMatrix;   //�����s��
	CMesh mMesh;    //Mesh�f�[�^
	char*mpName;
	int mIndex;

	//�R���X�g���N�^
	CModelXFrame(CModelX*model);
	CModelXFrame()
		:mpName(0)
		, mIndex(0){}

	//�f�X�g���N�^
	~CModelXFrame(){
		//�q�t���[�������ׂĊJ������
		std::vector<CModelXFrame*>::iterator itr;
		for (itr = mChild.begin(); itr != mChild.end(); itr++){
			delete*itr;
		}
		//���O�̃G���A���J������
		SAFE_DELETE_ARRAY(mpName);

	}
	void AnimateCombined(CMatrix*parent);
	void Render();
};

class CModelX{
public:

	int GetIntToken();
	CMatrix mTransformMatrix;  //�ϊ��s��
	//�V�F�[�_�[�p�̃X�L���}�g���b�N�X
	CMatrix *mpSkinningMatrix;
	CMyShader mShader;//�V�F�[�_�[�̃C���X�^���X

	char* mpPointer;
	char mToken[1024];

	//���������_
	float GetFloatToken();
	std::vector<CModelXFrame*>mFrame;  //�t���[���̔z��
	std::vector<CAnimationSet*>mAnimationSet;
	std::vector<CMaterial*>mMaterial;  //�}�e���A���̔z��
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

	//�t�@�C���ǂݍ���
	void Load(char*file);
	void GetToken();
	void AnimateFrame();
	void SetSkinWeightFrameIndex();
	void AnimateVertex();
	void AnimateVertex(CMatrix*);
	void SeparateAnimationSet(int idx, int start, int end, char*name);
	//�m�[�h�̓ǂݍ���
	void SkipNode();
	void Render();
	//�V�F�[�_�[���g�����`��
	void RenderShader(CMatrix*m);

	//�}�e���A���̌���
	CMaterial*FindMaterial(char*name);

	//�P�ʍs��̍쐬
	CMatrix Identity();
	/*
	FindFrame
	�t���[�����ɊY������t���[���̃A�h���X��Ԃ�
	*/
	CModelXFrame*CModelX::FindFrame(char*name){
		//�C�e���[�^�̍쐬
		std::vector<CModelXFrame*>::iterator itr;
		//�擪����Ō�܂ŌJ��Ԃ�
		for (itr = mFrame.begin(); itr != mFrame.end(); itr++){
			//���O����v�������H
			if (strcmp(name, (*itr)->mpName) == 0){
				//��v�����炻�̃A�h���X��Ԃ�
				return *itr;
			}
		}
		//��v����t���[�������ꍇNULL��Ԃ�
		return NULL;
	}

};

#endif
