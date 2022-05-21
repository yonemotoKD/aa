//=====================================================
//
// �S�V�F�[�_����
//
//=====================================================

//------------------------------
// �萔�o�b�t�@(�J����)
//------------------------------
cbuffer cbCamera : register(b7)
{
	// �J�������
	row_major float4x4  g_mView;	// �r���[�ϊ��s��
	row_major float4x4  g_mProj;	// �ˉe�ϊ��s��
	row_major float4x4  g_mProjInv;	// �ˉe�ϊ��s��F�t�s��
	float3              g_CamPos;	// �J�������W

	// �t�H�O
	int     g_DistanceFogEnable;	// �t�H�O�L��/�����t���O
	float3  g_DistanceFogColor;		// �t�H�O�F
	float   g_DistanceFogDensity;	// �t�H�O������
};

//------------------------------
// �萔�o�b�t�@(���C�g)
//------------------------------
cbuffer cbLight : register(b8)
{
	// ����
	float4  g_AmbientLight;

	// ���s��
	float3  g_DL_Dir;    // ���̕���
	float3  g_DL_Color;  // ���̐F
	row_major float4x4  g_DL_mLightVP;  // �r���[�s�� x �ˉe�s��
	
	//--------------
	// �_��
	//--------------
	// �g�p��
	int4			g_PointLightNum;   // �p�b�L���O�K������ςȂ̂ŁAint4��x�݂̂��g�p����
	// �f�[�^
	struct PointLight 
	{
	    float3	    Color;			// �F
	    float	    Radius;			// ���a
	    float3	    Pos;			// ���W
	    int			IsBright;		// ���x�p���C�g���ǂ���
	} g_PointLights[100];
};