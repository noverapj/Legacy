

#ifndef _ioIdxBuffer_h_
#define _ioIdxBuffer_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class __EX ioIdxBuffer
{
private:
	IDirect3DIndexBuffer9 *m_pIdxBuffer;
	void	*m_pBuffer;

	BufferType  m_BufType;
	BufferUsage m_BufUsage;
	DWORD	m_dwLockFlags;
	int		m_iIdxCnt;

public:
	void ReleaseIfDefaultPool();
	void ReCreateIfDefaultPool( IDirect3DDevice9 *pDevice );

public:
	bool CreateIndexBuffer( IDirect3DDevice9 *pDevice,
							BufferType eType,
							BufferUsage eUsage,
							int iIdxCnt );

	bool Lock( DWORD dwAddFlags = 0, int iStartBytes = 0, int iLockSizeBytes = 0 );
	bool UnLock();

public:
	inline IDirect3DIndexBuffer9* GetIdxBuffer(){ return m_pIdxBuffer; }
	inline void* GetBuffer(){ return m_pBuffer; }
	inline int GetIdxCnt() const { return m_iIdxCnt; }
	inline int GetBufferSizeByBytes() const { return m_iIdxCnt * sizeof(WORD); }

public:
	ioIdxBuffer();
	~ioIdxBuffer();
};

#endif