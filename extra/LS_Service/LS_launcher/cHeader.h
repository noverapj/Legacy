// cHeader.h: interface for the cHeader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHEADER_H__2F21E167_CB25_4600_B3E0_A34A07BCFC3D__INCLUDED_)
#define AFX_CHEADER_H__2F21E167_CB25_4600_B3E0_A34A07BCFC3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



// packet header
class cHeader  
{
public:
	cHeader()
	{
		m_nPayload		= 0;
		m_nCommand		= 0;
		m_nSequence     = 0;
	}

public:
	void	SetHeader( const uint16 nPayload = 0, const uint16 nCommand = 0, const uint8 nSequence = 0 )	
	{	
		m_nPayload	= nPayload;	
		m_nCommand	= nCommand;
		m_nSequence = nSequence;
	}

	BOOL	IsPerfect(const uint8 nSequence)			{	return (nSequence == m_nSequence) ? TRUE : FALSE;	}
	uint16	GetPayload()	{	return m_nPayload;		}
	uint16	GetCommand()	{	return m_nCommand;		}
	uint8   GetSequence()   {   return m_nSequence;     }

protected:
	uint16	m_nPayload;
	uint16	m_nCommand;
	uint8   m_nSequence;
};




#endif // !defined(AFX_CHEADER_H__2F21E167_CB25_4600_B3E0_A34A07BCFC3D__INCLUDED_)
