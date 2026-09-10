#pragma once

class ioPlayStage;

class ioMagicCircle
{
public:
	enum MagicCircleState
	{
		MCS_NONE,
		MCS_NORMAL,
		MCS_NORMAL_FIX,
		MCS_SPECIAL,
		MCS_SPECIAL_FIX,
	};

private:
	ioHashString m_ReadyCircle;
	ioHashString m_EnableCircle;
	ioHashString m_GuidCircle;

	UniqueObjID m_dwReadyEffect;
	UniqueObjID m_dwEnableEffect;

	D3DXVECTOR3 m_vStartPos;
	D3DXVECTOR3 m_vCurPos;
	D3DXVECTOR3 m_vPrePos;
	D3DXQUATERNION m_TargetRotate;

	bool m_bEnableSkill;
	bool m_bOtherVisible;
	bool m_bEnableRender;

	DWORD m_dwVisibleTime;

// 갤럭시 망토를 위한 추가 사항들
protected:
	MagicCircleState m_MagicCircleState;

	ioHashString m_GalaxyNormal;
	ioHashString m_GalaxyNormalFix;
	ioHashString m_GalaxySpecial;
	ioHashString m_GalaxySpecialFix;

	UniqueObjID m_dwCurGalaxyEffect;

public:
	void InitResource( const ioHashString &ReadyCircle,
					   const ioHashString &EnableCircle,
					   const ioHashString &GuidCircle,
					   const D3DXVECTOR3 &vPos,
					   ioPlayStage *pStage );

	void SetEnableRender( bool bEnable );
	void SetVisible( bool bVisible );
	void SetTargetRot( D3DXQUATERNION qtRotate );
	void SetVisibleTime( ioPlayStage *pStage );

	void DestroyCircle( ioPlayStage *pStage );

public:
	void UpdatePos( bool bEnableSkill, const D3DXVECTOR3 &vPos, ioBaseChar *pOwner, ioPlayStage *pStage );

	void SetGalaxyCircle( const ioHashString &szGalaxyNormal,
						  const ioHashString &szGalaxyNormalFix,
						  const ioHashString &szGalaxySpecial,
						  const ioHashString &szGalaxySpecialFix,
						  ioPlayStage *pStage );

	void ChangeGalaxyCircle( MagicCircleState eState, ioPlayStage *pStage );

protected:
	void UpdateNonePos( bool bEnableSkill, const D3DXVECTOR3 &vPos, ioBaseChar *pOwner, ioPlayStage *pStage );
	void UpdateGalaxyPos( const D3DXVECTOR3 &vPos, ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	inline bool IsCanRender() { return m_bEnableRender; }
	inline bool GetVisible() { return m_bOtherVisible; }
	inline D3DXVECTOR3 GetStartPos() { return m_vStartPos; }
	inline D3DXVECTOR3 GetCurPos() { return m_vCurPos;	}
	inline D3DXVECTOR3 GetPrePos() { return m_vPrePos;	}
	inline D3DXQUATERNION GetTargetRot() { return m_TargetRotate; }

	UniqueObjID GetReadyCircelID() const;

public:
	ioMagicCircle();
	~ioMagicCircle();
};

