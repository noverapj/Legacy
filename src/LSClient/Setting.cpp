

#include "stdafx.h"

#include "Setting.h"
#include "Version.h"
#include <WinIoCtl.h>

char Setting::m_szCPUName[MAX_PATH];
int  Setting::m_iCPUClockSpeed;
int  Setting::m_iCPUCount;
int  Setting::m_iSystemMemory;

char Setting::m_szGraphicName[512];
int  Setting::m_iVertexShader;
int  Setting::m_iPixelShader;

ioHashString Setting::m_szHDDSerialNumber;

Setting::EnableSampleList Setting::m_EnableSampleList;

Setting::SizeList Setting::m_vResolutionList;

char  Setting::m_szDirectXVersion[MAX_PATH];
DWORD Setting::m_dwDirectXVersion;

char Setting::m_szOSVersion[MAX_PATH];
bool Setting::m_bWin98Series;

char  Setting::m_szFullSystemInfo[1024];

int Setting::m_iReservedMode = 0; // 랜덤

SettingOption Setting::m_Option;
Setting::Spec Setting::m_DefaultSpec;

char Setting::m_szSettingString[512];

D3DVIEWPORT9 Setting::m_ViewPort;
D3DXMATRIX Setting::m_matView;
D3DXMATRIX Setting::m_matProj;
D3DXMATRIX Setting::m_matIdentity;

bool SettingOption::IsOptionChanged( const SettingOption &rkSetting )
{
	if( m_bFullScreen != rkSetting.m_bFullScreen )
		return true;

	if( m_bShadow != rkSetting.m_bShadow )
		return true;

	if( m_bGlow != rkSetting.m_bGlow )
		return true;

	if( m_bShowMapEffect != rkSetting.m_bShowMapEffect )
		return true;

	if( m_bTrailGlow != rkSetting.m_bTrailGlow )
		return true;

	if( m_bSoftSkinning != rkSetting.m_bSoftSkinning )
		return true;

	if( m_bUse16bitFrameBuffer != rkSetting.m_bUse16bitFrameBuffer )
		return true;

	if( m_bUseMapEdge != rkSetting.m_bUseMapEdge )
		return true;

	if( m_bUseObjectEdge != rkSetting.m_bUseObjectEdge )
		return true;

	if( m_bSpecAutoCheck != rkSetting.m_bSpecAutoCheck )
		return true;

	if( m_bSwapCopy != rkSetting.m_bSwapCopy )
		return true;

	if( m_AntiAliasingQuality != rkSetting.m_AntiAliasingQuality )
		return true;

	if( m_TextureQuality != rkSetting.m_TextureQuality )
		return true;

	if( m_LightQuality != rkSetting.m_LightQuality )
		return true;

	if( m_EffectQuality != rkSetting.m_EffectQuality )
		return true;

	if( m_MapObjectQuality != rkSetting.m_MapObjectQuality )
		return true;

	if( m_FilterType != rkSetting.m_FilterType )
		return true;

	if( m_OverayType != rkSetting.m_OverayType )
		return true;

	if( m_iWidth != rkSetting.m_iWidth )
		return true;

	if( m_iHeight != rkSetting.m_iHeight )
		return true;

	if( m_iBGMVolume != rkSetting.m_iBGMVolume )
		return true;
	
	if( m_iEffectSoundVolume != rkSetting.m_iEffectSoundVolume )
		return true;

	if( m_iVoiceVolume != rkSetting.m_iVoiceVolume )
		return true;

	if( m_iCustomSoundVolume != rkSetting.m_iCustomSoundVolume )
		return true;

	if( m_bUseBGM != rkSetting.m_bUseBGM )
		return true;
	
	if( m_bUseEffectSound != rkSetting.m_bUseEffectSound )
		return true;
	
	if( m_bUseOtherVoiceChat!= rkSetting.m_bUseOtherVoiceChat )
		return true;

	if( m_bUseMIC != rkSetting.m_bUseMIC )
		return true;

	if( m_bUseVoiceChat != rkSetting.m_bUseVoiceChat )
		return true;

	if( m_bUseCustomSound != rkSetting.m_bUseCustomSound )
		return true;

	if( m_bSlangFilterChange != rkSetting.m_bSlangFilterChange )
		return true;

	if( m_bSlangFilterOther != rkSetting.m_bSlangFilterOther )
		return true;

	if( m_bAutoRejctParty != rkSetting.m_bAutoRejctParty )
		return true;

	if( m_bAutoRejctCamp != rkSetting.m_bAutoRejctCamp )
		return true;

	if( m_szScreenShotSavePath != rkSetting.m_szScreenShotSavePath )
		return true;

	if( m_bScreenShotFormatJPG != rkSetting.m_bScreenShotFormatJPG )
		return true;

	if( m_bBlockUsersChat != rkSetting.m_bBlockUsersChat )
		return true;

	if( m_bChatBubbleAlarm != rkSetting.m_bChatBubbleAlarm )
		return true;

	if( m_bGuildChatAlarm != rkSetting.m_bGuildChatAlarm )
		return true;

	if( m_bBlockUsersCustomSound != rkSetting.m_bBlockUsersCustomSound )
		return true;

	if( m_iMovieCodec != rkSetting.m_iMovieCodec )
		return true;

	if( m_iMovieSizeType != rkSetting.m_iMovieSizeType )
		return true;

	if( m_iMovieFrameType != rkSetting.m_iMovieFrameType )
		return true;

	if( m_iMovieQualityType != rkSetting.m_iMovieQualityType )
		return true;

	if( m_iMovieSoundType != rkSetting.m_iMovieSoundType )
		return true;

	if( m_bHideSkillBuffEmoticon != rkSetting.m_bHideSkillBuffEmoticon )
		return true;

	if( m_bHideMiniSoldierSelectSlot != rkSetting.m_bHideMiniSoldierSelectSlot )
		return true;

	if( m_bHideCharChangeGauge != rkSetting.m_bHideCharChangeGauge )
		return true;

	if( m_bUseJoyPad != rkSetting.m_bUseJoyPad )
		return true;

	if( m_bNoUIModeTeamCircleShow != rkSetting.m_bNoUIModeTeamCircleShow )
		return true;

	if( m_bNoUIModeIDShow != rkSetting.m_bNoUIModeIDShow )
		return true;

	if( m_bNoUIModeHPShow != rkSetting.m_bNoUIModeHPShow )
		return true;

	if( m_bNoUIModeChatBubbleShow != rkSetting.m_bNoUIModeChatBubbleShow )
		return true;

	if( m_bMovieModeUIHide != rkSetting.m_bMovieModeUIHide )
		return true;

	if( m_bGuildFolderDel != rkSetting.m_bGuildFolderDel )
		return true;

	if( m_iChatBackType != rkSetting.m_iChatBackType )
		return true;

	if( m_bNormalChat != rkSetting.m_bNormalChat )
		return true;

	if( m_bWholeChat != rkSetting.m_bWholeChat )
		return true;

	if( m_bSystemChat != rkSetting.m_bSystemChat )
		return true;

	if( m_dwSlot != rkSetting.m_dwSlot )
		return true;

	if( m_bCostumeShowChange != rkSetting.m_bCostumeShowChange )
		return true;

	if ( m_bPlayKillSound != rkSetting.m_bPlayKillSound )
		return true;

	if ( m_bPlayComboSound != rkSetting.m_bPlayComboSound )
		return true;

	if ( m_bPlayExtendSound != rkSetting.m_bPlayExtendSound )
		return true;

	if ( m_dwLSCCheckTime != rkSetting.m_dwLSCCheckTime )
		return true;

	return false;
}

