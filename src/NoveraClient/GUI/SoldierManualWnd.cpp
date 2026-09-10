#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"

#include "../ChatMessageFunc.h"
#include "../WndID.h"
#include "../ioPlayStage.h"
#include "../ioMyInfo.h"
#include "../Setting.h"

#include "TutorialWnd.h"
#include "ioSP2GUIManager.h"
#include ".\soldiermanualwnd.h"


template<> ioSoldierManualMgr* Singleton< ioSoldierManualMgr >::ms_Singleton = 0;

ioSoldierManualMgr::ioSoldierManualMgr()
{

}

ioSoldierManualMgr::~ioSoldierManualMgr()
{

}

ioSoldierManualMgr& ioSoldierManualMgr::GetSingleton()
{
	return Singleton< ioSoldierManualMgr >::GetSingleton();
}

ioSoldierManualMgr *ioSoldierManualMgr::GetSingletonPtr()
{
	return Singleton< ioSoldierManualMgr >::GetSingletonPtr();
}