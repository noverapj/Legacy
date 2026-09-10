

#include "stdafx.h"

#include "ioSimpleEvent.h"

ioSimpleEvent::ioSimpleEvent()
{
	m_fZoomDistance = 0.0f;
	m_dwZoomInStartTime = 0;
	m_dwZoomStayStartTime = 0;
	m_dwZoomOutStartTime = 0;
	m_fCurFov = 0.0f;
	m_fStartFov = 0.0f;
	m_fCurAngleX = 0.0f;
	m_fCurAngleY = 0.0f;
	m_fCurAngleZ = 0.0f;
	m_bCameraEventStart = false;
	m_bCameraEventEnd = false;
	m_bSetOffSet = false;
	m_bLookTargetChar = false;
	m_bAutoRepeat = false;

	D3DXQuaternionIdentity( &m_qtTarget );
	m_dwEventState = SE_NONE;
}

ioSimpleEvent::ioSimpleEvent( const ioSimpleEvent &rhs )
: ioCameraEvent( rhs ),
 m_fCameraDistance( rhs.m_fCameraDistance ),
 m_fFov( rhs.m_fFov ),
 m_dwZoomInTime( rhs.m_dwZoomInTime ),
 m_dwZoomOutTime( rhs.m_dwZoomOutTime ),
 m_dwStayTime( rhs.m_dwStayTime ),
 m_fRotAngleX( rhs.m_fRotAngleX ),
 m_fRotAngleY( rhs.m_fRotAngleY ),
 m_fRotAngleZ( rhs.m_fRotAngleZ ),
 m_fOffSetHeight( rhs.m_fOffSetHeight ),
 m_fOffSetLength( rhs.m_fOffSetLength ),
 m_bAutoRepeat( rhs.m_bAutoRepeat )
{
	m_iEventCode = rhs.m_iEventCode;
	m_bLookTargetChar = rhs.m_bLookTargetChar;

	m_dwZoomInStartTime = 0;
	m_dwZoomStayStartTime = 0;
	m_dwZoomOutStartTime = 0;
	m_dwEventState = SE_NONE;
	m_bCameraEventStart = false;
	m_bCameraEventEnd = false;
	m_bSetOffSet = false;
	m_fZoomDistance = 0.0f;
	m_fCurFov = 0.0f;
	m_fStartFov = 0.0f;
	m_fCurAngleX = 0.0f;
	m_fCurAngleY = 0.0f;
	m_fCurAngleZ = 0.0f;
	D3DXQuaternionIdentity( &m_qtTarget );
}

ioSimpleEvent::~ioSimpleEvent()
{
	m_bSetOffSet = false;
}

ioCameraEvent* ioSimpleEvent::Clone()
{
	return new ioSimpleEvent( *this );
}

void ioSimpleEvent::LoadProperty( ioINILoader &rkLoader )
{
	ioCameraEvent::LoadProperty( rkLoader );
	
	m_fCameraDistance = rkLoader.LoadFloat_e( "camera_distance", 200.0f );
	m_fRotAngleX = rkLoader.LoadFloat_e( "camera_rotate_x", 0.0f );
	m_fRotAngleY = rkLoader.LoadFloat_e( "camera_rotate_y", 0.0f );
	m_fRotAngleZ = rkLoader.LoadFloat_e( "camera_rotate_z", 0.0f );

	m_fFov = rkLoader.LoadFloat_e( "camera_fov", 50.0f );
	m_dwZoomInTime = rkLoader.LoadInt_e( "zoom_in_time", 1000 );
	m_dwZoomOutTime = rkLoader.LoadInt_e( "zoom_out_time", 1000 );
	m_dwStayTime = rkLoader.LoadInt_e( "zoom_stay_time", 2000 );

	m_fOffSetLength = rkLoader.LoadFloat_e( "offset_length", 0.0f );
	m_fOffSetHeight = rkLoader.LoadFloat_e( "offset_height", 0.0f );

	m_bLookTargetChar = rkLoader.LoadBool_e( "look_target_char", false );
	m_bAutoRepeat     = rkLoader.LoadBool_e( "auto_repeat", false );
}

ioCameraEvent::CameraEvnetType ioSimpleEvent::GetType() const
{
	return CET_SIMPLE;
}

