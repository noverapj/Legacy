

#ifndef _ioRenderSystem_h_
#define _ioRenderSystem_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioPlane;
class ioLight;
class ioRenderable;
class ioRenderOperation;
class ioSkeleton;
class ioShader;
class ioTexture;
class ioRenderTexture;

class ioMeshData;
class ioVertexDeclaration;
class ioVertexBufferBinder;
class ioIndexBufferInstance;
class ioTextureUnitState;
class ioEnumDisplayMode;

class __EX ioRenderSystem
{
protected:
	IDirect3D9			*m_pD3D;
	IDirect3DDevice9	*m_pD3DDevice;
	DWORD	m_dwDeviceBehaviorFlags;

	IDirect3DSurface9 *m_pRenderTarget;
	IDirect3DSurface9 *m_pDepthSurface;
	IDirect3DSurface9 *m_pCaptureTarget;

	D3DPRESENT_PARAMETERS m_D3DParam;
	ioEnumDisplayMode   *m_pEnumDisplay;

	CRITICAL_SECTION  m_CaptureSection;

	D3DCOLORVALUE	m_Ambient;
	int m_iUsedLights;

	D3DXMATRIX		m_matModel;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matProj;

	D3DVIEWPORT9	m_ViewPort;

	HWND	m_hWnd;
	UINT	m_uClientWidth;
	UINT	m_uClientHeight;

	typedef std::vector< D3DMULTISAMPLE_TYPE > MultiSampleEnableList;
	MultiSampleEnableList m_EnableMuntiSampleList;
	bool    m_bNowMultiSampleOn;

	bool	m_bDeviceLost;
	bool	m_bSoftwareProcessing;

	bool	m_bFogEnable;
	float	m_fFogStart;
	float	m_fFogEnd;
	float	m_fFogConstants;
	DWORD	m_dwFogColor;

	D3DGAMMARAMP m_CurGamma;
	int		m_iCurBright;
	int		m_iCurContrast;

	ShadowMapType m_ShadowMapType;

	int		m_iRenderedFaceCount;
	int		m_iRenderCallCount;

	FillMode m_GlobalFillMode;
	bool m_bSetGlobalFillMode;

	bool m_bDialogBoxMode;
	bool m_bNowDeviceResetted;
	bool m_bCaptureShotSafe;	// DeviceLost or 설정변경으로 인한 Reset시 false

public:
	static D3DCAPS9	m_D3DCaps;
	static D3DSURFACE_DESC m_BackSurfaceDesc;
	static BufferUsage m_NoneAutoBufferUsage;

public:
	// ioEnumDisplayMode가 체크할 최소 해상도 : Create이전에 호출해야함. 디폴트 : 640x480
	void SetMinDisplayMode( UINT uMinWidth, UINT uMinHeight );

	// Create 이후에 호출가능하다.
	int  GetNumEnableDisplayMode() const;
	bool GetDisplayResolution( int iIndex, UINT &uWidth, UINT &uHeight ) const;

public:
	bool Create( HWND hWnd,
				 UINT uWidth,
				 UINT uHeight,
				 bool bSwapCopy,
				 bool bWindow = true,
				 bool bUse16BitBuffer = false );

protected:
	bool CreateDirect3D();
	HRESULT CreateD3DDevice( UINT AdapterToUse, D3DDEVTYPE DeviceType );

public:
	void RenderPrimitive( D3DPRIMITIVETYPE eType, UINT uStartVertex, UINT uPrimitiveCount );
	void Render( ioRenderOperation &rkRo, ioShader *pShader );
	void RenderQuad();

public:
	void SetFixedFVF( DWORD dwFVF );
	void SetSoftwareVertexProcessing( bool bSoftware );

	void SetVertexDeclaration( ioVertexDeclaration *pDeclaration );
	void SetVertexBufferBinder( ioVertexBufferBinder *pBinder );
	bool SetIndexBufferInstance( ioIndexBufferInstance *pIndex );

