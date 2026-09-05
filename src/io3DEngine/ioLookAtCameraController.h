

#ifndef _ioLookAtCameraController_h_
#define _ioLookAtCameraController_h_

class ioCameraInput;
class ioCameraController;

class __EX ioLookAtCameraInput : public ioCameraInput
{
public:
	bool	m_bForward, m_bBack, m_bLeft, m_bRight;
	bool	m_bZoom, m_bMiddle;

public:
	virtual CamInputType GetType() const { return CAM_INPUT_LOOKAT; }

public:
	ioLookAtCameraInput();
	virtual ~ioLookAtCameraInput();
};

inline ioLookAtCameraInput* ToLookAtInput( ioCameraInput *pInput )
{
	if( !pInput || pInput->GetType() != ioCameraInput::CAM_INPUT_LOOKAT )
		return NULL;

	return dynamic_cast< ioLookAtCameraInput* >( pInput );
}

class __EX ioLookAtCameraController : public ioCameraController
{
protected:
	D3DXVECTOR3	m_vLookAt;
	float m_fForwardSpeed, m_fStrafeSpeed, m_fRotSpeed;
	float m_fMinDistance;

public:
	virtual void Update( ioCameraInput *pInput );

	virtual void CreateViewMatrix( D3DXMATRIX *pViewMat ) const;
	virtual CamCtrlType GetCamCtrlType() const { return CAM_CTRL_LOOKAT; }
	virtual D3DXVECTOR3 GetDirection() const;
	virtual const char* GetName();

	virtual void GetExtraInfoText( char *szBuf, int iLen ) const;

public:
	virtual void SetPosition( const D3DXVECTOR3 &vPos );

	void SetLookAt( const D3DXVECTOR3 &vLookAt );
	void SetMinDistance( float fDistance );
	void SetSpeed( float fForward, float fStrafe, float fRot );

	void GetSpeed( float& fForward, float& fStrafe, float& fRot );
	inline const D3DXVECTOR3& GetLookAt() const { return m_vLookAt; }

protected:
	void Rotate( float dt, float fXAmt, float fYAmt );
	void ZoomToLookAt( float dt, float fAmt );
	void MoveInLocal( ioLookAtCameraInput *pInput, float dt );

	D3DXVECTOR3 CalcDirectionVector( float fHeadD, float fPitchD );
	float MaxMinPitchLimit( float fPitchD );

public:
	ioLookAtCameraController();
	virtual ~ioLookAtCameraController();
};

inline ioLookAtCameraController* ToLookAtCtrl( ioCameraController *pControl )
{
	if( pControl == NULL ||
		pControl->GetCamCtrlType() != ioCameraController::CAM_CTRL_LOOKAT )
	{
		return NULL;
	}

	return dynamic_cast< ioLookAtCameraController* >( pControl );
}

#endif