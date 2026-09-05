

#ifndef _ioVertexDeclaration_h_
#define _ioVertexDeclaration_h_

#include "io3DCommon.h"

class __EX ioStreamElement
{
public:
	int		m_iStream;
	VertexComponent m_Component;

public:
	bool operator<( const ioStreamElement &rhs ) const
	{
		if( m_iStream < rhs.m_iStream )
			return true;
		else if( m_iStream == rhs.m_iStream )
		{
			if( m_Component < rhs.m_Component )
				return true;
		}

		return false;
	}

	bool operator==( const ioStreamElement &rhs ) const
	{
		if( m_iStream != rhs.m_iStream || m_Component != rhs.m_Component )
			return false;

		return true;
	}

	bool operator!=( const ioStreamElement &rhs ) const
	{
		return !( *this == rhs );
	}

public:
	ioStreamElement( int iStream, VertexComponent eComponent )
		: m_iStream( iStream ), m_Component( eComponent ){}
};

class __EX ioVertexDeclaration
{
private:
	typedef std::set< ioStreamElement > StreamElementList;
	StreamElementList	m_VertexStreamList;

	IDirect3DVertexDeclaration9 *m_pVertexDeclaration;
	bool m_bNeedUpdate;

public:
	void InsertStreamSource( int iStream, DWORD dwMask );
	void InsertStreamSource( int iStream, VertexComponent eComponent );
	void RemoveStreamSource( int iStream, DWORD dwMask );
	void RemoveStreamSource( int iStream, VertexComponent eComponent );
	void ModifyStreamSource( int iPreStream, DWORD dwPreMask, int iNewStream, DWORD dwNewMask );
	void ModifyStreamSource( int iPreStream, VertexComponent ePreComponent,
							 int iNewStream, VertexComponent eNewComponent );
	void RemoveStreamGap();

	static void ParseStreamMaskToComponent( DWORD dwMask, VertexComponentList &kComponentList );

	void ClearAllStreamSource();

public:
	DWORD GetFullStreamMask() const;
	DWORD GetStreamMask( int iStream ) const;
	int GetMaxStream() const;

	void SetD3DDeclaration( IDirect3DVertexDeclaration9 *pDecl ) { m_pVertexDeclaration = pDecl; }
	IDirect3DVertexDeclaration9* GetD3DDeclaration();

public:
	inline bool operator==( const ioVertexDeclaration &rhs ) const
	{
		if( m_VertexStreamList.size() != rhs.m_VertexStreamList.size() )
			return false;

		StreamElementList::const_iterator iter, iEnd;
		StreamElementList::const_iterator iRhs, iRhsEnd;

		iter = m_VertexStreamList.begin();
		iEnd = m_VertexStreamList.end();
		iRhs = rhs.m_VertexStreamList.begin();
		iRhsEnd = rhs.m_VertexStreamList.end();

		for( ; iter!=iEnd && iRhs!=iRhsEnd ; ++iter, ++iRhs )
		{
			if( *iter != *iRhs )
				return false;
		}

		return true;
	}

	inline bool operator!=( const ioVertexDeclaration &rhs ) const
	{
		return !( *this == rhs );
	}

public:
	ioVertexDeclaration();
	ioVertexDeclaration( const ioVertexDeclaration &rhs );
	~ioVertexDeclaration();
};

#endif
