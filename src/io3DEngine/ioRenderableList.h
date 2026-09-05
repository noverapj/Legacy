

#ifndef _ioRenderableList_h_
#define _ioRenderableList_h_

class ioPass;
class ioShader;
class ioRenderable;

struct RenderablePass
{
	ioPass *pPass;
	ioRenderable *pRend;

	ioShader *pShader;
	int iLightIndex;

	RenderablePass(){}
	RenderablePass( ioPass *p, ioRenderable *r )
		: pPass(p), pRend(r)
	{
		pShader = NULL;
		iLightIndex = 0;
	}
};

class ioRenderableList
{
private:
	RenderablePass *m_pArray;
	int m_iMaxSize;
	int m_iCurSize;
	int m_iGrowBy;

public:
	void Initialize( int iMaxSize, int iGrowBy );
	void Add( const RenderablePass &rkPass );

public:
	inline const RenderablePass& GetAt( int iIndex ) const { return m_pArray[iIndex]; }
	inline void Clear() { m_iCurSize = 0; }

public:
	inline RenderablePass* GetPtr() const { return m_pArray; }
	inline int GetCount() const { return m_iCurSize; }
	inline int GetAllocatedSize() const { return m_iMaxSize; }
	inline int GetGrowSize() const { return m_iGrowBy; }

public:
	ioRenderableList();
	ioRenderableList( int iMaxSize, int iGrowBy );
	~ioRenderableList();
};

#endif