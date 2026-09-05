#pragma once


#include "ioNpcDefine.h"
#include "ioActionController.h"
#include "ioMoveController.h"
#include "ioJumpController.h"
#include "ioDefenseController.h"

#define SIGHT_360  360.0f
class ioAI
{
protected:
	AIType		m_AIType;

	ioNpcChar    *m_pCreator;
	ioPlayStage  *m_pPlayStage;

protected:
	float m_fSearchTargetTime;
	float m_fSearchRangeMin;
	float m_fSearchRangeMax;
	float m_fSearchRangeUpByCylinder;
	float m_fSearchRangeDownByCylinder;
	float m_fSearchRangeCurrent;
	float m_fJumpCheckRange;
	float m_fTargetSearchAngle;
	float m_fObjectSearchAngle;
	float m_fEscapeRange;
	bool  m_bCheckDownState;
	int   m_iOffensiveLevel;
	bool  m_bRoaming;

protected:
	float m_fCurTargetFormTime;

protected:
	// 고정 어그로 : 해당 시간 동안은 고정 어그로 유저만 공격한다.
	float m_fCurTargetFixingTime;
	float m_fTargetFixingTime;
	ioHashString m_szFixingName;

	bool  m_bGlobalTarget;
	float m_fGlobalTargetSearchTimeMin;
	float m_fGlobalTargetSearchTimeMax;
	float m_fGlobalTargetSearchTimeRand;
	float m_fCurGlobalTargetSearchTime;
	float m_fGlobalTargetFixingTimeMin;
	float m_fGlobalTargetFixingTimeMax;
	bool  m_bGlobalTargetRand;
	ioHashStringVec m_vGlobalTargetBuff;

protected:
	// 특정 지역 이동 루트
	typedef std::vector< D3DXVECTOR3 > vWayPoint;
	vWayPoint m_WayPointList;

protected:	
	float m_fCurSearchTime;
	
protected:       // 타겟 정보
	enum TargetType
	{
		TARGET_NONE     = 0,
		TARGET_BASECHAR,
		TARGET_OBJECT,
		TARGET_POSITION,
		TARGET_ROAMING,
		TARGET_WAYPOINT,
		TARGET_ESCAPE,
	};

	enum 
	{
		SEARCH_TOP_DAMAGE,
		SEARCH_LOWEST_DAMAGE,
		SEARCH_RANGE,
		SEARCH_MODE_CONDITION,
		MAX_SEARCH_TYPE,
		SEARCH_RANDOM,    // 랜덤은 Max다음에 있어야 랜덤 돌릴 때 랜덤이 걸리지 않는다.
	};
	
	struct TargetSearchData
	{
		float m_fHPRate;
		DWORD m_dwSearchType;
		TargetSearchData()
		{
			m_fHPRate = 0.0f;
			m_dwSearchType = 0;
		}
	};
	typedef std::vector< TargetSearchData > vTargetSearchData;
	vTargetSearchData m_TargetSearchData;

	struct TargetData
	{
	protected:
		enum { MAX_ABUSE_SEND_TIME = 3000, };
	protected:
		TargetType   m_TargetType;
		ioHashString m_szTargetName;
		D3DXVECTOR3  m_vTargetPos;
		int          m_iTargetLevel;
		DWORD        m_dwCurrentTime;
		
		CEncrypt<DWORD> m_dwCheckTargetType;
		CEncrypt<DWORD> m_dwCheckTargetName;
		CEncrypt<DWORD> m_dwAbuseSendTime;

	public:
		TargetData()
		{
			m_dwCurrentTime   = 0;
			m_dwAbuseSendTime = 0;
			Init();
		}
		void Init();

		ioAI::TargetType GetTargetType() const { return m_TargetType; }
		ioHashString &GetTargetName() { return m_szTargetName; }
		D3DXVECTOR3 &GetTargetPos() { return m_vTargetPos; }
		int GetTargetLevel() const { return m_iTargetLevel; }
		DWORD GetCurrentTime() const { return m_dwCurrentTime; }