Setting::Setting()
{
}

Setting::~Setting()
{
}

void Setting::CheckOSVersion()
{
	OSVERSIONINFOEX	osv;
	ZeroMemory(&osv, sizeof(osv));
	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionEx((OSVERSIONINFO*)&osv);

	memset( m_szOSVersion, 0, sizeof(m_szOSVersion) );
	m_bWin98Series = false;

	if(osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)	// 98, Me
	{
		if(osv.dwMinorVersion == 0)
		{
			strcpy( m_szOSVersion, "Microsoft Windows 95 " );
			if ( osv.szCSDVersion[1] == 'C' || osv.szCSDVersion[1] == 'B' )
				strcat(m_szOSVersion, "OSR2 ");
		}
		else if(osv.dwMinorVersion == 10)
		{
			 strcpy(m_szOSVersion, "Microsoft Windows 98 ");
             if ( osv.szCSDVersion[1] == 'A' )
                strcat(m_szOSVersion, "SE ");			
		}
		else if(osv.dwMinorVersion == 90)
		{
			strcpy(m_szOSVersion, "Microsoft Windows Millennium Edition");
		}

		m_bWin98Series = true;
	}
	else if( osv.dwPlatformId == VER_PLATFORM_WIN32_NT ) //	NT, 2000, XP, Server
	{
		if( osv.dwMajorVersion == 5)
		{
			if(osv.dwMinorVersion == 0)
				strcpy( m_szOSVersion, "Microsoft Windows 2000" );
			else if(osv.dwMinorVersion == 1)
				strcpy( m_szOSVersion, "Microsoft Windows XP");
			else if(osv.dwMinorVersion == 2)
				strcpy( m_szOSVersion, "Microsoft Windows Server 2003 Family");
		}
		else if( osv.dwMajorVersion == 6 )
		{
			if( osv.dwMinorVersion == 0 )
			{
				if( osv.wProductType == VER_NT_WORKSTATION )
					strcpy( m_szOSVersion, "Microsoft Windows Vista" );
				else
					strcpy( m_szOSVersion, "Microsoft Windows Server 2008" );
			}
			else if( osv.dwMinorVersion == 1 )
			{
				if( osv.wProductType == VER_NT_WORKSTATION )
					strcpy( m_szOSVersion, "Microsoft Windows 7" );
				else
					strcpy( m_szOSVersion, "Microsoft Windows Server 2008 R2" );
			}
			else if( osv.dwMinorVersion == 2 )
			{
				if( osv.wProductType == VER_NT_WORKSTATION )
					strcpy( m_szOSVersion, "Microsoft Windows 8" );
				else
					strcpy( m_szOSVersion, "Microsoft Windows Server 2012" );
			}
		}
		else		
		{
			strcpy( m_szOSVersion, "Microsoft Windows NT" );
		}

		// 32 or 64
		SYSTEM_INFO si;
		ZeroMemory(&si, sizeof(SYSTEM_INFO));

		typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
		PGNSI pGNSI = NULL;
		pGNSI = (PGNSI) GetProcAddress(	GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
		if(NULL != pGNSI)
		{
			pGNSI(&si);

			if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
				StringCchCat( m_szOSVersion, sizeof( m_szOSVersion ), " 64-bit" );
			else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL )
				StringCchCat( m_szOSVersion, sizeof( m_szOSVersion ), " 32-bit" );
		}
		//

		m_bWin98Series = false;
	}
	else		// Maybe Win 3.1
	{
		strcpy( m_szOSVersion, "Microsoft Win32s" );
		m_bWin98Series = true;
	}
}

