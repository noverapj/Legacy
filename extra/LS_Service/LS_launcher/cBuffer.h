// cBuffer.h: interface for the cBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CBUFFER_H__ECE7A49E_767B_4F38_91CE_A4795B0F4535__INCLUDED_)
#define AFX_CBUFFER_H__ECE7A49E_767B_4F38_91CE_A4795B0F4535__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class cBuffer  
{
public:
	cBuffer();
	cBuffer( uint32 uBuffLen );
	virtual ~cBuffer();

	void	Init();
	void	Destroy();

public:
	bool	Create( uint32 uBuffLen );
	bool	Erase();

	void	Resize( uint32 uBuffLen );

public:
	void	SetLength( uint32 unLen )	{	m_unLen = unLen;	}

	uint32	GetMaxLength()		{	return m_unMax;			}
	uint32	GetLength()			{	return m_unLen;			}
	uint8*	GetBuffer()	const 	{	return m_pDT;			}
	char*	GetString()	const 	{	return reinterpret_cast<char*>(m_pDT);	}
	
	uint8*	GetBuffer( uint32 unLen )	const 	{	return (m_pDT+unLen);		}

	bool	Copy( const char* pDT );
	bool	Copy( uint32 unDst, const char* pDT );
	bool	Copy( const uint8* pDT, uint32 unLen );
	bool	Copy( uint32 unDst, const uint8* pDT, uint32 unLen );

	bool	Append( const char pDT );
	bool	Append( const char* pDT );
	bool	Append( const uint8* pDT, uint32 unLen );

protected:
	uint8*	m_pDT;
	uint32	m_unLen;
	uint32	m_unMax;
};




#endif // !defined(AFX_CBUFFER_H__ECE7A49E_767B_4F38_91CE_A4795B0F4535__INCLUDED_)
