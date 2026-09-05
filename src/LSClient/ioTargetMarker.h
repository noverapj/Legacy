#pragma once

class ioBaseChar;
class ioPlayStage;
class ioMapEffect;

class ioTargetMarker
{
public:
	enum MarkerType
	{
		MT_NONE,
		MT_FIRE_ITEM,
		MT_FLAME_STAFF,
		MT_DYNAMITE,
		MT_HEAL_ITEM,
		MT_BOW_ITEM,
		MT_CROSS_BOW,
		MT_OTHER_MARKER,
	};

	enum MarkerState
	{
		MS_NONE,
		MS_READY,
		MS_AIMING,
		MS_AIMED,
		MS_ZERO_AIMED,
		MS_JUMP_AIM,
		MS_HEAD_AIMED,
		MS_SPECIAL_AIMED,
	};

protected:
	ioBaseChar *m_pOwner;
	MarkerState m_MarkerState;
	
	ioHashString m_Target;
	
	ioHashString m_szReadyEffect;
	ioHashString m_szAimingEffect;
	ioHashString m_szAimedEffect;
	ioHashString m_szZeroAimedEffect;
	ioHashString m_szHeadAimedEffect;
	ioHashString m_szSpecailEffect;

	UniqueObjID m_ReadyID;
	UniqueObjID m_AimingID;
	UniqueObjID m_AimedID;
	UniqueObjID m_ZeroAimedID;
	UniqueObjID m_JumpAimID;
	UniqueObjID m_HeadAimedID;
	UniqueObjID m_SpecailID;

	float m_fMarkerRange;
	float m_fMarkerYOffset;
	float m_fJumpMarkerAngle;

	bool m_bBall;

	bool m_bUseEnable;
	bool m_bPreUseEnable;

public:
	void DestroyMarker( ioPlayStage *pStage );
	void DestroyMarkerEffect( ioPlayStage *pStage, UniqueObjID dwEffectID );

	virtual void SetMarkerState( ioPlayStage *pStage, MarkerState eState );
	virtual void SetTarget( const ioHashString &rkTarget, bool bBall );
	virtual bool IsTarget();

	ioBaseChar* GetTargetChar() const;
	ioTargetMarker::MarkerState GetMarkerState() const;

	void SetMarkerRange( float fRange ) { m_fMarkerRange = fRange; }
	void SetMarkerYOffset( float fYOffset ) { m_fMarkerYOffset = fYOffset; }
	void SetJumpMarkerAngle( float fAngle ) { m_fJumpMarkerAngle = fAngle; }
	void SetUseEnable( bool bEnable );

	inline bool IsBallTarget() const { return m_bBall; }

	void SetMarkerEffect( const ioHashString &szReady, 
						  const ioHashString &szAiming,
						  const ioHashString &szAimed,
						  const ioHashString &szZeroAimed,
						  const ioHashString &szHeadAimed);
	void SetSpecialMarker( const ioHashString &szSpecial );

	ioMapEffect* GetCurMarker( ioPlayStage *pStage );

protected:
	virtual UniqueObjID CreateEffect( ioPlayStage *pStage, const ioHashString &szEffect ) = 0;

public:
	virtual void Update( ioPlayStage *pStage ) = 0;
	virtual MarkerType GetMarkerType() const = 0;

public:
	ioTargetMarker( ioBaseChar *pOwner );
	virtual ~ioTargetMarker();
};