void Setting::CheckSystemInfo()
{
	// CPU info
	Help::RegReadString( REG_SHLM, REG_KEY_CPU, "ProcessorNameString", "Unknown", m_szCPUName, MAX_PATH );
	Help::HeadBlankClear( m_szCPUName );

	m_iCPUClockSpeed = ioCPU::GetCPUClockSpeed();
	m_iCPUCount = ioCPU::GetCPUCount();

	// RAM info
	MEMORYSTATUS MemStatus;
	MemStatus.dwLength=sizeof(MemStatus);
	GlobalMemoryStatus(&MemStatus);
	m_iSystemMemory = MemStatus.dwTotalPhys / 1048576;

	// Graphic Card Info
	IDirect3D9 *pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if( pD3D )
	{
		D3DCAPS9 kCaps;
		if( SUCCEEDED( pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &kCaps ) ) )
		{
			int iMajor, iMinor;
			iMajor = ( kCaps.VertexShaderVersion & 0xff00 ) >> 8;
			iMinor = kCaps.VertexShaderVersion & 0xff;
			m_iVertexShader = iMajor * 10 + iMinor;

			iMajor = ( kCaps.PixelShaderVersion & 0xff00 ) >> 8;
			iMinor = kCaps.PixelShaderVersion & 0xff;
			m_iPixelShader = iMajor * 10 + iMinor;
		}

		D3DADAPTER_IDENTIFIER9 kAdapter;
		if( SUCCEEDED( pD3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &kAdapter ) ) )
		{
			DWORD dwProduct = HIWORD(kAdapter.DriverVersion.HighPart);
			DWORD dwVersion = LOWORD(kAdapter.DriverVersion.HighPart);
			DWORD dwSubVersion = HIWORD(kAdapter.DriverVersion.LowPart);
			DWORD dwBuild = LOWORD(kAdapter.DriverVersion.LowPart);

			wsprintf( m_szGraphicName, "%s(%d.%d.%d.%d)",
									   kAdapter.Description,
									   dwProduct,
									   dwVersion,
									   dwSubVersion,
									   dwBuild );
		}
		else
		{
			strcpy( m_szGraphicName, "GetName Failed" );
		}

		SAFERELEASE( pD3D );
	}

	// HDD Info
	//m_dwHDDSerialNumber = 0;
	/*TCHAR szWindowDir[1024];
	GetWindowsDirectory(szWindowDir, 1024);
	TCHAR szDrive[4];

	szDrive[0] = szWindowDir[0];
	szDrive[1] = szWindowDir[1];
	szDrive[2] = szWindowDir[2];
	szDrive[3] = 0;

	DWORD Flag1, Flag2;
	TCHAR szType[256];
	TCHAR Buffer[256];
	GetVolumeInformation(szDrive, Buffer, 256, &m_dwHDDSerialNumber, &Flag1, &Flag2, szType, 256);*/

	HANDLE h = CreateFile( "\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0 );
	if( h != INVALID_HANDLE_VALUE )
	{
		DWORD dwResult = NO_ERROR;
		STORAGE_PROPERTY_QUERY storagePropertyQuery;
		ZeroMemory(&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
		storagePropertyQuery.PropertyId = StorageDeviceProperty;
		storagePropertyQuery.QueryType = PropertyStandardQuery;

		// get the necessary output buffer size
		STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = { 0 };
		DWORD dwBytesReturned = 0;
		if(!::DeviceIoControl(h, IOCTL_STORAGE_QUERY_PROPERTY,
			&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
			&storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER),
			&dwBytesReturned, NULL))
		{
			dwResult = ::GetLastError();
			::CloseHandle(h);
		}

		const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
		BYTE* pOutBuffer = new BYTE[dwOutBufferSize];
		ZeroMemory(pOutBuffer, dwOutBufferSize);

		if (!::DeviceIoControl(h, IOCTL_STORAGE_QUERY_PROPERTY,
			&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
			pOutBuffer, dwOutBufferSize,
			&dwBytesReturned, NULL))
		{
			dwResult = ::GetLastError();
			delete[]pOutBuffer;
			::CloseHandle(h);
		}

		STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pOutBuffer;
		DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
		char strSerialNumber[MAX_PATH];
		if (dwSerialNumberOffset != 0)
		{
			memcpy_s( strSerialNumber, MAX_PATH, pOutBuffer + dwSerialNumberOffset, MAX_PATH );
			//strSerialNumber = CString(pOutBuffer + dwSerialNumberOffset);
		}
		
		delete[]pOutBuffer;
		
		int iStart, iEnd;
		iStart = 0;
		iEnd = strlen(strSerialNumber) - 1;
		for( int i = 0; i < strlen(strSerialNumber); i++ )
		{
			if( strSerialNumber[i] == 32 )
				iStart = i + 1;
			else
				break;
		}

		for( int i = strlen(strSerialNumber) - 1; i <= 0; i++ )
		{
			if( strSerialNumber[i] == 32 )
				iEnd = i;
			else
				break;
		}

		if( iEnd > iStart )
			strSerialNumber[iEnd + 1] = NULL;

		char szHDDSerialNumber[60];
		strcpy_s( szHDDSerialNumber, 60, strSerialNumber + iStart );

		m_szHDDSerialNumber = szHDDSerialNumber;

		if( h != INVALID_HANDLE_VALUE )
			CloseHandle( h );
	}
}

void Setting::Initialize( const ioHashString &rkPublicID )
{
	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\setting.ini",rkPublicID.c_str() );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "Setting" );

	CheckSystemInfo();

	LoadSettingAlways( kLoader );

	if( kLoader.LoadInt_e( "SpecAutoCheck", 1 ) == 1 )
		m_Option.m_bSpecAutoCheck = true;
	else
		m_Option.m_bSpecAutoCheck = false;

	if( m_Option.m_bSpecAutoCheck )
	{
		m_DefaultSpec = CheckHardwareSpec();
		SetOptionBySpec( m_Option, m_DefaultSpec );
	}
	else
	{
		m_DefaultSpec = (Spec)kLoader.LoadInt_e( "Spec", LOW_SPEC );
		if( m_DefaultSpec == CUSTOM_SPEC )
		{
			LoadCustomSetting( kLoader );
		}
		else
		{
			SetOptionBySpec( m_Option, m_DefaultSpec );
		}
	}

	if( GetPixelShaderVersion() < 10 )
	{
		SetLightQuality( LQ_LOW );
	}

	D3DXMatrixIdentity( &m_matIdentity );

	LOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
	LOG.PrintTimeAndLog( 0, "OS : %s", m_szOSVersion );
	LOG.PrintTimeAndLog( 0, "DirectX : %s(%x)", m_szDirectXVersion, m_dwDirectXVersion );
	LOG.PrintTimeAndLog( 0, "CPU : %s, Clock : %dMHz(%d)", m_szCPUName, m_iCPUClockSpeed, m_iCPUCount );
	LOG.PrintTimeAndLog( 0, "RAM : %dMBytes", m_iSystemMemory );
	LOG.PrintTimeAndLog( 0, "GPU : %s(VS:%d, PS:%d)", m_szGraphicName, m_iVertexShader, m_iPixelShader );
	LOG.PrintTimeAndLog( 0, "HDD : %s", m_szHDDSerialNumber.c_str() );

	sprintf( m_szFullSystemInfo, "Version : %s | %s\r\n"
		                         "OS : %s\r\n"
								 "DirectX : %s(%x)\r\n"
								 "CPU : %s, Clock : %dMHz(%d)\r\n"
								 "RAM : %dMBytes\r\n"
								 "GPU : %s(VS:%d, PS:%d)\r\n"
								 "HDD : %s",
								 STRFILEVER, STRINTERNALNAME,
								 m_szOSVersion,
								 m_szDirectXVersion, m_dwDirectXVersion,
								 m_szCPUName, m_iCPUClockSpeed, m_iCPUCount,
								 m_iSystemMemory,
								 m_szGraphicName, m_iVertexShader, m_iPixelShader,
								 m_szHDDSerialNumber.c_str() );

#ifdef _DEBUG
	SetFullScreen( false );
#endif
}

void Setting::SetDirectXVersion( const char *szVersion, DWORD dwVersion )
{
	strcpy( m_szDirectXVersion, szVersion );
	m_dwDirectXVersion = dwVersion;
}

void Setting::SetMultiSampleList( const ioRenderSystem *pSystem )
{
	m_EnableSampleList.clear();

	int iMaxCount = pSystem->GetEnableMultiSampleTypeCount();
	if( iMaxCount > 0 )
	{
		m_EnableSampleList.reserve( iMaxCount );

		for( int i=0 ; i<iMaxCount ; i++ )
		{
			m_EnableSampleList.push_back( pSystem->GetEnableMultiSampleType( i ) );
		}
	}

	if( m_Option.m_bSpecAutoCheck )
	{
		if( m_DefaultSpec == VERY_HIGH_SPEC && m_EnableSampleList.size() < 3 )
		{
			m_DefaultSpec = HIGH_SPEC;
		}
	}

	if( m_DefaultSpec == VERY_HIGH_SPEC )
	{
		m_Option.m_AntiAliasingQuality = ModifyAvailableQuality( AQ_LOW );
	}
	else
	{
		m_Option.m_AntiAliasingQuality = ModifyAvailableQuality( m_Option.m_AntiAliasingQuality );
	}
}

