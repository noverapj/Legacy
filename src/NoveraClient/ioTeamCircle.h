#pragma once


class ioPlayStage;

class ioTeamCircle
{
protected:
	UniqueObjID m_EffectID;
	UniqueObjID m_ShadowEffectID;

	bool m_bMyCircle;

public:
	static ioHashString m_RedTeamCircle;
	static ioHashString m_BlueTeamCircle;
	static ioHashString m_DefaultShadow;

public:
	void SetTeam( ioPlayStage *pStage, TeamType eTeam, const D3DXVECTOR3 &vPos, bool bMyCircle );
	void UpdatePos( ioPlayStage *pStage, const D3DXVECTOR3 &vPos, bool bCircleVisible, bool bShadowVisible );

	void DestroyEffect( ioPlayStage *pStage );

public:
	ioTeamCircle();
	virtual ~ioTeamCircle();
};


