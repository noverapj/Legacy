
#include "stdafx.h"
#include "SingleSetting.h"

ModeType SingleSetting::m_ModeType;
ioHashString SingleSetting::m_MapFileName;

SingleSetting::SingleSetting()
{
	m_ModeType = MT_TEAM_SURVIVAL;
	m_MapFileName.Clear();
}

SingleSetting::~SingleSetting()
{

}

void SingleSetting::LoadINI()
{
	ioINILoader_e kLoader( "config/lsc_singlemode.ini" );
	kLoader.SetTitle_e( "common" );
	m_ModeType = static_cast<ModeType>( kLoader.LoadInt_e( "ModeType", MT_NONE ) );

	char szBuf[MAX_PATH] = "";
	kLoader.LoadString_e( "MapFile", "", szBuf, MAX_PATH );
	m_MapFileName = szBuf;

	bool bCheck = false;
	bCheck = kLoader.LoadBool_e( "ShowBoundBox", true );
	Setting::SetShowBoundBox( bCheck );

	bCheck = kLoader.LoadBool_e( "ExtraInfo", true );
	Setting::SetShowExtraInfo( bCheck );
}