AntiAliasingQuality Setting::ModifyAvailableQuality( AntiAliasingQuality eQuality )
{
	int iMaxCount = m_EnableSampleList.size();
	AntiAliasingQuality eAvailableQuality = eQuality;

	switch( eQuality )
	{
	case AQ_HIGH:
		if( iMaxCount < 3 )
		{
			eAvailableQuality = AQ_MIDDLE;
		}
		// break 빼먹은것 아님
	case AQ_MIDDLE:
		if( iMaxCount < 2 )
		{
			eAvailableQuality = AQ_LOW;
		}
		// break 빼먹은것 아님
	case AQ_LOW:
		if( iMaxCount < 1 )
		{
			eAvailableQuality = AQ_NONE;
		}
		break;
	}

	return eAvailableQuality;
}

D3DMULTISAMPLE_TYPE Setting::GetMultiSampleType( AntiAliasingQuality eQuality )
{
	if( m_EnableSampleList.empty() )
		return D3DMULTISAMPLE_NONE;

	D3DMULTISAMPLE_TYPE eSampleType = D3DMULTISAMPLE_NONE;
	int iMaxEnableCount = m_EnableSampleList.size();

	switch( eQuality )
	{
	case AQ_HIGH:
		if( iMaxEnableCount >= 3 )
		{
			eSampleType = m_EnableSampleList[2];
			break;
		}
		// break; 빼먹은것 아님
	case AQ_MIDDLE:
		if( iMaxEnableCount >= 2 )
		{
			eSampleType = m_EnableSampleList[1];
			break;
		}
		// break; 빼먹은것 아님
	case AQ_LOW:
		// empty가 아니므로 최소 하나는 존재
		eSampleType = m_EnableSampleList[0];
		break;
	case AQ_NONE:
		eSampleType = D3DMULTISAMPLE_NONE;
		break;
	}

	return eSampleType;
}

void Setting::SetResolutionList( const ioRenderSystem *pSystem )
{
	m_vResolutionList.clear();

	UINT uPreWidth = 0;
	UINT uPreHeight = 0;

	int iCount = pSystem->GetNumEnableDisplayMode();
	for( int i=0 ; i<iCount ; i++ )
	{
		UINT uCurWidth, uCurHeight;

		if( !pSystem->GetDisplayResolution( i, uCurWidth, uCurHeight ) )
			continue;

		if( uPreWidth != uCurWidth || uPreHeight != uCurHeight )
		{
			uPreWidth  = uCurWidth;
			uPreHeight = uCurHeight;

			SIZE kSize;
			kSize.cx = (int)uPreWidth;
			kSize.cy = (int)uPreHeight;

			m_vResolutionList.push_back( kSize );
		}
	}
}

int Setting::GetResolutionCount()
{
	return m_vResolutionList.size();
}

bool Setting::GetResolution( int iIndex, int &iWidth, int &iHeight )
{
	if( COMPARE( iIndex, 0, GetResolutionCount() ) )
	{
		iWidth  = m_vResolutionList[iIndex].cx;
		iHeight = m_vResolutionList[iIndex].cy;
		return true;
	}

	return false;
}

Setting::Spec Setting::CheckHardwareSpec()
{
	Spec eCurSpec = LOW_SPEC;

	int iCPUSpeed = m_iCPUClockSpeed;
	if( m_iCPUCount > 1 )
	{
		iCPUSpeed += iCPUSpeed * ( FLOAT05 * ( m_iCPUCount - 1 ) );
	}

	// CPU,
	if( iCPUSpeed > 3000 && m_iCPUCount >= 2 )
		eCurSpec = VERY_HIGH_SPEC;
	else if( iCPUSpeed > 2500 )
		eCurSpec = HIGH_SPEC;
	else if( COMPARE( iCPUSpeed, 1700, 2500 ) )
		eCurSpec = MIDDLE_SPEC;
	else
		eCurSpec = LOW_SPEC;

	// RAM
	if( m_iSystemMemory < 600 )	// 512M 이하
		eCurSpec = LOW_SPEC;
	else if( m_iSystemMemory < 2000 )
	{
		if( eCurSpec == VERY_HIGH_SPEC )
		{
			eCurSpec = HIGH_SPEC;
		}
	}

	// VIDEO
	if( m_iVertexShader < 10 )
		eCurSpec = MINIMUM_SPEC;
	else if( m_iVertexShader < 20 )
	{
		eCurSpec = LOW_SPEC;
	}

	if( m_iPixelShader > 25 )	// ps 3.0
	{
	}
	else if( m_iPixelShader > 15 )	// ps 2.0
	{
		if( eCurSpec == VERY_HIGH_SPEC || eCurSpec == HIGH_SPEC )
			eCurSpec = MIDDLE_SPEC;
	}
	else if( m_iPixelShader < 10 )	// no pixel
	{
		eCurSpec = MINIMUM_SPEC;
	}

	if( strstr( m_szGraphicName, "NVIDIA" ) )
	{
		if( strstr( m_szGraphicName, "00 LE" ) || strstr( m_szGraphicName, "00 GS" ) )
		{
			if( eCurSpec == VERY_HIGH_SPEC || eCurSpec == HIGH_SPEC )
			{
				eCurSpec = MIDDLE_SPEC;
			}
		}
	}
	//UJ 130912, 최적화: 인텔 내장 그래픽은 절대 최고 사양으로 잡지않아야함
	else if( strstr( m_szGraphicName, "Intel" ) )
	{
		if( eCurSpec == VERY_HIGH_SPEC || eCurSpec == HIGH_SPEC )
		{
			eCurSpec = MIDDLE_SPEC;
		}
	}

	return eCurSpec;
}

