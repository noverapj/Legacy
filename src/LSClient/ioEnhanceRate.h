#pragma once

enum EnhanceOwnerType
{
	EOT_PUSH,		// Push
	EOT_BLOW,		// Blow
	EOT_AIR_PUSH,	// Air Push
	EOT_AIR_BLOW,	// Air Blow
};

struct EnhanceRate
{
	EnhanceOwnerType m_Type;
	float m_fEnhanceRate;
	DWORD m_dwID;

	EnhanceRate()
	{
		m_Type = EOT_PUSH;
		m_fEnhanceRate = FLOAT1;
		m_dwID = 0;
	}
};

class SortEnhanceRateBigToSmall : public std::binary_function< const EnhanceRate&, const EnhanceRate&, bool >
{
public:
	bool operator()( const EnhanceRate &lhs , const EnhanceRate &rhs ) const
	{
		if( lhs.m_fEnhanceRate > rhs.m_fEnhanceRate )
			return true;

		return false;
	}
};

class ioEnhanceRate
{
private:
	typedef std::list< EnhanceRate > EnhanceRateList;
	EnhanceRateList m_EnhanceList;

	DWORD m_dwCurID;

public:
	DWORD AddEnhanceRate( EnhanceOwnerType eType, float fRate );
	void RemoveEnhanceRate( EnhanceOwnerType eType, DWORD dwID=0 );
	void ResetEnhanceRateList();

public:
	bool HasEnhanceRate( EnhanceOwnerType eType ) const;

	float GetEnhanceRate( EnhanceOwnerType eType ) const;
	float GetTotalEnhanceRate( EnhanceOwnerType eType ) const;

public:
	ioEnhanceRate();
	~ioEnhanceRate();
};

