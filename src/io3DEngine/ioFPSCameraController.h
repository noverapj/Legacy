

#ifndef _ioFPSCameraController_h_
#define _ioFPSCameraController_h_

class ioCameraInput;
class ioCameraController;

class __EX ioFPSCameraInput : public ioCameraInput
{
public:
	bool	m_bForward, m_bBack, m_bLeft, m_bRight, m_bMiddle;
	bool	m_bLeftRot, m_bRightRot, m_bUp, m_bDown, m_bSpeedUp;

public:
	virtual CamInputType GetType() const { return CAM_INPUT_FPS; }

public:
	ioFPSCameraInput();
	virtual ~ioFPSCameraInput();
};

inline ioFPSCameraInput* ToFPSInput( ioCameraInput *pInput )
{
	if( !pInput || pInput->GetType() != ioCameraInput::CAM_INPUT_FPS )
		return NULL;

	return dynamic_cast< ioFPSCameraInput* >( pInput );
}

class __EX ioFPSCameraController : public ioCameraController
{
protected:
	float	m_fForwardSpeed, m_fStrafeSpeed, m_fUpSpeed, m_fRotSpeed;
	float	m_fSpeedUpFactor;

public:
	virtual void Update( ioCameraInput *pInput );

	virtual void CreateViewMatrix( D3DXMATRIX *pViewMat ) const;
	virtual CamCtrlType GetCamCtrlType() const { return CAM_CTRL_FPS; }
	virtual D3DXVECTOR3 GetDirection() const;
	virtual const char* GetName();

public:
	void SetSpeed( float fForward, float fStrafe, float fUp, float fRot );

public:
	ioFPSCameraController();
	virtual ~ioFPSCameraController();
};

inline ioFPSCameraController* ToFPSCtrl( ioCameraController *pControl )
{
	if( pControl == NULL ||
		pControl->GetCamCtrlType() != ioCameraController::CAM_CTRL_FPS )
	{
		return NULL;
	}

	return dynamic_cast< ioFPSCameraController* >( pControl );		
}

#endif