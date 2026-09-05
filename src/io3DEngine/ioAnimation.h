

#ifndef _ioAnimation_h_
#define _ioAnimation_h_

class ioRcAnimation;
class ioAniController;
class ioSkeleton;
class ioAniTrack;

#include "ioMemoryPool.h"

class __EX ioAnimation : public ioPoolObject< ioAnimation >
{
public:
	enum AniType
	{
		AT_ACTION = 0,
		AT_LOOP
	};

	enum AniState
	{
		AS_WAIT = 0,
		AS_FADE_IN,
		AS_PLAY,
		AS_FADE_OUT,
		AS_ACT_FORCE_OUT,
		AS_END,

		// 다른 애니메이션간의 끊김을 방지하기 위한 좀비상태
		AS_ACT_GUARANTEE,
		AS_ACT_GUARANTEE_OUT,
		AS_ACT_GUARANTEE_END,
	};

protected:
	friend class ioAnimationManager;

protected:
	ioRcAnimation	*m_pRcAnimation;

	typedef std::vector< ioAniTrack* > AniTrackList;
	AniTrackList m_AniTrackList;

	ioAniController *m_pAniCtrl;

	float	m_fFullTime;
	float	m_fWaitTime;

	float   m_fPreTime;
	float	m_fCurTime;
	float	m_fMidStartTime;

	float	m_fTargetWeight;
	float	m_fCurWeight;
	float	m_fTimeFactor;

	AniType		m_eAniType;
	AniState	m_eAniState;

	float	m_fFadeIn;
	float	m_fFadeOut;

	float	m_fActPreDelay;
	float	m_fActPreDelayPast;

	float	m_fActForceOut;
	float	m_fLoopFadeOut;

	float	m_fGuaranteeOut;

	bool	m_bAniStop;
	bool	m_bHoldLastFrame;

	mutable bool	m_bNeedEndNotify;

public:
	void NotifyAttached( ioAniController *pParent );
	void BuildAniTrackList( ioSkeleton *pSkel );

	void ExecuteAni( float fWeight,
					 float fTimeFactor,
					 float fFadeIn,
					 float fFadeOut,
					 float fWaitTime,
					 float fPreDelay,
					 bool bHoldLastFrame );

	void ForceEndAction( float fFadeOut );

	void StartLoop( float fWeight,
					float fFadeIn,
					float fTimeFactor,
					float fWaitTime );	// start, reset ani

	void StartLoopInMid( float fStartTime,
						 float fWeight,
						 float fFadeIn,
						 float fTimeFactor );

	void EndLoop( float fFadeOut, bool bAlsoWait );

public:
	void SetCurTime( float fTime );
	void ForceWeight( float fWeight );

	void StopAni() { m_bAniStop = true; }
	void ResumeAni() { m_bAniStop = false; }

	void SetGuaranteeState();
	void SetGuaranteeOut();

public:
	void Update( float fTime );
	void BlendSkeleton( ioSkeleton *pSkeleton );
	void UpdateAniEvent();

protected:
	void UpdateActionAni( float fTime );
	void UpdateLoopAni( float fTime );
	void ClearAniTrackList();

public:
	inline void SetEndEventNotify() const { m_bNeedEndNotify = true; }
	inline bool IsNeedEndEventNotify() const { return m_bNeedEndNotify; }

public:
	inline int GetTrackCount() const { return m_AniTrackList.size(); }

	float GetTrackWeight( int iTrackID ) const;
	float GetTrackWeight( const ioHashString &kName ) const;

public:
	ioAniTrack* GetAniTrack( int iTrackID ) const;
	ioAniTrack* GetAniTrack( const ioHashString &rkBipedName ) const;

public:
	inline float GetTimeRate() const { return m_fCurTime / m_fFullTime; }
	inline float GetFullTime() const { return m_fFullTime; }
	inline float GetCurTime() const { return m_fCurTime; }
	inline float GetWeight() const { return m_fCurWeight; }
	inline float GetTimeFactor() const { return m_fTimeFactor; }

	inline bool IsWaiting() const { return ( m_eAniState == AS_WAIT ); }
	inline AniType  GetType() const { return m_eAniType; }
	inline AniState GetState() const { return m_eAniState; }

	const ioHashString& GetName() const;

public:
	float GetEventTime( const ioHashString &szEventType ) const;
	int GetEventCnt() const;

protected:
	ioAnimation( ioRcAnimation *pRcAnimation );

public:
	virtual ~ioAnimation();
};

#endif
