

#ifndef _ioEventCameraController_h_
#define _ioEventCameraController_h_

class ioCameraInput;
class ioCameraController;

class __EX ioSlerpCameraController : public ioCameraController
{
public:
	typedef struct tagSlerpTarget
	{
		D3DXVECTOR3 vPosition;
		D3DXVECTOR3 vLookAt;
		int		iTime;
	}SlerpTarget;

	typedef std::list< SlerpTarget > SlerpTargetList;

protected:
	D3DXVECTOR3	m_vLookAt;

	SlerpTargetList m_vSlerpList;

public:
	void SetLookAt( const D3DXVECTOR3 &vLookAt );
	void AddNextTarget( const D3DXVECTOR3 &vPos,
						const D3DXVECTOR3 &vLookAt,
						int iTime );

	bool IsNowSlerping() const;
	void ClearSlerping();

public:
	virtual void Update( ioCameraInput *pInput );
	virtual void CreateViewMatrix( D3DXMATRIX *pViewMat ) const;
	virtual CamCtrlType GetCamCtrlType() const { return CAM_CTRL_SLERP; }
	virtual D3DXVECTOR3 GetDirection() const;
	virtual const char* GetName();

public:
	ioSlerpCameraController();
	virtual ~ioSlerpCameraController();
};

inline ioSlerpCameraController* ToSlerpCtrl( ioCameraController *pControl )
{
	if( pControl == NULL ||	pControl->GetCamCtrlType() != ioCameraController::CAM_CTRL_SLERP )
		return NULL;

	return dynamic_cast< ioSlerpCameraController* >( pControl );
}

#endif