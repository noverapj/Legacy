

#ifndef _ioUIRenderer_h_
#define _ioUIRenderer_h_

#include "io3DCommon.h"

#include "ioRenderOperation.h"

class ioRenderSystem;
class ioQuadRenderArray;
class ioUIRenderImage;

class __EX IUIQuad
{
public:
	virtual void AddInfo( ioQuadRenderArray *pQuadArray ) = 0;
	virtual int GetQuadCount() const = 0;

public:
	virtual bool IsDirectQuad() const
	{
		return false;
	}

	virtual void RenderDirect( ioRenderSystem *pRenderSystem )
	{
	}

public:
	IUIQuad(){}
	virtual ~IUIQuad(){}
};

class __EX ioQuadRenderArray
{
public:
	struct QuadInfo
	{
		IDirect3DTexture9 *m_pTex;
		UIRenderType m_RenderType;
		TextureFilterOption m_Filter;
		IUIQuad *m_pDirectQuad;
		const ioUIRenderImage *m_pAlphaImage;

		inline bool IsSameSetting( const QuadInfo &rhs ) const
		{
			if( m_pTex == rhs.m_pTex &&
				m_RenderType == rhs.m_RenderType &&
				m_Filter == rhs.m_Filter &&
				m_pDirectQuad == rhs.m_pDirectQuad )
			{
				return true;
			}

			return false;
		}
	};

private:
	QuadInfo   *m_pRenderInfo;
	ioVtxRHWColorTex2 *m_pVtxArray;

	int m_iMaxCount;

	int m_iDirectInfoCount;
	int m_iRenderInfoCount;

public:
	void AddDirectQuadInfo( IUIQuad *pQuad );

	void AddRenderQuadInfo( const D3DXVECTOR2 &vPos,
							const D3DXVECTOR2 &vSize,
							IDirect3DTexture9 *pTex,
							const D3DXVECTOR4 &vTexUV,
							UIRenderType eRenderType,
							TextureFilterOption eFilter,
							DWORD dwColor,
							const ioUIRenderImage *pAlphaImage = NULL );

public:
	void SetBuffer( ioVtxRHWColorTex2 *pBuf );

public:
	inline const QuadInfo& GetAt( int iIndex ) const { return m_pRenderInfo[iIndex]; }
	inline void Clear() 
	{
		m_pVtxArray = NULL;

		m_iDirectInfoCount = 0;
		m_iRenderInfoCount = 0;
	}

public:
	inline QuadInfo* GetInfoPtr() const { return m_pRenderInfo; }

	inline int GetAllocatedCount() const { return m_iMaxCount; }
	inline int GetRenderInfoCount() const { return m_iRenderInfoCount; }
	inline int GetDirectInfoCount() const { return m_iDirectInfoCount; }
	inline int GetTotalUseCount() const { return m_iDirectInfoCount + m_iRenderInfoCount; }

public:
	ioQuadRenderArray( int iMaxCount );
	~ioQuadRenderArray();
};

class __EX ioUIRenderer : public Singleton< ioUIRenderer >
{
protected:
	typedef std::vector< IUIQuad* > UIQuadList;
	UIQuadList m_UIQuadList;

	ioRenderOperation m_RenderOp;
	ioQuadRenderArray *m_pQuadRenderArray;
	int m_iPreVertexEnd;

public:
	void Initialize();
	void AddUIQuad( IUIQuad *pUIQuad );

public:
	int GetNotRenderedQuadCount() const;

public:
	// 내부적으로 BeginScene - EndScene을 호출함
	// 15.02.21 JSM, BeginScene - EndScene을 호출을 선택적으로 바꿈
	void Render( ioRenderSystem *pRenderSystem, bool bSceneCall = true );

protected:
	void InitCommonRenderState( ioRenderSystem *pRenderSystem );
	
	bool UpdateBuffers();
	void FillBuffers( ioVtxRHWColorTex2 *pBuf );
	void ClearQuadList();

public:
	static ioUIRenderer& GetSingleton();
	static ioUIRenderer* GetSingletonPtr();

public:
	ioUIRenderer();
	virtual ~ioUIRenderer();
};

#define g_UIRenderer ioUIRenderer::GetSingleton()

#endif