	void ClearRenderStateDesc();
	void ClearStreamDesc();
	void ClearRenderedCounts();

	void DisableShader();
	void EvictManagedResources();

public:
	void SetSceneBlend( D3DBLENDOP eOp, D3DBLEND eSrc, D3DBLEND eDest );
	void SetShadingType( ShadeType shade );
	void SetCullingMode( CullingMode mode );
	void SetFillMode( FillMode mode );
	void SetGlobalFillMode( bool bUse, FillMode eMode = FM_SOLID );

	void SetDepthFunction( D3DCMPFUNC eDepthFunc, bool bWriteEnable, bool bCheckEnable );
	void SetDepthBias( float fDepthBias, float fBiasSlope );

	void SetAlphaTest( D3DCMPFUNC eTestFunc, int iAlphaRef );
	void SetColorWriteEnable( DWORD dwColorFlag );
	void SetTextureFactor( DWORD dwFactor );

	void SetTextureUnitSetting( int iUnit, ioTextureUnitState *pState );
	void DisableTextureUnitsFrom( int iUnit );

	void SetTextureMaxAnisotropy( int iUnit, int iMaxAniso );
	void SetTextureStageColorBlend( int iUnit, const StageBlendOperation &rkBlend );
	void SetTextureStageColorBlend( int iUnit, D3DTEXTUREOP eStageOp,
											   DWORD dwSrcFactor,
											   DWORD dwDestFactor );

	void SetTextureStageAlphaBlend( int iUnit, const StageBlendOperation &rkBlend );
	void SetTextureStageAlphaBlend( int iUnit, D3DTEXTUREOP eStageOp,
											   DWORD dwSrcFactor,
											   DWORD dwDestFactor );

	void DisableAlphaBlend();
	void DisableAlphaTest();

public:
	void SetRenderTarget( ioRenderTexture *pRTT );
	void RestoreRenderTarget();

	bool CopyRenderTarget( IDirect3DSurface9 *pDest,
						   RECT *pRcDest = NULL,
						   D3DTEXTUREFILTERTYPE eFilter = D3DTEXF_NONE );

	void SetDialogBoxMode( bool bEnable );
	inline bool IsDialogBoxMode() const { return m_bDialogBoxMode; }

public:
	void ClearBack( DWORD dwFlags, DWORD dwColor = 0xffffffff, const D3DRECT *pRect = NULL );
	bool BeginScene();	
	bool EndScene();
	bool Present();

public:
	// Lost : false, Not Lost : true
	bool CheckLostDevice( DWORD dwSleepTime = 50 );
	void HandlePossibleSizeChange();

public:
	// Parameter만 변경하고 실제 적용하지 않는다.
	bool ChangeBackBufferFormat( bool bUse16BitFormat );
	bool ChangeBackBufferSize( UINT uWidth, UINT uHeight );
	bool ChangeSwapEffect( D3DSWAPEFFECT eSwapEffect );
	bool ChangeMultiSamplingType( D3DMULTISAMPLE_TYPE eType );
	bool ChangeScreenMode( bool bFullScreen );

	// PresentationInterval
	void SetPresentationInterval();

	// 변경된 Parameter에 대한 실제 적용
	bool ApplyChangedPresentParameters();

public:
	bool IsEnableMultiSamplingType( D3DMULTISAMPLE_TYPE eType );
	bool IsPossibleMultiSampling() const;
	bool IsNowMultiSamplingOn() const;

	void EnableMultiSampling( bool bEnable );

protected:
	void CheckMaxMultiSampling( D3DFORMAT eFormat, BOOL bWindow );
	void UpdateSurfaceInfo();

	HRESULT Reset3DEnvironment();

public:
	// 스크린샷 작업을 한방에 완료..
	bool CaptureScreen( const char *szFileName );

	// 스레딩관련해서 m_pCatureTarget에 저장만 해둠 ( DeviceLost시 날아갈수 있음 )
	// 0.1f <= fSizeRate <= 1.0f
	bool PrePareThreadCapture( D3DTEXTUREFILTERTYPE eFilter, float fSizeRate );

