

#ifndef _ioTargetLookAtCameraController_h_
#define _ioTargetLookAtCameraController_h_

class ioCameraInput;
class ioMovableObject;
class ioCameraController;

class __EX ioTargetLookAtCameraController : public ioCameraController
{
protected:
	ioMovableObject	*m_pTargetObj;

	D3DXVECTOR3	m_vLookAt;
	D3DXVECTOR3 m_vPreTargetPos;

	float	m_fCurDist;
	float	m_fMinDist;
	float	m_fMaxDist;
	float	m_fPreDist;

	float	m_fSpringConst;
	float	m_fDampConst;

	float   m_fRotSpringConst;
	float	m_fRotDampConst;

	D3DXVECTOR3 m_vLookAtOffset;

	float	m_fFwdSpeed, m_fRotSpeed;
	float	m_fTargetHead, m_fTargetPitch;

	float	m_fCurBottomHeight;
	float	m_fDefaultHeight;

	D3DXVECTOR3 m_vRepairDir;
	bool	m_bRepairDefault;

public:
	virtual void Update( ioCameraInput *pInput );

	virtual void CreateViewMatrix( D3DXMATRIX *pViewMat ) const;
	virtual CamCtrlType GetCamCtrlType() const { return CAM_CTRL_TARGET; }
	virtual D3DXVECTOR3 GetDirection() const;
	virtual const char* GetName();

protected:
	void UpdatePos( ioCameraInput *pInput );
	void UpdateAngleByDir();

public:
	virtual void SetPosition( const D3DXVECTOR3 &vPos );
	void SetMinMaxDistance( float fMinDist, float fMaxDist );
	void SetSpringConstants( float fSpringConst, float fDampConst );
	void SetRotSpringConstants( float fSpringConst, float fDampConst );
	void SetSpeed( float fForward, float fRot );
	void SetLookAtTarget( ioMovableObject *pObj,
						  float fDefaultHeight,
						  float fCurBottomHeight,
						  float fCurDist,
						  const D3DXVECTOR3 &vOffset );

	void SetCurDist( float fDist );

	void SetDefaultHeight( float fHeight );
	void SetCurBottomHeight( float fHeight );

	void SetObjMoveDir( const D3DXVECTOR3 &vMoveDir );

public:
	virtual void GetExtraInfoText( char *szBuf, int iLen ) const;

	float GetMinDist() const { return m_fMinDist; }
	float GetMaxDist() const { return m_fMaxDist; }
	float GetCurDist() const { return m_fCurDist; }

	float GetDefaultHeight() const { return m_fDefaultHeight; }
	D3DXVECTOR3 GetDefaultBackPos() const;
	D3DXVECTOR3 GetLookAtByObjPos() const;

protected:
	D3DXVECTOR3 SpringMoveDamp( float fSpringConst,
								float fDampConst,
								const D3DXVECTOR3 &vCurPos,
								const D3DXVECTOR3 &vTargetPos,
								const D3DXVECTOR3 &vPreTargetPos );

	float SpringDistDamp( float fSpringConst,
						  float fDampConst,
						  float fCurDist,
						  float fTargetDist,
						  float fPreDist );

	void ZoomToLookAt( float fAmt );
	float LimitPitch( float fPitch );

public:
	ioTargetLookAtCameraController();
	virtual ~ioTargetLookAtCameraController();
};

inline ioTargetLookAtCameraController* ToTargetCtrl( ioCameraController *pControl )
{
	if( pControl == NULL ||
		pControl->GetCamCtrlType() != ioCameraController::CAM_CTRL_TARGET )
	{
		return NULL;
	}

	return dynamic_cast< ioTargetLookAtCameraController* >( pControl );
}

#endif