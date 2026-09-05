#pragma once

#include "ioCameraEvent.h"

class ioINILoader;

class ioSimpleEvent : public ioCameraEvent
{
public:
	enum SimpleEventState
	{
		SE_NONE,
		SE_ZOOMIN,
		SE_ZOOMSTAY,
		SE_ZOOMOUT
	};

protected:
	D3DXVECTOR3 m_vStartPosition;
	D3DXVECTOR3 m_vTargetPosition;

	float m_fZoomDistance;
	float m_fCameraDistance;
	float m_fFov;
	float m_fStartFov;
	float m_fCurFov;
	DWORD m_dwZoomInTime;
	DWORD m_dwZoomOutTime;
	DWORD m_dwStayTime;

	DWORD m_dwZoomInStartTime;
	DWORD m_dwZoomStayStartTime;
	DWORD m_dwZoomOutStartTime;

	DWORD m_dwEventState;

	D3DXQUATERNION m_qtTarget;
	D3DXQUATERNION m_qtCurRot;
	
	float m_fRotAngleX;
	float m_fCurAngleX;

	float m_fRotAngleY;
	float m_fCurAngleY;

	float m_fRotAngleZ;
	float m_fCurAngleZ;

	float m_fOffSetLength;
	float m_fOffSetHeight;

	D3DXVECTOR3 m_vLookAt;
	bool m_bSetOffSet;
	bool m_bLookTargetChar;
	bool m_bAutoRepeat;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual CameraEvnetType GetType() const;
	virtual ioCameraEvent* Clone();

	virtual D3DXVECTOR3 ProcessCameraEvent( D3DXVECTOR3 vLookAt,
											D3DXVECTOR3 vPosition,
											float fFov,
											D3DXQUATERNION qtTarget );

	virtual D3DXVECTOR3 CheckOffSet( D3DXVECTOR3 vLookAt, D3DXQUATERNION qtTarget );
	
	virtual bool  IsForceLookTargetChar() const { return m_bLookTargetChar; }
	virtual float GetCameraFov() { return m_fCurFov; }
	virtual float GetCameraRoll() { return m_fCurAngleZ; }

protected:
	D3DXVECTOR3 ProcessZoomIn( D3DXVECTOR3 vLookAt,
							   D3DXVECTOR3 vPosition,
							   float fFov,
							   D3DXQUATERNION qtTarget );

	D3DXVECTOR3 ProcessZoomStay( D3DXVECTOR3 vLookAt,
								 D3DXVECTOR3 vPosition,
								 float fFov,
								 D3DXQUATERNION qtTarget );

	D3DXVECTOR3 ProcessZoomOut( D3DXVECTOR3 vLookAt,
								D3DXVECTOR3 vPosition,
								float fFov,
								D3DXQUATERNION qtTarget );

	void CheckCameraRot( D3DXQUATERNION qtTarget );

public:
	ioSimpleEvent();
	ioSimpleEvent( const ioSimpleEvent &rhs );
	virtual ~ioSimpleEvent();
};

inline ioSimpleEvent* ToSimpleEvent( ioCameraEvent *pCameraEvent )
{
	if( !pCameraEvent || pCameraEvent->GetType() != ioCameraEvent::CET_SIMPLE )
		return NULL;

	return dynamic_cast< ioSimpleEvent* >( pCameraEvent );
}

