#pragma once


class ioBaseChar;
class ioPlayStage;

class FindPredicate
{
protected:
	const ioBaseChar *m_pOwner;

protected:
	struct CharDistanceSq
	{
		ioBaseChar *pChar;
		float fDistanceSq;

		bool operator<( const CharDistanceSq &rhs ) const
		{
			return fDistanceSq < rhs.fDistanceSq;
		}
	};
	typedef std::vector< CharDistanceSq > CharDistanceList;

	struct CharAngle
	{
		ioBaseChar *pChar;
		float fHalfCosine;

		bool operator<( const CharAngle &rhs ) const
		{
			return fHalfCosine < rhs.fHalfCosine;
		}
	};
	typedef std::vector< CharAngle > CharAngleSortList;

protected:
	static CharDistanceList m_vDistanceSortList;
	static CharAngleSortList m_vAngleSortList;

public:
	virtual ioBaseChar* GetFindChar( const BaseCharList &rkCandidateList ) = 0;

protected:
	void MakeMostCloseSortList( const BaseCharList &rkCandidateList, float fLimitDistSq );
	void MakeMostCloseSortList( const BaseCharList &rkCandidateList, float fMinDistSq, float fLimitDistSq );
	void MakeSmallestAngleSortList( const BaseCharList &rkCandidateList, float fLimitDistSq, float fMaxHalfCosine );

protected:
	bool IsExperienceOwner();

public:
	FindPredicate( const ioBaseChar *pOwner );
	virtual ~FindPredicate();
};

//-----------------------------------------------------------------------------

class FindListPredicate
{
public:
	virtual bool FindList( const GameEntityList &rkList, ioPlayStage *pStage ) = 0;

public:
	FindListPredicate();
	virtual ~FindListPredicate();
};

//-----------------------------------------------------------------------------

namespace FindPredicateUtil
{
	bool CheckTargetState( ioBaseChar* pTarget, bool bDownTargetFind, bool bHideTargetFind = true );
}