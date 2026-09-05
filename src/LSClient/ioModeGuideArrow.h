#pragma once

class ioINILoader;

class ioMapEffect;
class ioPlayMode;

class ioGameEntity;
class ioBaseChar;

class ioModeGuideArrow
{
protected:
	ioPlayMode *m_pCreator;

	typedef std::vector< UniqueObjID > ArrowList;
	ArrowList m_ArrowList;

	ioHashString m_ModeGuideBlueArrow;
	ioHashString m_ModeGuideRedArrow;
	ioHashString m_ModeGuideArrow;

	float m_fMaxArrowRange;
	float m_fModeGuideScale;
	int m_iMaxArrowCount;

public:
	void Initialize( ioINILoader &rkLoader );
	void Process( ioBaseChar *pOwner, ioGameEntity *pTarget );
	void Process( ioBaseChar *pOwner, ioGameEntity *pTarget, TeamType eTeamType );
	void Process( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos, TeamType eTeamType );

	void SetVisible( bool bVisible );

protected:
	void DestroyArrow();
	bool CheckArrowCreated( TeamType eTeam );
	bool CheckGuideHide( ioBaseChar *pOwner );
	void ArrangeArrows( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos );

	ioMapEffect* GetEffect( UniqueObjID eObjID );

public:
	ioModeGuideArrow( ioPlayMode *pCreator );
	virtual ~ioModeGuideArrow();
};