void Setting::LoadCustomSetting( ioINILoader &rkLoader )
{
	int iOption;
	SettingOption kDefaultOpt;
	SetOptionBySpec( kDefaultOpt, CheckHardwareSpec() );

	iOption = rkLoader.LoadInt_e( "FullScreen", -1 );
	if( iOption == -1 )
	{
		SetFullScreen( kDefaultOpt.m_bFullScreen );
	}
	else
	{
		if( iOption == 1 )
			SetFullScreen( true );
		else
			SetFullScreen( false );
	}

	iOption = rkLoader.LoadInt_e( "Shadow", -1 );
	if( iOption == -1 )
	{
		SetUseShadow( kDefaultOpt.m_bShadow );
	}
	else
	{
		if( iOption == 1 )
			SetUseShadow( true );
		else
			SetUseShadow( false );
	}

	iOption = rkLoader.LoadInt_e( "Glow", -1 );
	if( iOption == -1 )
	{
		SetUseGlow( kDefaultOpt.m_bGlow );
	}
	else
	{
		if( iOption == 1 )
			SetUseGlow( true );
		else
			SetUseGlow( false );
	}

	iOption = rkLoader.LoadInt_e( "MapEffect", -1 );
	if( iOption == -1 )
	{
		SetShowMapEffect( kDefaultOpt.m_bShowMapEffect );
	}
	else
	{
		if( iOption == 1 )
			SetShowMapEffect( true );
		else
			SetShowMapEffect( false );
	}

	iOption = rkLoader.LoadInt_e( "LightQuality", -1 );
	if( iOption == -1 )
	{
		SetLightQuality( kDefaultOpt.m_LightQuality );
	}
	else
	{
		iOption = max( LQ_LOW, min( iOption, LQ_HIGH ) );
		SetLightQuality( (LightQuality)iOption );
	}

	iOption = rkLoader.LoadInt_e( "AntiAliasing", -1 );
	if( iOption == -1 )
	{
		SetAntiAliasingQuality( kDefaultOpt.m_AntiAliasingQuality );
	}
	else
	{
		iOption = max( AQ_NONE, min( iOption, AQ_HIGH ) );
		SetAntiAliasingQuality( (AntiAliasingQuality)iOption );
	}

	iOption = rkLoader.LoadInt_e( "TextureQuality", -1 );
	if( iOption == -1 )
	{
		SetTextureQuality( kDefaultOpt.m_TextureQuality );
	}
	else
	{
		iOption = max( TQ_LOW, min( iOption, TQ_HIGH ) );
		SetTextureQuality( (TextureQuality)iOption );
	}

	iOption = rkLoader.LoadInt_e( "EffectQuality", -1 );
	if( iOption == -1 )
	{
		SetEffectQuality( kDefaultOpt.m_EffectQuality );
	}
	else
	{
		iOption = max( EQ_LOW, min( iOption, EQ_HIGH ) );
		SetEffectQuality( (EffectQuality)iOption );
	}
	
	iOption = rkLoader.LoadInt_e( "MapObjectQuality", -1 );
	if( iOption == -1 )
	{
		SetMapObjectQuality( kDefaultOpt.m_MapObjectQuality );
	}
	else
	{
		iOption = max( MQ_LOW, min( iOption, MQ_HIGH ) );
		SetMapObjectQuality( (MapObjectQuality)iOption );
	}

	iOption = rkLoader.LoadInt_e( "TrailGlow", -1 );
	if( iOption == -1 )
	{
		SetTrailGlow( kDefaultOpt.m_bTrailGlow );
	}
	else
	{
		if( iOption == 1 )
			SetTrailGlow( true );
		else
			SetTrailGlow( false );
	}

	iOption = rkLoader.LoadInt_e( "SoftSkinning", -1 );
	if( iOption == -1 )
	{
		SetSoftSknning( kDefaultOpt.m_bSoftSkinning );
	}
	else
	{
		if( iOption == 1 )
			SetSoftSknning( true );
		else
			SetSoftSknning( false );
	}

	iOption = rkLoader.LoadInt_e( "Use16bitFrameBuffer", -1 );
	if( iOption == -1 )
	{
		SetUse16BitFrameBuffer( kDefaultOpt.m_bUse16bitFrameBuffer );
	}
	else
	{
		if( iOption == 1 )
			SetUse16BitFrameBuffer( true );
		else
			SetUse16BitFrameBuffer( false );
	}

	iOption = rkLoader.LoadInt_e( "UseMapEdge", -1 );
	if( iOption == -1 )
	{
		SetUseMapEdge( kDefaultOpt.m_bUseMapEdge );
	}
	else
	{
		if( iOption == 1 )
			SetUseMapEdge( true );
		else
			SetUseMapEdge( false );
	}

	iOption = rkLoader.LoadInt_e( "UseObjectEdge", -1 );
	if( iOption == -1 )
	{
		SetUseObjectEdge( kDefaultOpt.m_bUseObjectEdge );
	}
	else
	{
		if( iOption == 1 )
			SetUseObjectEdge( true );
		else
			SetUseObjectEdge( false );
	}
}

