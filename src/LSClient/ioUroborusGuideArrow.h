#pragma once

class ioINILoader;

class ioMapEffect;
class ioPlayStage;

class ioGameEntity;
class ioBaseChar;

class ioUroborusGuideArrow
{
protected:
	ioPlayStage *m_pCreator;

	typedef std::vector< UniqueObjID > ArrowList;
	ArrowList m_ArrowList;

	ioHashString m_GuideBlueArrow;
	ioHashString m_GuideRedArrow;
	ioHashString m_GuideArrow;

	int m_iMaxArrowCount;
	float m_fMaxArrowRange;

	bool m_bStartPosMode;
	D3DXVECTOR3 m_StartPos;

	bool m_bSetStartInvisible;
	bool m_bSetStartCharPos;

	bool m_bDestroyForce;

public:
	void Initialize( ioINILoader &rkLoader );
	void SetMaxArrowRange( float fMaxRange );

	void Process( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos );

	void SetStartPosMode( bool bEnable, D3DXVECTOR3 vStartPos );

	void SetEffectVisble( bool bSetVisble );
	bool IsHasArrowList() const;
	void SetDestroyForceType( bool b );

protected:
	void DestroyArrow();
	bool CheckArrowCreated( ioBaseChar *pOwner );
	void ArrangeArrows( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos );
	void ArrangeArrowsPosMode( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos );

	ioMapEffect* GetEffect( UniqueObjID eObjID );

public:
	ioUroborusGuideArrow( ioPlayStage *pCreator );
	virtual ~ioUroborusGuideArrow();
};

