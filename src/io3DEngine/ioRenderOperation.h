

#ifndef _ioRenderOperation_h_
#define _ioRenderOperation_h_

class ioMeshData;

class __EX ioRenderOperation
{
public:
	ioMeshData			*m_pMeshData;
	D3DPRIMITIVETYPE	m_PrimitiveType;

	int m_iMinIndex;
	int m_iVertexCount;
	int m_iBaseVertexIndex;

	int m_iIndexStart;
	int m_iFaceCount;

	bool m_bUseIndex;

public:
	ioRenderOperation()
	{
		m_pMeshData = NULL;
		m_PrimitiveType = D3DPT_TRIANGLELIST;

		m_iMinIndex = 0;
		m_iVertexCount = 0;
		m_iBaseVertexIndex = 0;

		m_iIndexStart = 0;
		m_iFaceCount  = 0;

		m_bUseIndex = true;
	}
};

#endif