void Setting::LoadSettingAlways( ioINILoader &rkLoader )
{
	// 0~MAX_VOLUME-1 : 단계총 MAX_VOLUME
	SetBGMVolume( rkLoader.LoadInt_e( "BGMVolume", Help::MAX_VOLUME-1 ) );
	SetEffectSoundVolume( rkLoader.LoadInt_e( "EffectSoundVolume", Help::MAX_VOLUME-1 ) );
	SetVoiceVolume( rkLoader.LoadInt_e( "VoiceVolume", Help::MAX_VOLUME-1 ) );
	SetCustomSoundVolume( rkLoader.LoadInt_e( "CustomSoundVolume", Help::MAX_VOLUME-1 ) );

	SetUseBGM( rkLoader.LoadBool_e( "UseBGM", true ) );	
	SetUseEffectSound( rkLoader.LoadBool_e( "UseEffectSound", true  ) );
	SetUseOtherVoiceChat( rkLoader.LoadBool_e( "UseOtherVoiceChat", true ) );
	SetUseMIC( rkLoader.LoadBool_e( "UseMIC", false ) );
	SetUseVoiceChat( rkLoader.LoadBool_e( "UseVoiceChat", false ) );
	SetUseCustomSound( rkLoader.LoadBool_e( "UseCustomSound", true) );
	setAutoRejectParty( rkLoader.LoadBool_e( "AutoRejectParty", false ) );
	setAutoRejectCamp( rkLoader.LoadBool_e( "AutoRejectCamp", false ) );
	SetSwapCopy( rkLoader.LoadBool_e( "SwapCopy", false ) );
	SetWholeChatOff( false );	// 항상 시작은 false로 로드되게 한다.

	SetFilterType( (FilterType)rkLoader.LoadInt_e( "FilterType", FILTER_NONE ) );
	SetOverayType( (OverayType)rkLoader.LoadInt_e( "OverayType", OVERAY_NONE ) );

	char szSavePath[MAX_PATH*2]="";
	rkLoader.LoadString_e( "ScreenShotSavePath", "", szSavePath, sizeof(szSavePath) );
	SetScreenShotSavePath(szSavePath );

	SetScreenShotFormatJPG( rkLoader.LoadBool_e( "ScreenShotFormatJPG", true) );
	SetBlockUsersChat( rkLoader.LoadBool_e( "BlockUsersChat", false) );
	SetChatBubbleAlarm( rkLoader.LoadBool_e( "ChatBubbleAlarm", true ) );
	SetGuildChatAlarm( rkLoader.LoadBool_e( "GuildChatAlarm", true ) );
	SetBlockUsersCustomSound( rkLoader.LoadBool_e( "BlockUsersCustomSound", false ) );

	SetSlangFilterChange( rkLoader.LoadBool_e( "SlangFilterFunc", false ) );
	SetSlangFilterOther( false );
	//SetSlangFilterOther( rkLoader.LoadBool_e( "SlangFilterTarget", false ) );

	// 기능을 반대로 돌리고 유저들이 저장한 값은 유지하기 위해
	// INI에서 기존의 ShowSkillBuffEmoticon 값은 유지한다.
	SetHideSkillBuffEmoticon( rkLoader.LoadBool_e( "ShowSkillBuffEmoticon", true ) );      
	SetHideMiniSoldierSelectSlot( rkLoader.LoadBool_e( "ShowMiniSoldierSelectSlot", false ) );
	SetHideCharChangeGauge( rkLoader.LoadBool_e( "ShowCharChangeGauge", false ) );

	SetUseJoyPad( rkLoader.LoadBool_e( "UseJoyPad", false ) );

	SetNoUIModeTeamCircleShow( rkLoader.LoadBool_e( "NoUIModeTeamCircleShow", false ) );
	SetNoUIModeIDShow( rkLoader.LoadBool_e( "NoUIModeIDShow", false ) );
	SetNoUIModeHPShow( rkLoader.LoadBool_e( "NoUIModeHPShow", false ) );
	SetNoUIModeChatBubbleShow( rkLoader.LoadBool_e( "NoUIModeChatBubbleShow", false ) );
	SetMovieModeUIHide( rkLoader.LoadBool_e( "MovieModeUIHide", true ) );

	SetMovieCodec(  rkLoader.LoadInt_e( "MovieCodec", ioMovieMaker::CODEC_DEF ) );
	SetMovieSizeType( rkLoader.LoadInt_e( "MovieSizeType", ioMovieMaker::SIZE_DEF ) );
	SetMovieFrameType( rkLoader.LoadInt_e( "MovieFrameType", ioMovieMaker::FRAME_DEF ) );
	SetMovieQualityType( rkLoader.LoadInt_e( "MovieQualityType", ioMovieMaker::QUALITY_DEF ) );
	SetMovieSoundType( rkLoader.LoadInt_e( "MovieSoundType", ioMovieMaker::SOUND_DEF ) );

	SetGuildFolderDel( rkLoader.LoadBool_e( "GuildFolderDelete", true ) );
	SetChatInfoOff( false ); // ini 저장되지 않으며 항상 false 로딩됨

	SetChatBackType( rkLoader.LoadInt_e( "ChatBackType", 0 ) );

	SetChatNormalChat( rkLoader.LoadBool_e( "ChatNormalChat", true ) );
	SetChatWholeChat( rkLoader.LoadBool_e( "ChatWholeChat", true ) );
	SetChatSystemChat( rkLoader.LoadBool_e( "ChatSystemChat", true ) );

	SetCostumeShowChange( rkLoader.LoadBool_e( "CostumeShowFunc", true ) );
	SetPlayKillSound( rkLoader.LoadBool_e( "PlayKillSound", true ) );
	SetPlayComboSound( rkLoader.LoadBool_e( "PlayComboSound", true ) );
	SetPlayExtendSound( rkLoader.LoadBool_e( "PlayExtendSound", false ) );

	SetLSCCheckTime( rkLoader.LoadInt_e( "LSCFileCheckTime", 0 ) );

	int iWidth  = rkLoader.LoadInt_e( "ScreenWidth", -1 );
	int iHeight = rkLoader.LoadInt_e( "ScreenHeight", -1 );
	if( iWidth > 0 && iHeight > 0 )
	{
		SetCurResolution( iWidth, iHeight );
	}
	else
	{
		SetCurResolution( 800, 600 );
	}	

	SetSlotItem( rkLoader.LoadInt_e("ItemSlot", 0x01024100));  // default set   hp potion / sp potion / revive
}

