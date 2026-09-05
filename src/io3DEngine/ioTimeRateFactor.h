

#ifndef _ioTimeRateFactor_h_
#define _ioTimeRateFactor_h_

#include "io3DCommon.h"

class __EX ioTimeRateFactor
{
private:
	struct TimeFactor
	{
		float fTimeRate;
		float fFactor;

		TimeFactor( float fTime, float fFact )
		{
			fTimeRate = fTime;
			fFactor = fFact;
		}
	};

private:
	typedef std::vector< TimeFactor > TimeFactorList;
	TimeFactorList m_TimeFactorList;

	class FactorSort : public std::binary_function< const TimeFactor&, const TimeFactor&, bool >
	{
	public:
		bool operator()( const TimeFactor &lhs, const TimeFactor &rhs ) const
		{
			return ( lhs.fTimeRate < rhs.fTimeRate );
		}
	};

public:
	void AddTimeFactor( float fTimeRate, float fFactor );

	void SetFactor( int iIdx, float fFactor );
	void RemoveFactor( int iIdx );
	void RemoveAllFactor();

public:
	float GetCurFactor( float fTimeRate ) const;	
	float GetFactor( int iIdx ) const;
	float GetTimeRate( int iIdx ) const;

	int GetFactorCnt() const;

public:
	void SetTimeFactorList( const Vector2Vec &rkList );
	void GetTimeFactorList( Vector2Vec &rkList ) const;	

public:
	ioTimeRateFactor& operator=( const ioTimeRateFactor &rhs );
	bool operator==( const ioTimeRateFactor &rhs ) const;
	bool operator!=( const ioTimeRateFactor &rhs ) const;

public:
	ioTimeRateFactor();
	~ioTimeRateFactor();
};

#endif
