

#ifndef _ioVertexBufferBinder_h_
#define _ioVertexBufferBinder_h_

class ioVertexBufferInstance;

class __EX ioVertexBufferBinder
{
public:
	struct VertexBufferStream 
	{
		UINT m_uStream;
		ioVertexBufferInstance *m_pInstance;

		VertexBufferStream(){}
		VertexBufferStream( UINT uStream, ioVertexBufferInstance *pInstance )
		{
			m_uStream = uStream;
			m_pInstance = pInstance;
		}

		bool operator < ( const VertexBufferStream &rhs )
		{
			return ( m_uStream < rhs.m_uStream );
		}
	};
	typedef std::vector< VertexBufferStream > VertexBufferBindList;

private:
	VertexBufferBindList m_BindStreamList;

public:
	void SetBinding( UINT uIndex, ioVertexBufferInstance *pInstance );

	ioVertexBufferInstance* UnSetBinding( UINT uIndex );
	ioVertexBufferInstance* UnSetBindingByMask( DWORD dwMask );
	void UnSetAllBindings();

public:
	ioVertexBufferInstance* GetBufferInstance( UINT uIndex ) const;
	ioVertexBufferInstance* GetBufferInstanceByMask( DWORD dwMask ) const;

public:
	inline const VertexBufferBindList& GetBufferList() const { return m_BindStreamList; }
	inline int GetBufferCount() const { return m_BindStreamList.size(); }

public:
	ioVertexBufferBinder();
	~ioVertexBufferBinder();
};

#endif