D3DXVECTOR3 ioSimpleEvent::ProcessZoomIn( D3DXVECTOR3 vLookAt,
										  D3DXVECTOR3 vPosition,
										  float fFov,
										  D3DXQUATERNION qtTarget )
{
	float fLength;
	D3DXVECTOR3 vNewPosition;

	m_vStartPosition = vPosition;
	m_vTargetPosition = vLookAt;	

	if( !m_bCameraEventStart )
	{
		m_dwZoomInStartTime = FRAMEGETTIME();
		m_bCameraEventStart = true;
		m_fZoomDistance = D3DXVec3Length( &(m_vTargetPosition - m_vStartPosition) );
		m_fZoomDistance -= m_fCameraDistance;
		m_fStartFov = fFov;
		m_qtTarget = qtTarget;
		m_fCurAngleX = 0.0f;
		m_fCurAngleY = 0.0f;
		m_fCurAngleZ = 0.0f;
	}

	D3DXVECTOR3 vDir = m_vTargetPosition - m_vStartPosition;
	D3DXVec3Normalize( &vDir, &vDir );

	if( m_dwZoomInStartTime > 0 && m_dwZoomInTime > 0 )
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwZoomInStartTime;
		float fRate = (float)dwGap / (float)m_dwZoomInTime;
		fRate = max( 0.0f, min( fRate, FLOAT1 ) );

		if( fRate < FLOAT1 )
		{
			fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

			fLength = m_fZoomDistance * (1.0f - fRate);
			vNewPosition = m_vStartPosition + vDir * fLength;
			m_fCurFov = m_fStartFov + (m_fFov - m_fStartFov) * (1.0f - fRate);
			m_fCurAngleX = m_fRotAngleX * ( FLOAT1 - fRate );
			m_fCurAngleY = m_fRotAngleY * ( FLOAT1 - fRate );
			m_fCurAngleZ = m_fRotAngleZ * ( FLOAT1 - fRate );
		}
		else
		{
			vNewPosition = m_vStartPosition + vDir * m_fZoomDistance;
			m_fCurFov = m_fFov;
			m_fCurAngleX = m_fRotAngleX;
			m_fCurAngleY = m_fRotAngleY;
			m_fCurAngleZ = m_fRotAngleZ;
			m_dwZoomInStartTime = 0;
			//change state
			m_dwZoomStayStartTime = FRAMEGETTIME();
			m_dwEventState = SE_ZOOMSTAY;
		}
	}
	else
	{
		vNewPosition = m_vStartPosition + vDir * m_fZoomDistance;
		m_fCurFov = m_fFov;
		m_fCurAngleX = m_fRotAngleX;
		m_fCurAngleY = m_fRotAngleY;
		m_fCurAngleZ = m_fRotAngleZ;
		//change state
		m_dwZoomStayStartTime = FRAMEGETTIME();
		m_dwEventState = SE_ZOOMSTAY;
	}

	vDir = vNewPosition - m_vTargetPosition;
	CheckCameraRot( m_qtTarget );
	vDir = m_qtCurRot * vDir;
	vNewPosition = m_vTargetPosition + vDir;
	return vNewPosition;
}

D3DXVECTOR3 ioSimpleEvent::ProcessZoomOut( D3DXVECTOR3 vLookAt,
										   D3DXVECTOR3 vPosition,
										   float fFov,
										   D3DXQUATERNION qtTarget )
{
	float fLength;
	D3DXVECTOR3 vNewPosition;

	if( !m_bCameraEventStart )
		return vPosition;
	
	m_vStartPosition = vPosition;
	m_vTargetPosition = vLookAt;
	D3DXVECTOR3 vDir = m_vTargetPosition - m_vStartPosition;
	D3DXVec3Normalize( &vDir, &vDir );

	if( m_dwZoomOutStartTime > 0 && m_dwZoomOutTime > 0 )
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwZoomOutStartTime;
		float fRate = (float)dwGap / (float)m_dwZoomOutTime;

		fRate = max( 0.0f, min( fRate, FLOAT1 ) );

		if( fRate < FLOAT1 )
		{
			fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

			fLength = m_fZoomDistance * fRate;
			vNewPosition = m_vStartPosition + vDir * fLength;
			m_fCurFov = m_fStartFov + (m_fFov - m_fStartFov) * fRate;
			m_fCurAngleX = m_fRotAngleX * fRate;
			m_fCurAngleY = m_fRotAngleY * fRate;
			m_fCurAngleZ = m_fRotAngleZ * fRate;
		}
		else
		{
			vNewPosition = m_vStartPosition;
			m_fCurFov = m_fStartFov;
			m_fCurAngleX = 0.0f;
			m_fCurAngleY = 0.0f;
			m_fCurAngleZ = 0.0f;
			m_dwZoomOutStartTime = 0;
			m_bCameraEventStart = false;
			//change state
			m_dwEventState = SE_NONE;
			if( !m_bAutoRepeat )  
				m_bCameraEventEnd = true;
		}
	}
	else
	{
		vNewPosition = m_vStartPosition;
		m_fCurFov = m_fStartFov;
		m_fCurAngleX = 0.0f;
		m_fCurAngleY = 0.0f;
		m_fCurAngleZ = 0.0f;
		m_bCameraEventStart = false;
		//change state
		m_dwEventState = SE_NONE;
		if( !m_bAutoRepeat )  
			m_bCameraEventEnd = true;
	}

	vDir = vNewPosition - m_vTargetPosition;
	CheckCameraRot( m_qtTarget );
	vDir = m_qtCurRot * vDir;
	vNewPosition = m_vTargetPosition + vDir;
	
	return vNewPosition;
}

