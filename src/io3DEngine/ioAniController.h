

#ifndef _ioAniController_h_
#define _ioAniController_h_

class ioAnimation;
class ioEntityGroup;
class ioAnimateFX;
class ioAniEventHandler;

class __EX ioAniController
{
public:
	typedef std::list< ioAnimation* >	ioAniList;

private:
	ioEntityGroup *m_pParent;
	ioAniEventHandler *m_pEventHandler;

	ioAniList	m_ActionList;
	ioAniList	m_LoopList;

	typedef std::list< ioAnimateFX* > AnimateFXList;
	AnimateFXList m_AnimateFXList;

	bool m_bAnimationStop;

public:
	static bool m_bGuaranteeAtLeastOneAnimation;

public:
	void Update( float fTime );
	void ExecuteAction( int id,
						float fFadeIn,
						float fFadeOut,
						float fWeight,
						float fTimeFactor,
						float fWaitTime,
						float fPreDelay,
						bool bHoldLastFrame, bool bUseFX = true );

	void ClearAction( int id, float fFadeOut );
	void ClearAllAction( float fFadeOut, bool bAlsoWait );
	void ClearAniEvent();
	void ClearAniEffect();

	void ExecuteLoop( int id, float fFadeIn, float fWeight, float fTimeFactor, float fWaitTime );
	void ExecuteLoopInMid( int id, float fStartTime, float fFadeIn, float fWeight, float fTimeFactor );

	void ClearLoop( int id, float fFadeOut, bool bAlsoWait );
	void ClearAllLoop( float fFadeOut, bool bAlsoWait );
	bool ClearAllLoopExceptOne( int iExceptID, float fFadeOut, bool bWaitAlso );

public:
	void FindActionAniInPlay( int id, ioAniList &vAniList );
	void FindActionAniInPlay( const ioHashString &kAniName, ioAniList &vAniList );

	ioAnimation* FindLoopAniInPlay( const ioHashString &rkName );
	ioAnimation* FindLoopAniInPlay( int id );

	void StopAllPlayingAnimation();
	void ResumeAllPlayingAnimation();

public:
	void SetAniEventHandler( ioAniEventHandler *pHandler );

	void NotifyAniEvent( const AniEventTime *pEvent, const ioAnimation *pNotifier );   //  ioAnimation이 수정될 필요가 없다.
	void ChangeAniEvent( const AniEventTime *pEvent, ioAnimation *pNotifier );		   //  ioAnimation이 수정되어야한다.

	void HideMeshExceptionCheck( const ioHashString &szMeshName );
	
private:
	void NotifyEndEvent( const ioAnimation *pNotifier );
	void NotifyClearEvent( const ioAnimation *pNotifier );

private:
	void UpdateSkeleton( float fTime );
	void UpdateAnimation( float fTime );

	void UpdateAnimateFX( float fTime );
	void ClearEndedAnimation();
	void CheckGuaranteeOut();

public:
	ioAnimation* GetPlayingActionByOrder( int iOrder );
	ioAnimation* GetPlayingLoopByOrder( int iOrder );

public:
	inline int GetPlayingActionCnt() const { return m_ActionList.size(); }
	inline int GetPlayingLoopCnt() const { return m_LoopList.size(); }

public:
	ioAniController( ioEntityGroup *pParent );
	~ioAniController();
};

#endif