void Setting::SaveSetting( const ioHashString &rkPublicID, bool bExceptResolution )
{
	char fName[MAX_PATH]="";
	StringCbPrintf( fName, sizeof(fName), "Save\\%s\\setting.ini",rkPublicID.c_str() );

	FILE *fp = fopen( fName, "r" );
	if( !fp )
	{
		sprintf( fName, "Save\\%s" , rkPublicID.c_str() );
		CreateDirectory("Save",NULL);
		CreateDirectory(fName,NULL);
		sprintf(fName,"Save\\%s\\setting.ini", rkPublicID.c_str() );

		fp = fopen( fName, "w" );
		if( !fp )
		{
			LOG.PrintTimeAndLog( 0, "Setting::SaveSetting File Create Failed." );
			return;
		}
	}

	fclose( fp );

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "Setting" );

	// 일반 유저 옵션
	kLoader.SaveInt( "Spec", m_DefaultSpec );
	kLoader.SaveBool( "Shadow", m_Option.m_bShadow );
	kLoader.SaveBool( "Glow", m_Option.m_bGlow );
	kLoader.SaveBool( "MapEffect", m_Option.m_bShowMapEffect );
	kLoader.SaveBool( "TrailGlow", m_Option.m_bTrailGlow );
	kLoader.SaveBool( "SoftSkinning", m_Option.m_bSoftSkinning );
	kLoader.SaveBool( "Use16bitFrameBuffer", m_Option.m_bUse16bitFrameBuffer );
	kLoader.SaveBool( "UseMapEdge", m_Option.m_bUseMapEdge );
	kLoader.SaveBool( "UseObjectEdge", m_Option.m_bUseObjectEdge );
	kLoader.SaveBool( "SpecAutoCheck", m_Option.m_bSpecAutoCheck );
	kLoader.SaveBool( "SwapCopy", m_Option.m_bSwapCopy );

	if( !bExceptResolution )
	{
		kLoader.SaveBool( "FullScreen", m_Option.m_bFullScreen );
		kLoader.SaveInt( "ScreenWidth", m_Option.m_iWidth );
		kLoader.SaveInt( "ScreenHeight", m_Option.m_iHeight );
	}

	kLoader.SaveInt( "AntiAliasing", (int)m_Option.m_AntiAliasingQuality );
	kLoader.SaveInt( "LightQuality", (int)m_Option.m_LightQuality );
	kLoader.SaveInt( "TextureQuality", (int)m_Option.m_TextureQuality );
	kLoader.SaveInt( "EffectQuality", (int)m_Option.m_EffectQuality );
	kLoader.SaveInt( "MapObjectQuality", (int)m_Option.m_MapObjectQuality );

	kLoader.SaveInt( "FilterType", (int)m_Option.m_FilterType );
	kLoader.SaveInt( "OverayType", (int)m_Option.m_OverayType );

	kLoader.SaveInt( "BGMVolume", m_Option.m_iBGMVolume );
	kLoader.SaveInt( "EffectSoundVolume", m_Option.m_iEffectSoundVolume );
	kLoader.SaveInt( "VoiceVolume", m_Option.m_iVoiceVolume );
	kLoader.SaveInt( "CustomSoundVolume", m_Option.m_iCustomSoundVolume );

	kLoader.SaveBool( "UseBGM", m_Option.m_bUseBGM );
	kLoader.SaveBool( "UseEffectSound", m_Option.m_bUseEffectSound );
	kLoader.SaveBool( "UseOtherVoiceChat", m_Option.m_bUseOtherVoiceChat );
	kLoader.SaveBool( "UseMIC", m_Option.m_bUseMIC );
	kLoader.SaveBool( "UseVoiceChat", m_Option.m_bUseVoiceChat );
	kLoader.SaveBool( "UseCustomSound", m_Option.m_bUseCustomSound );

	kLoader.SaveBool( "AutoRejectParty", m_Option.m_bAutoRejctParty );
	kLoader.SaveBool( "AutoRejectCamp", m_Option.m_bAutoRejctCamp );

	kLoader.SaveString( "ScreenShotSavePath", m_Option.m_szScreenShotSavePath.c_str() );
	kLoader.SaveBool( "ScreenShotFormatJPG", m_Option.m_bScreenShotFormatJPG );
	kLoader.SaveBool( "BlockUsersChat", m_Option.m_bBlockUsersChat );
	kLoader.SaveBool( "ChatBubbleAlarm", m_Option.m_bChatBubbleAlarm );
	kLoader.SaveBool( "GuildChatAlarm", m_Option.m_bGuildChatAlarm );
	kLoader.SaveBool( "BlockUsersCustomSound", m_Option.m_bBlockUsersCustomSound );

	kLoader.SaveBool( "SlangFilterFunc", m_Option.m_bSlangFilterChange );
	//kLoader.SaveBool( "SlangFilterTarget", m_Option.m_bSlangFilterOther );

	// 기능을 반대로 돌리고 유저들이 저장한 값은 유지하기 위해
	// INI에서 기존의 ShowSkillBuffEmoticon 값은 유지한다.
	kLoader.SaveBool( "ShowSkillBuffEmoticon", m_Option.m_bHideSkillBuffEmoticon );
	kLoader.SaveBool( "ShowMiniSoldierSelectSlot", m_Option.m_bHideMiniSoldierSelectSlot );
	kLoader.SaveBool( "ShowCharChangeGauge", m_Option.m_bHideCharChangeGauge );

	kLoader.SaveBool( "UseJoyPad", m_Option.m_bUseJoyPad );

	kLoader.SaveBool( "NoUIModeTeamCircleShow", NoUIModeTeamCircleShow() );
	kLoader.SaveBool( "NoUIModeIDShow", NoUIModeIDShow() );
	kLoader.SaveBool( "NoUIModeHPShow", NoUIModeHPShow() );
	kLoader.SaveBool( "NoUIModeChatBubbleShow", NoUIModeChatBubbleShow() );
	kLoader.SaveBool( "MovieModeUIHide", MovieModeUIHide() );

	kLoader.SaveInt( "MovieCodec", m_Option.m_iMovieCodec );
	kLoader.SaveInt( "MovieSizeType", m_Option.m_iMovieSizeType );
	kLoader.SaveInt( "MovieFrameType", m_Option.m_iMovieFrameType );
	kLoader.SaveInt( "MovieQualityType", m_Option.m_iMovieQualityType );
	kLoader.SaveInt( "MovieSoundType", m_Option.m_iMovieSoundType );

	kLoader.SaveBool( "GuildFolderDelete", m_Option.m_bGuildFolderDel );

	kLoader.SaveInt( "ChatBackType", m_Option.m_iChatBackType );
	kLoader.SaveBool( "ChatNormalChat", m_Option.m_bNormalChat );
	kLoader.SaveBool( "ChatWholeChat", m_Option.m_bWholeChat );
	kLoader.SaveBool( "ChatSystemChat", m_Option.m_bSystemChat );

	kLoader.SaveInt( "ItemSlot", m_Option.m_dwSlot );

	kLoader.SaveBool( "CostumeShowFunc", m_Option.m_bCostumeShowChange );
	kLoader.SaveBool( "PlayKillSound", m_Option.m_bPlayKillSound );
	kLoader.SaveBool( "PlayComboSound", m_Option.m_bPlayComboSound );
	kLoader.SaveBool( "PlayExtendSound", m_Option.m_bPlayExtendSound );

	kLoader.SaveInt( "LSCFileCheckTime", m_Option.m_dwLSCCheckTime );
}

char* Setting::GetSettingString()
{
	memset( m_szSettingString, 0, sizeof(m_szSettingString) );

	wsprintf( m_szSettingString, "(%d:%dx%d:%d):%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
								 (int)m_DefaultSpec,
								 m_Option.m_iWidth,
								 m_Option.m_iHeight,
								 (int)m_Option.m_bFullScreen,

								 (int)m_Option.m_bTrailGlow,
								 (int)m_Option.m_AntiAliasingQuality,
								 (int)m_Option.m_MapObjectQuality,
								 (int)m_Option.m_bShowMapEffect,
								 (int)m_Option.m_bShadow,
								 (int)m_Option.m_LightQuality,
								 (int)m_Option.m_bUseObjectEdge,
								 (int)m_Option.m_bUseMapEdge,
								 (int)m_Option.m_TextureQuality,
								 (int)m_Option.m_EffectQuality,
								 (int)m_Option.m_bSoftSkinning,
								 (int)m_Option.m_bUse16bitFrameBuffer,
								 (int)m_Option.m_bSpecAutoCheck,
								 (int)m_Option.m_bSwapCopy,
								 (int)m_Option.m_bGlow,
								 (int)m_Option.m_FilterType,
								 (int)m_Option.m_OverayType );

	return m_szSettingString;
}

