#pragma once

#include "io3DCommon.h"
#include "ioMath.h"

class ioSceneManager;
class ioCamera;
class ioEffect;
class ioEntityGrpMaterialModifier;

class __EX ioUI3DEffectRender
{
public:
	enum
	{
		eERT_PRERENDER = 1,
		eERT_NORMALRENDER = 2,
		eERT_AFTERRENDER = 3,
	};

    enum
    {
        UI_3D_EFFECT_BASE_WIDTH  = 1280,
        UI_3D_EFFECT_BASE_HEIGHT = 800,
    };

protected:
    struct UIEffectInfo
    {
        ioHashString m_szName;
        D3DXVECTOR3  m_vPos;
		D3DXVECTOR3  m_vScale;
		D3DXVECTOR3  m_vDir;

        void Init()
        {
            m_szName = "";
            m_vPos   = ioMath::VEC3_ZERO;
            m_vScale = ioMath::UNIT_ALL;
            m_vDir   = ioMath::UNIT_Y;
        }

        UIEffectInfo()
        {
            Init();
        }
    };

protected:
	ioSceneManager *m_pSceneMgr;
	ioCamera *m_pCamera;

	D3DCOLORVALUE m_UISceneAmbient;
	D3DCOLORVALUE m_BackUpAmbient;

	typedef std::vector< ioEffect* > EffectList;
	EffectList m_EffectList;
	
	int m_iXPos;
	int m_iYPos;
	int m_iWidth;
	int m_iHeight;
	int m_iRenderType;
	float m_fDist;

    D3DXVECTOR3 m_vPos;

	ioHashString	m_szType;

	bool		m_bShow;

    bool m_bUseViewPort;
    D3DRECT m_rcViewPortRect;

    ioHashString    m_szCamINI;

    typedef std::list<UIEffectInfo> UIEffectInfoList;
    UIEffectInfoList m_UIEffectInfoList;

	bool m_bPosAbsolute;

public:
    void Start();
    void Stop();
    bool IsLive();
    void ClearInfoList();

public:
    void Initialize( ioHashString &szType, ioHashString &szININame, ioHashString &szCamera );

	void Destroy();
	ioHashString GetType(){return m_szType;};

	void SetShow(bool bShow){m_bShow = bShow;};
	bool isShow(){return m_bShow;};

public:
	void RenderNow( const D3DXMATRIX *pExtraProj, bool bZClear );
	void RenderViewPort( const D3DRECT *pRcView, bool bZClear );
    void Render( const D3DXMATRIX *pExtraProj, bool bZClear );
	void Render( int iRenderType, int iXPos, int iYPos );
	void Render(int iXPos, int iYPos );

    void Render();
    void RenderViewPort();

public:
	void ApplyUIAmbient();
	void RestoreAmbient();

	float GetDistince();
	void SetDistince(float fdist, int nIndex = 0);
	void SetLookAt( const D3DXVECTOR3 &vLookAt );

public:
	D3DXMATRIX MakeUIProjection( int iXPos, int iYPos, int iWidth, int iHeight );

protected:
	void SetMainLight( const D3DXVECTOR3 &vDir,
					   const D3DCOLORVALUE &rkDiffuse,
					   const D3DCOLORVALUE &rkSpecular,
					   const ioHashString &rkLightTexture );

public:
    bool AddEffect( const ioHashString &szName,
					const D3DXVECTOR3 &vPos,
					const D3DXVECTOR3 &vScale = ioMath::UNIT_ALL,
					const D3DXVECTOR3 &vDir = ioMath::UNIT_Y,
                    const bool bStart = false);

	bool AddEffect( const ioHashString &szName,
                    const ioHashString &szRenderType,
					const D3DXVECTOR3 &vPos,
					const D3DXVECTOR3 &vScale = ioMath::UNIT_ALL,
					const D3DXVECTOR3 &vDir = ioMath::UNIT_Y,
                    const bool bStart = false);

	void UpdateEffectList( float fFramesPerSec );

	inline int  GetPosX()           { return m_iXPos; }
	inline int	GetPosY()           { return m_iYPos; }
	inline void SetPosX(int iXPos )  { m_iXPos= iXPos; }
	inline void SetPosY(int iYPos)  { m_iYPos = iYPos; }

    inline int  GetWidth()          { return m_iWidth; }
    inline int  GetHeight()         { return m_iHeight; }
	inline void SetSize(int iWidth, int iHeight);

	bool GetPosAbsolute() const { return m_bPosAbsolute; }
	void SetPosAbsolute(bool val) { m_bPosAbsolute = val; }

	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetCamPos();
	void		SetCamPos(D3DXVECTOR3 vPos);

    int ParserRenderType(const ioHashString &szRenderType);

protected:
	void DestroyAllEffects();
	void DestroyEffect( ioEffect *pEffect );

public:
	ioUI3DEffectRender();
	virtual ~ioUI3DEffectRender();
};