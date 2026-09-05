#pragma once

enum
{
	MAX_GASHAPON_SELECT_SLOT    = 10,
	MAX_GASHAPON_SELECT_SUCCESS = 100,
};

struct SelectGashaponValue
{
	short m_iType;
	int m_iValue1;
	int m_iValue2;
	
	SelectGashaponValue()
	{
		Clear();
	}

	void Clear()
	{
		m_iType	  = 0;
		m_iValue1 = 0;
		m_iValue2 = 0;
	}

	bool IsEmpty()
	{
		if( m_iType	<= 0 && m_iValue1 <= 0 && m_iValue2 <= 0 )
			return true;

		return false;
	}

	bool operator==( const SelectGashaponValue &rhs )
	{
		if( m_iType == rhs.m_iType && m_iValue1 == rhs.m_iValue1 && m_iValue2 == rhs.m_iValue2 )
			return true;

		return false;
	}
};

typedef std::vector<SelectGashaponValue> SelectGashaponValueVec;