void Setting::SetOptionBySpec( SettingOption &rkOption, Spec eSpec )
{
	bool bFullScreen = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsStartFullScreen() ) 
		bFullScreen = true;

	switch( eSpec )
	{
	case HIGH_SPEC:
		rkOption.m_bFullScreen = bFullScreen;
		rkOption.m_bShadow = true;
		rkOption.m_bGlow = false;
		rkOption.m_bShowMapEffect = true;
		rkOption.m_bTrailGlow = true;
		rkOption.m_bSoftSkinning = false;
		rkOption.m_bUse16bitFrameBuffer = false;
		rkOption.m_bUseMapEdge = true;
		rkOption.m_bUseObjectEdge = true;
		rkOption.m_bSwapCopy = false;

		rkOption.m_AntiAliasingQuality = AQ_NONE;
		rkOption.m_TextureQuality = TQ_HIGH;
		rkOption.m_LightQuality  = LQ_HIGH;
		rkOption.m_EffectQuality = EQ_HIGH;
		rkOption.m_MapObjectQuality = MQ_HIGH;

		rkOption.m_FilterType = FILTER_NONE;
		rkOption.m_OverayType = OVERAY_NONE;
		rkOption.m_iWidth  = 800;
		rkOption.m_iHeight = 600;
		break;
	case MIDDLE_SPEC:
		rkOption.m_bFullScreen = bFullScreen;
		rkOption.m_bShadow = false;
		rkOption.m_bGlow = false;
		rkOption.m_bShowMapEffect = true;
		rkOption.m_bTrailGlow = false;
		rkOption.m_bSoftSkinning = false;
		rkOption.m_bUse16bitFrameBuffer = false;
		rkOption.m_bUseMapEdge = true;
		rkOption.m_bUseObjectEdge = true;
		rkOption.m_bSwapCopy = false;

		rkOption.m_AntiAliasingQuality = AQ_NONE;
		rkOption.m_TextureQuality = TQ_HIGH;
		rkOption.m_LightQuality  = LQ_MIDDLE;
		rkOption.m_EffectQuality = EQ_MIDDLE;
		rkOption.m_MapObjectQuality = MQ_MIDDLE;

		rkOption.m_FilterType = FILTER_NONE;
		rkOption.m_OverayType = OVERAY_NONE;
		rkOption.m_iWidth  = 800;
		rkOption.m_iHeight = 600;
		break;
	case LOW_SPEC:
		rkOption.m_bFullScreen = bFullScreen;
		rkOption.m_bShadow = false;
		rkOption.m_bGlow = false;
		rkOption.m_bShowMapEffect = false;
		rkOption.m_bTrailGlow = false;
		rkOption.m_bSoftSkinning = true;
		rkOption.m_bUse16bitFrameBuffer = true;
		rkOption.m_bUseMapEdge = true;
		rkOption.m_bUseObjectEdge = false;
		rkOption.m_bSwapCopy = false;

		rkOption.m_AntiAliasingQuality = AQ_NONE;
		rkOption.m_TextureQuality = TQ_MIDDLE;
		rkOption.m_LightQuality = LQ_MIDDLE;
		rkOption.m_EffectQuality = EQ_LOW;
		rkOption.m_MapObjectQuality = MQ_LOW;

		rkOption.m_FilterType = FILTER_NONE;
		rkOption.m_OverayType = OVERAY_NONE;
		rkOption.m_iWidth  = 800;
		rkOption.m_iHeight = 600;
		break;
	case MINIMUM_SPEC:
		rkOption.m_bFullScreen = bFullScreen;
		rkOption.m_bShadow = false;
		rkOption.m_bGlow = false;
		rkOption.m_bShowMapEffect = false;
		rkOption.m_bTrailGlow = false;
		rkOption.m_bSoftSkinning = true;
		rkOption.m_bUse16bitFrameBuffer = true;
		rkOption.m_bUseMapEdge = false;
		rkOption.m_bUseObjectEdge = false;
		rkOption.m_bSwapCopy = false;

		rkOption.m_AntiAliasingQuality = AQ_NONE;
		rkOption.m_TextureQuality = TQ_LOW;
		rkOption.m_LightQuality = LQ_LOW;
		rkOption.m_EffectQuality = EQ_LOW;
		rkOption.m_MapObjectQuality = MQ_LOW;

		rkOption.m_FilterType = FILTER_NONE;
		rkOption.m_OverayType = OVERAY_NONE;
		rkOption.m_iWidth  = 800;
		rkOption.m_iHeight = 600;
		break;
	case VERY_HIGH_SPEC:
		rkOption.m_bFullScreen = bFullScreen;
		rkOption.m_bShadow = true;
		rkOption.m_bGlow = true;
		rkOption.m_bShowMapEffect = true;
		rkOption.m_bTrailGlow = true;
		rkOption.m_bSoftSkinning = false;
		rkOption.m_bUse16bitFrameBuffer = false;
		rkOption.m_bUseMapEdge = true;
		rkOption.m_bUseObjectEdge = true;
		rkOption.m_bSwapCopy = false;

		if( m_EnableSampleList.empty() )
		{
			// 아직 확인할수 없는 시점으로
			// m_EnableSampleList가 채워질때 현재 스펙이 VERY_HIGH_SPEC이면 확인해서 설정한다.
			rkOption.m_AntiAliasingQuality = AQ_NONE;
		}
		else
		{
			rkOption.m_AntiAliasingQuality = ModifyAvailableQuality( AQ_LOW );
		}

		rkOption.m_TextureQuality = TQ_HIGH;
		rkOption.m_LightQuality  = LQ_HIGH;
		rkOption.m_EffectQuality = EQ_HIGH;
		rkOption.m_MapObjectQuality = MQ_HIGH;

		rkOption.m_FilterType = FILTER_NONE;
		rkOption.m_OverayType = OVERAY_NONE;
		rkOption.m_iWidth  = 1024;
		rkOption.m_iHeight = 768;
		break;
	}
}

void Setting::SetDimensionConvertInfo( const D3DVIEWPORT9 &rkViewPort, 
									   const D3DXMATRIX &rkView,
									   const D3DXMATRIX &rkProj )
{
	m_ViewPort = rkViewPort;
	m_matView  = rkView;
	m_matProj  = rkProj;
}

bool Setting::Check2DInScreen( const D3DXVECTOR3 &v3DPos, int &iXPos, int &iYPos )
{
	D3DXVECTOR3 v2DPos = Convert3DTo2D( v3DPos );

	iXPos = (int)v2DPos.x;
	iYPos = (int)v2DPos.y;

	if( !COMPARE( v2DPos.z, 0.0f, FLOAT1 ) )
		return false;

	if( !COMPARE( iXPos, 0, Width() ) || !COMPARE( iYPos, 0, Height() ) )
		return false;

	return true;
}

D3DXVECTOR3 Setting::Convert3DTo2D( const D3DXVECTOR3 &v3DPos )
{
	D3DXVECTOR3 v2DPos;
	D3DXVec3Project( &v2DPos, &v3DPos, &m_ViewPort, &m_matProj, &m_matView, &m_matIdentity );

	return v2DPos;
}

int Setting::Width() 
{
	return m_Option.m_iWidth;
}

int Setting::Height() 
{
	return m_Option.m_iHeight;
}

int Setting::GetReservedMode()
{
	return m_iReservedMode;
}
