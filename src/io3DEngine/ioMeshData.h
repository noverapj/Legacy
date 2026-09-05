

#ifndef _ioMeshData_h_
#define _ioMeshData_h_

class ioVertexDeclaration;
class ioVertexBufferBinder;
class ioIndexBufferInstance;
class ioVertexBufferInstance;

class __EX ioMeshData
{
public:
	ioVertexDeclaration		*m_pVertexDeclaration;
	ioVertexBufferBinder	*m_pVertexBufferBinder;
	ioIndexBufferInstance	*m_pIndexBufferInstance;

	int m_iVertexCount;
	int m_iIndexCount;

public:
	void InsertStreamSource( int iStream, DWORD dwMask );
	void InsertStreamSource( int iStream, VertexComponent eComponent );
	void SetBinding( UINT uIndex, ioVertexBufferInstance *pInstance );

public:
	ioVertexBufferInstance* GetVtxInstance( UINT uIndex );
	ioVertexBufferInstance* GetVtxInstanceByMask( DWORD dwMask );

public:
	void DeleteAllBuffers();	// 모든 버퍼가 개인소유일경우에만 사용
	void DeleteAllBindingBuffers(); // 버텍스 버퍼쪽만 제거

public:
	ioMeshData();
	~ioMeshData();
};

#endif