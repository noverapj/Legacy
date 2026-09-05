#pragma once

// packet header
class cHeader  
{
public:
	cHeader()
	{
		m_payload	= 0;
		m_command	= 0;
		m_sequence	= 0;
	}

public:
	void SetHeader( const uint16 payload = 0, const uint16 command = 0, const uint8 sequence = 0 )	
	{	
		m_payload	= payload;	
		m_command	= command;
		m_sequence	= sequence;
	}

	BOOL   IsPerfect(const uint8 sequence)	{ return (sequence == m_sequence) ? TRUE : FALSE; }
	uint16 GetPayload()	{ return m_payload;  }
	uint16 GetCommand()	{ return m_command;  }
	uint8  GetSequence()   { return m_sequence; }

protected:
	uint16	m_payload;
	uint16	m_command;
	uint8   m_sequence;
};

