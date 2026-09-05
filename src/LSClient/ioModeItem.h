#pragma once

class SP2Packet;
class ioMapEffect;
class ioPlayStage;

class ioModeItem
{
protected:
	ioPlayStage *m_pCreator;

protected:
	int m_iType;

	UniqueObjID m_dwID;
	
	ioHashString m_MeshEffectName;
	UniqueObjID  m_dwMeshEffectID;

	ioHashString m_DestroyEffectName;

	ioHashString m_CreateSoundName;
	ioHashString m_DestroySoundName;

	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vMoveDirection;

	bool m_bLive;

	float m_fCollisionRange;
	bool  m_bSyncWait;

	ioOrientBox m_AttackBox;
	bool m_bAttach;

	float m_fHeightOffset;
	float m_fWidth;
	float m_fUnderHeight;
	float m_fUpHeight;
	float m_fZoneRange;

	bool m_bVisible;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioModeItem *Clone();
	
	virtual void  OnCreate();
	virtual void OnDestroy();
	
public:
	virtual void SetItemValue( DWORD dwIndex, float fXPos, float fZPos );

public:
	virtual void Process();

public:	
	virtual bool IsCollision( ioBaseChar *pChar );

public:
	inline DWORD GetUniqueID() { return m_dwID; }
	inline void  SetUniqueID( DWORD dwID ) { m_dwID = dwID; }

	inline void SetTypeID( int iType ) { m_iType = iType; }
	inline int  GetTypeID() { return m_iType; }

	inline void SetPlayStage( ioPlayStage *pStage ) { m_pCreator = pStage; }

	virtual void SetPosition( D3DXVECTOR3 vPos );

	virtual D3DXVECTOR3 GetMidPositionByOffset() const;
	virtual D3DXVECTOR3 GetPosition() const;

	inline float GetHeightOffset(){ return m_fHeightOffset; }

protected:
	virtual void SendGetModeItem( ioBaseChar *pChar );

public:
	virtual bool IsAttach();
	virtual void UpdateAttackBox();
	virtual const ioOrientBox& GetAttackBox() const;
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

public:
	inline void SetReserveDestroy() { m_bLive = false; }
	inline bool IsLive() { return m_bLive; }
	inline bool IsVisible()	{ return m_bVisible; }

public:
	ioModeItem();
	ioModeItem( const ioModeItem &rhs );
	virtual ~ioModeItem();
};