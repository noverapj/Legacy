

#ifndef _ioCameraController_h_
#define _ioCameraController_h_

class __EX ioCameraInput
{
public:
	enum CamInputType
	{
		CAM_INPUT_DEFAULT,
		CAM_INPUT_FPS,
		CAM_INPUT_LOOKAT
	};

public:
	float	m_fXMoveAmt;
	float	m_fYMoveAmt;
	float	m_fWheelAmt;

public:
	virtual CamInputType GetType() const { return CAM_INPUT_DEFAULT; }

public:
	ioCameraInput();
	virtual ~ioCameraInput();
};

class __EX ioCameraController
{
public:
	enum CamCtrlType
	{
		CAM_CTRL_FPS,
		CAM_CTRL_LOOKAT,
		CAM_CTRL_TARGET,
		CAM_CTRL_SLERP
	};

protected:
	D3DXVECTOR3	m_vPos;
	float		m_fHead, m_fPitch, m_fRoll;

public:
	virtual void Update( ioCameraInput *pCamInput ) = 0;
	virtual void CreateViewMatrix( D3DXMATRIX *pViewMat ) const = 0;

	virtual CamCtrlType GetCamCtrlType() const = 0;
	virtual D3DXVECTOR3 GetDirection() const = 0;
	virtual const char* GetName() = 0;

public:
	virtual void SetRollValue( float fRoll );
	virtual void SetPosition( const D3DXVECTOR3 &vPos );
	virtual void SetDirection( float fHead, float fPitch, float fRoll );

	virtual void GetExtraInfoText( char *szBuf, int iLen ) const;

	inline const D3DXVECTOR3& GetPosition() const{ return m_vPos; }
	inline float GetHead() const { return m_fHead; }
	inline float GetPitch() const { return m_fPitch; }
	inline float GetRoll() const { return m_fRoll; }

public:
	ioCameraController();
	virtual ~ioCameraController();
};

#endif