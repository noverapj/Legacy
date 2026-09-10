#pragma once

#include "../io3DEngine/io3DCommon.h"
#include "../io3DEngine/ioPrerequisites.h"
#include "../io3DEngine/ioSingleton.h"

class ioPlayStage;

struct ShakeCamera
{
public:
	enum ShakePriority
	{
		SP_DEFAULT,
		SP_HIGH,
	};

public:
	int iShakeID;

	DWORD dwShakeChangeTime;
	float fShakeRate;
	float fShakeAmountX;
	float fShakeAmountY;
	float fEndAmount;

	DWORD dwRollChangeTime;
	DWORD dwRollEndSlerp;
	float fRollRate;
	float fRollAmount;
	float fRollEndAmount;

public:
	ShakeCamera()
	{
		iShakeID = 0;
		dwShakeChangeTime = 0;
		fShakeRate = 0.0f;
		fShakeAmountX = 0.0f;
		fShakeAmountY = 0.0f;
		fEndAmount = 0.0f;

		dwRollChangeTime = 0;
		dwRollEndSlerp = 0;
		fRollRate = 0.0f;
		fRollAmount = 0.0f;
		fRollEndAmount = 0.0f;
	}
};

class ioShakeCameraManager : public Singleton< ioShakeCameraManager >
{
protected:
	typedef std::vector< ShakeCamera > ShakeCameraList;
	ShakeCameraList m_ShakeCameraList;

	ioPlayStage *m_pPlayStage;

public:
	void InitManager();
	void SetShakeCamera( int iShakeType, int iPriority = 0 );

public:
	static ioShakeCameraManager& GetSingleton();

public:
	ioShakeCameraManager( ioPlayStage *pStage );
	virtual ~ioShakeCameraManager();
};

#define g_ShakeCameraMgr ioShakeCameraManager::GetSingleton()
