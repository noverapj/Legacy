#pragma once



class ioINILoader;
class ioCameraEventManager;

class ioCameraEvent
{
public:
	enum CameraEvnetType
	{
		CET_NONE,
		CET_SIMPLE,
		CET_SCRIPT,
		CET_EXPORT,
	};

protected:
	ioCameraEventManager *m_pCreator;
	DWORD m_iEventCode;

	bool m_bCameraEventStart;
	bool m_bCameraEventEnd;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual CameraEvnetType GetType() const = 0;
	virtual ioCameraEvent* Clone() = 0;
	
	virtual D3DXVECTOR3 ProcessCameraEvent( D3DXVECTOR3 vLookAt,
											D3DXVECTOR3 vPosition,
											float fFov,
											D3DXQUATERNION qtTarget );


	virtual D3DXVECTOR3 CheckOffSet( D3DXVECTOR3 vLookAt,
									 D3DXQUATERNION qtTarget );

	virtual bool  IsForceLookTargetChar() const = 0;
	virtual float GetCameraFov() = 0;
	virtual float GetCameraRoll() = 0;

public:
	void SetCreator( ioCameraEventManager *pCreator );

public:
	inline bool ChckeCameraEventStart() const { return m_bCameraEventStart; }
	inline bool CheckCameraEventEnd() const { return m_bCameraEventEnd; }
	inline DWORD GetCECode() const { return m_iEventCode; }

public:
	ioCameraEvent();
	ioCameraEvent( const ioCameraEvent &rhs );
	virtual ~ioCameraEvent();
};

typedef std::list< ioCameraEvent* > CameraEventList;
typedef std::vector< ioCameraEvent* > CameraEventVector;

