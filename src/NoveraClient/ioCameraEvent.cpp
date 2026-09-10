

#include "stdafx.h"

#include "ioCameraEvent.h"

ioCameraEvent::ioCameraEvent()
{
	m_pCreator = NULL;
	m_bCameraEventStart = false;
	m_bCameraEventEnd = false;
}

ioCameraEvent::ioCameraEvent( const ioCameraEvent &rhs )
: m_pCreator( NULL )
{
}

ioCameraEvent::~ioCameraEvent()
{
}

void ioCameraEvent::LoadProperty( ioINILoader &rkLoader )
{
	m_iEventCode = rkLoader.LoadInt_e( "event_code", 0 );
}

void ioCameraEvent::SetCreator( ioCameraEventManager *pCreator )
{
	m_pCreator = pCreator;
}

D3DXVECTOR3 ioCameraEvent::ProcessCameraEvent( D3DXVECTOR3 vLookAt, D3DXVECTOR3 vPosition,
											   float fFov, D3DXQUATERNION qtTarget )
{
	return vPosition;
}

D3DXVECTOR3 ioCameraEvent::CheckOffSet( D3DXVECTOR3 vLookAt, D3DXQUATERNION qtTarget )
{
	return vLookAt;
}