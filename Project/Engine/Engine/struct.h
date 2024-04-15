#pragma once


struct tEventInfo
{
	EVENT_TYPE eType;
	DWORD_PTR  lParam;
	DWORD_PTR  wParam;
};


struct Vertex
{
	Vec3 vPos;		// 좌표	
	Vec4 vColor;	// 색상
	Vec2 vUV;

	Vec3 vTangent;	// 접선
	Vec3 vNormal;	// 법선
	Vec3 vBinormal; // 종법선

	// 3DAnimation
	Vec4 vWeights;
	Vec4 vIndices;
};

typedef Vertex Vtx;


// =======
// TileMap
// =======
struct tTileData
{
	int  iImgIdx;
	Vec2 vLTUV;
	int  iPadding;

	tTileData()
		: iImgIdx(-1)
		, iPadding(0) {}
};


// ========
// Material
// ========
struct tMtrlData
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
	Vec4 vEmv;
};


// ===========
// Animtion 2D
// ===========
struct tAnim2DFrame
{
	Vec2  vLT;
	Vec2  vSlice;
	Vec2  vOffset;
	float fDuration;
};


// ============
// Animation 3D
// ============
struct tFrameTrans
{
	Vec4 vTranslate;
	Vec4 vScale;
	Vec4 qRot;
};

struct tMTKeyFrame
{
	double dTime;
	int    iFrame;
	Vec3   vTranslate;
	Vec3   vScale;
	Vec4   qRot;
};


struct tMTBone
{
	wstring             strBoneName;
	int                 iDepth;
	int                 iParentIndx;
	Matrix              matOffset;	// Offset 행렬(뼈 -> 루트 까지의 행렬)
	Matrix              matBone;   // 이거 안씀
	vector<tMTKeyFrame> vecKeyFrame;
};

struct tMTAnimClip
{
	wstring strAnimName;
	int     iStartFrame;
	int     iEndFrame;
	int     iFrameLength;

	double dStartTime;
	double dEndTime;
	double dTimeLength;
	float  fUpdateTime; // 이거 안씀

	FbxTime::EMode eMode;
};


//===============
// Struct of FBX 
//===============
struct tFbxMaterial
{
	tMtrlData tMtrl;
	wstring   strMtrlName;
	wstring   strDiff;
	wstring   strNormal;
	wstring   strSpec;
};

struct tWeightsAndIndices
{
	int    iBoneIdx;
	double dWeight;
};

struct tContainer
{
	wstring      strName;
	vector<Vec3> vecPos;
	vector<Vec3> vecTangent;
	vector<Vec3> vecBinormal;
	vector<Vec3> vecNormal;
	vector<Vec2> vecUV;

	vector<Vec4> vecIndices;
	vector<Vec4> vecWeights;

	vector<vector<UINT>> vecIdx;
	vector<tFbxMaterial> vecMtrl;

	// Animation 관련 정보
	bool                               bAnimation;
	vector<vector<tWeightsAndIndices>> vecWI;

	void Resize(UINT _iSize)
	{
		vecPos.resize(_iSize);
		vecTangent.resize(_iSize);
		vecBinormal.resize(_iSize);
		vecNormal.resize(_iSize);
		vecUV.resize(_iSize);
		vecIndices.resize(_iSize);
		vecWeights.resize(_iSize);
		vecWI.resize(_iSize);
	}
};

struct tKeyFrame
{
	FbxAMatrix matTransform;
	double     dTime;
};

struct tBone
{
	wstring           strBoneName;
	int               iDepth;			// 계층구조 깊이
	int               iParentIndx;		// 부모 Bone 의 인덱스
	FbxAMatrix        matOffset;		// Offset 행렬( -> 뿌리 -> Local)
	FbxAMatrix        matBone;
	vector<tKeyFrame> vecKeyFrame;
};

struct tAnimClip
{
	wstring        strName;
	FbxTime        tStartTime;
	FbxTime        tEndTime;
	FbxLongLong    llTimeLength;
	FbxTime::EMode eMode;
};


// ========
// Particle 
// ========
struct tParticle
{
	Vec3 vPos;
	Vec3 vScale;
	Vec3 vDir;
	Vec4 vColor;

	int   Alive;
	float m_fCurTime; // 생존 시간
	float m_fMaxTime; // 최대 생명 시간
};

struct tParticleData
{
	int  iAliveCount;
	Vec3 vPadding;
};


struct tLightColor
{
	Vec4 vDiff;	// Diffuse Light	난반사광
	Vec4 vSpec; // Specular Light	반사광
	Vec4 vAmb;  // Ambient Light	환경광
};

struct tLightInfo
{
	tLightColor color;
	Vec3        vLightDir;
	int         iLightType;
	Vec3        vWorldPos;
	float       fAngle;
	float       fRange;
	Vec3        vPadding;
};

// 광선 구조체
struct tRay
{
	Vec3 vStart;
	Vec3 vDir;
};

// Raycast 결과를 받을 구조체
struct tRaycastOut
{
	Vec2 vUV;
	int  iDist;
	int  bSuccess;
};


struct tWeight_4
{
	float arrWeight[4];
};


// ===================
// 상수버퍼 대응 구조체
// ===================
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;

	Matrix matWV;
	Matrix matWVP;

	Matrix matWorldInv;
	Matrix matViewInv;
};

struct tScalarParam
{
	tMtrlData mtrl;

	int    iArr[4];
	float  fArr[4];
	Vec2   v2Arr[4];
	Vec4   v4Arr[4];
	Matrix matArr[4];

	int bTex[10];
	int arrAnimData[2];	// 3D Animation 정보
};

struct tAnim2D
{
	Vec2 vLT;
	Vec2 vSlice;
	Vec2 vBackgroundSize;
	Vec2 vOffset;

	int   useAnim2D;
	float Atlas_Width;
	float Atlas_Height;
	float Anim2D_Padding;
};

struct tGlobal
{
	Vec2  vResolution;		// RenderTarget Resolution
	Vec2  vNoiseResolution; // NoiseTexture Resolution
	float fDT;
	float fAccTime;
	int   Light2DCount;
	int   Light3DCount;
};

struct SB_PcikingData
{
	UINT objectID;
	Vec3 objectPos;
};


struct tSSAO_ChangesOnResize
{
	Matrix g_ViewToTexSpace;     // Proj * Texture
	Vec4   g_FarPlanePoints[3];  // 원거리 삼각형(네 모서리를 덮음), 삼각형은 아래 참조
	Vec2   g_TexelSize;          // (1.0f/W, 1.0f/H)

	Vec2 padding;
};

struct tSSAO_ChangesRarely
{
	// 방향으로 균일한 분포를 갖지만 길이는 무작위인 14개의 벡터
	Vec4 g_OffsetVectors[14];

	// 보는 공간의 좌표
	float g_OcclusionRadius;
	float g_OcclusionFadeStart;
	float g_OcclusionFadeEnd;
	float g_SurfaceEpsilon;

	//
	// SSAO_Blur용
	//
	Vec4 g_BlurWeights[3];
	// 정적 변수는 상수 버퍼에 속하지 않습니다.
	//static float s_BlurWeights[12] = (float[12]) g_BlurWeights;

	int  g_BlurRadius;
	Vec3 g_Pad;
};