D3DXVECTOR3 ioSimpleEvent::ProcessZoomStay( D3DXVECTOR3 vLookAt,
										    D3DXVECTOR3 vPosition,
										    float fFov,
											D3DXQUATERNION qtTarget )
{
	D3DXVECTOR3 vNewPosition;
	
	if( !m_bCameraEventStart )
		return vPosition;

	m_vStartPosition = vPosition;
	m_vTargetPosition = vLookAt;
	D3DXVECTOR3 vDir = m_vTargetPosition - m_vStartPosition;
	D3DXVec3Normalize( &vDir, &vDir );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwZoomStayStartTime > 0 )
	{
		if( m_dwZoomStayStartTime + m_dwStayTime < dwCurTime )
		{
			m_dwZoomStayStartTime = 0;
			//change state
			m_dwZoomOutStartTime = FRAMEGETTIME();
			m_dwEventState = SE_ZOOMOUT;
		}
	}
	else
	{
		//change state
		m_dwZoomOutStartTime = FRAMEGETTIME();
		m_dwEventState = SE_ZOOMOUT;
	}

	vNewPosition = m_vStartPosition + vDir * m_fZoomDistance;
	vDir = vNewPosition - m_vTargetPosition;
	vDir = m_qtCurRot * vDir;
	vNewPosition = m_vTargetPosition + vDir;

	return vNewPosition;
}

D3DXVECTOR3 ioSimpleEvent::ProcessCameraEvent( D3DXVECTOR3 vLookAt,
											   D3DXVECTOR3 vPosition,
											   float fFov,
											   D3DXQUATERNION qtTarget  )
{
	switch( m_dwEventState )
	{
	//HARDCODE: event가 종료된 상태가 아니면 SE_ZOOMIN state로 바로 전환된다.
	case SE_NONE:
		if( m_bCameraEventEnd )
			return vPosition;
	case SE_ZOOMIN:
		return ProcessZoomIn( vLookAt, vPosition, fFov, qtTarget );
	case SE_ZOOMSTAY:
		return ProcessZoomStay( vLookAt, vPosition, fFov, qtTarget );
	case SE_ZOOMOUT:
		return ProcessZoomOut( vLookAt, vPosition, fFov, qtTarget );
	}

	LOG.PrintTimeAndLog( 0, "ioSimpleEvent::ProcessCameraEvent - UnkonwState" );
	return vPosition;
}

void ioSimpleEvent::CheckCameraRot( D3DXQUATERNION qtTarget )
{
	bool bLeftRot = true;
	D3DXVECTOR3 vDir = qtTarget * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if( vDir.z == 0.0f )
	{
		D3DXQuaternionRotationYawPitchRoll( &m_qtCurRot, DEGtoRAD( 0.0f ), 0.0f, 0.0f );
		return;
	}

	if( (vDir.x >= 0.0f && vDir.z > 0.0f) ||
		(vDir.x < 0.0f && vDir.z < 0.0f) )
	{
		bLeftRot = false;
	}

	if( bLeftRot )
		D3DXQuaternionRotationYawPitchRoll( &m_qtCurRot, DEGtoRAD(m_fCurAngleX), DEGtoRAD(m_fCurAngleY), 0.0f );
	else
		D3DXQuaternionRotationYawPitchRoll( &m_qtCurRot, DEGtoRAD(-m_fCurAngleX), DEGtoRAD(m_fCurAngleY), 0.0f );

}

D3DXVECTOR3 ioSimpleEvent::CheckOffSet( D3DXVECTOR3 vLookAt, D3DXQUATERNION qtTarget )
{
//	if( !m_bSetOffSet )
//	{
		D3DXVECTOR3 vDir = qtTarget * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		vDir = vDir * m_fOffSetLength;
		vDir.y += m_fOffSetHeight;
		
		m_vLookAt = vLookAt + vDir;
		m_bSetOffSet = true;
//	}

	return m_vLookAt;
}

