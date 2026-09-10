#pragma once

class ioMeshData;
class ioTexture;
class ioRenderSystem;

//////////////////////////////////////////////////////////////////////////
class io2DRotateQuad
{
protected:
	ioMeshData *m_pMeshData;
	std::vector<ioTexture*> m_vTexList;

	int m_iRenderX;
	int m_iRenderY;

	int m_iCurArray;

	D3DXQUATERNION m_qtOriginal;

public:
	void InitResource( ioHashStringVec vTexNameList );

public:
	bool IsNeedRender() const;
	void Update( int iXPos, int iYPos, float fAngle, int iArray );
	void Render( ioRenderSystem *pSystem );
	int GetWidth();
	int GetHeight();

public:
	io2DRotateQuad();
	virtual ~io2DRotateQuad();
};

//////////////////////////////////////////////////////////////////////////
class ioRotateQuad : public IUIQuad
{
protected:
	io2DRotateQuad *m_pQuad;

public:
	virtual void AddInfo( ioQuadRenderArray *pQuadArray );
	virtual int GetQuadCount() const { return 0; }
	virtual bool IsDirectQuad() const { return true; }
	virtual void RenderDirect( ioRenderSystem *pRenderSystem );

public:
	ioRotateQuad( io2DRotateQuad *pQuad );
	virtual ~ioRotateQuad();
};