	IDirect3DSurface9* BeginThreadCapture();
	void EndThreadCapture();

protected:
	bool CreateCaptureTarget( bool bReCreateMust );

// Light Setting
public:
	void SetLightEnable( bool bEnable );
	void SetMaterial( const D3DMATERIAL9 &rkMaterial );
	void SetAmbientLight( float r, float g, float b );
	void SetLightsList( const ioLightList &rkLight );	

	inline D3DCOLORVALUE GetAmbientColor() const { return m_Ambient; }

// Fog
public:
	void SetFogEnable( bool bEnable );
	void SetFogFactor( DWORD dwColor, float fStart, float fEnd );
	D3DXVECTOR4 GetFogFactor() const;

	float GetFogStart() const;
	float GetFogEnd() const;
	DWORD GetFogColor() const;

// Texture Filtering
	void SetTextureAddressMode( int iStage, TextureAddressMode mode );
	void SetTextureFiltering( int iStage, TextureFilterOption filter );
	DWORD GetMinFilter( TextureFilterOption filter );
	DWORD GetMagFilter( TextureFilterOption filter );
	DWORD GetMipFilter( TextureFilterOption filter );

	void SetTexture( int iStage, const ioHashString &kName );
	void SetTexture( int iStage, IDirect3DTexture9 *pTex );
	void SetioTexture( int iStage, ioTexture *pTex );

	void SetTextureMipMapBias( int iStage, float fBias );
	void SetTextureTransformFlag( int iStage, DWORD dwFlags );
	void SetTextureMatrix( int iStage, const D3DXMATRIX &rkMatrix );
	void SetTextureCoordIndex( int iStage, int iIndex );

// Transform Matrix
	void SetModelMatrix( const D3DXMATRIX *pMatModel );
	void SetModelMatrixIdentity();
	void SetViewMatrix( const D3DXMATRIX *pMatView );
	void SetProjMatrix( const D3DXMATRIX *pMatProj );
	void SetViewPort( const D3DVIEWPORT9 &kViewPort );

public:
	inline const D3DXMATRIX* GetViewMatrix() const { return &m_matView; }
	inline const D3DXMATRIX* GetProjectMatrix() const { return &m_matProj; }
	inline const D3DVIEWPORT9& GetViewPort() const { return m_ViewPort; }

// Gamma
public:
	void InitGamma();
	void ControlBrightness( int iBright );
	void ControlContrast( int iContrast );

// About Caps
public:
	int GetVertexShaderVersion() const;
	int GetPixelShaderVersion() const;
	
	inline int GetWindowWidth() const { return m_D3DParam.BackBufferWidth; }
	inline int GetWindowHeight() const { return m_D3DParam.BackBufferHeight; }

public:
	D3DMULTISAMPLE_TYPE GetEnableMultiSampleType( int iIndex ) const;
	inline int GetEnableMultiSampleTypeCount() const { return m_EnableMuntiSampleList.size(); }

// Etc	
public:
	inline IDirect3D9* GetD3D() const { return m_pD3D; }
	inline IDirect3DDevice9* GetDevice() const { return m_pD3DDevice; }
	inline HWND GetHWND() const { return m_hWnd; }
	inline bool IsDeviceLost() const { return m_bDeviceLost; }
	inline bool IsDeviceResetted() const { return m_bNowDeviceResetted; }
	inline bool IsCaptureShotSafe() const { return m_bCaptureShotSafe; }

public:	// Test
	ShadowMapType CheckEnableShadowMapType();
	inline ShadowMapType GetShadowMapType() const { return m_ShadowMapType; }
	inline int GetRenderedFaceCount() const { return m_iRenderedFaceCount; }
	inline int GetRenderCallCount() const { return m_iRenderCallCount; }

public:
	ioRenderSystem();
	virtual ~ioRenderSystem();
};

__EX ioRenderSystem& RenderSystem();
__EX void ReleaseRenderSystem();

#endif