		void SetTargetType( TargetType eTargetType );
		void SetTargetName( const ioHashString &rsTargetName );
		void SetTargetPos( const D3DXVECTOR3 &vPos ){ m_vTargetPos = vPos;}
		void SetTargetLevel( int iTargetLevel )	{m_iTargetLevel = iTargetLevel;	}
		void SetCurrentTime(DWORD dwCurrentTime) { m_dwCurrentTime = dwCurrentTime; }

	};	
	TargetData m_TargetData;
	DWORD      m_dwSearchType;

protected:       // 액션 정보
	ioActionController	m_ActionController;
	ioMoveController	m_MoveController;
	ioJumpController	m_JumpController;
	ioDefenseController	m_DefenseController;

protected:       // 타겟 제외
	DWORDVec		m_vTargetIgnoreState;
	ioHashStringVec m_vTargetIgnoreBuff;

protected:
	float GetTargetAngle( D3DXVECTOR3 vTargetPos );
	float GetTargetRange( D3DXVECTOR3 vPos );
	float GetTargetRange( D3DXVECTOR3 vPos1, D3DXVECTOR3 vPos2 );
	D3DXVECTOR3 GetTargetLeftPos( const D3DXQUATERNION &qtTargetRotate, const D3DXVECTOR3 &vTargetPos, float fMoveRange );
	D3DXVECTOR3 GetTargetRightPos( const D3DXQUATERNION &qtTargetRotate, const D3DXVECTOR3 &vTargetPos, float fMoveRange );
	bool CheckLeftRightPos(  const D3DXQUATERNION &qtTargetRotate, const D3DXVECTOR3 &vTargetPos, float fMoveRange, 
							 D3DXVECTOR3 &vPrevTargetPos, D3DXVECTOR3 &vReturnPos );
	D3DXVECTOR3 GetRoamingPoint();

protected:
	float GetGlobalTargetSearchMinMaxGapTime(){ return max( FLOAT1, m_fGlobalTargetSearchTimeMax - m_fGlobalTargetSearchTimeMin ); }
	float GetGlobalTargetFixingMinMaxGapTime(){ return max( FLOAT1, m_fGlobalTargetFixingTimeMax - m_fGlobalTargetFixingTimeMin ); }

	void CheckRandomTarget( ioHashString &rkReturnName );
	void CheckReSearchFixingTarget();
	void CheckGlobalTarget();
	void CheckFixingTarget();
	void CheckRandomAggroTarget();
	void CheckWayPoint();

protected:
	virtual bool OnClearAction();
	virtual bool IsActionToMove();
	virtual bool OnMove();
	virtual bool OnJump();
	virtual bool OnDefense();
	virtual bool OnAction();

public:
	virtual void LoadAI( ioINILoader &rkLoader );

public:
	virtual void ProcessAI();
	void ProcessAIforAOS();

public:
	bool IsTargetIgnoreState( ioBaseChar *pChar, bool bPassBuffCheck = true );
	virtual bool IsRoaming();
	virtual bool IsOffensive();
	virtual void CheckTargetChar();

public:
	float GetAngle( D3DXVECTOR3 vPos1, D3DXVECTOR3 vPos2 );

public:
	void SetFixingTarget( const ioHashString &rkTargetName, float fFixingTime );

public:
	bool IsTargetBaseChar(){ return ( m_TargetData.GetTargetType() == TARGET_BASECHAR ); }
	D3DXVECTOR3 &GetTargetPos(){ return m_TargetData.GetTargetPos(); }
	ioHashString &GetTargetName(){ return m_TargetData.GetTargetName(); }

public:
	bool IsWayPoint(){ return !m_WayPointList.empty(); }

public:
	virtual AIType GetAIType() const;

public:
	ioAI( ioNpcChar *pCreator, ioPlayStage *pPlayStage );
	virtual ~ioAI();
};




class ioAIPathFind:public ioAI
{

public:
	void ProcessAI();


	ioAIPathFind( ioNpcChar *pCreator, ioPlayStage *pPlayStage );
	virtual ~ioAIPathFind();

protected:
	enum eState
	{
		S_STAND, S_NAV, S_ATTACK,
	};

	eState m_eState;

	virtual bool OnMove();
	virtual void CheckTargetChar();

private:
	float GetDirection(const D3DXVECTOR3& vTarget);
	int findBoxPath( const D3DXVECTOR3& posFrom, const D3DXVECTOR3& posTo, BoxPath& outPath );
	bool getNextWaypoint( BoxPath& path, const D3DXVECTOR3& posCur, const D3DXVECTOR3& posGoal, D3DXVECTOR3& outWaypoint, char& outType ) const;


	D3DXVECTOR3 m_vecWayPts;
	float		m_fAngle;
	BoxPath		m_cBoxPath;
	DWORD		m_dwFindPathTime;
};