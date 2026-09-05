#pragma once

class ioEffect;

class ioMapEffect
{
protected:
	ioEffect *m_pEffect;

public:
	virtual void Update( float fTimeGap );
	virtual bool IsLive() const;

public:
	void SetWorldPosition( const D3DXVECTOR3 &vPos );
	void SetWorldOrientation( const D3DXQUATERNION &qtRot );
	void SetEffectDirection( const D3DXVECTOR3 &vDir );

	void Rotate( const D3DXVECTOR3 &vAxis, float fDegrees );

	void SetVisible( bool bVisible );
	void SetAlphaRate( int iAlpha );
	void SetRenderQueueGroup( RenderGroupID eGroup );

	void ReserveEffectStop( float fStopDuration, float fStopLooseRate );

public:
	void EndEffectForce();
	void EndEffectGrace();
	
public:
	UniqueObjID GetUniqueID() const;
	const D3DXVECTOR3& GetWorldPosition() const;
	const D3DXQUATERNION& GetWorldOrientation() const;

public:
	void Translate( const D3DXVECTOR3 &vMove );

public:
	inline ioEffect* GetEffect() const { return m_pEffect; }

public:
	ioMapEffect( ioEffect *pEffect );
	virtual ~ioMapEffect();
};


