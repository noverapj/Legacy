#include "NMCONew.h"
#include "NMManager.h"

#include "NMGSGeneralSession.h"

#define DEFAULT_BUFFER_SIZE		( 1024 * 1024 )		// 1 Mega Bytes

#ifdef _NM_DYNAMIC_LOAD
	wapp::CDynamicFunctionT< BOOL ( __cdecl* )( _UInt32_, BYTE*, _UInt8_, BYTE**, _UInt32_& ) >		s_pNMCO_CallNMFunc2;
	wapp::CDynamicFunctionT< void ( __cdecl* )( LPVOID ) >											s_pNMCO_MemoryFree;
	wapp::CDynamicFunctionT< BOOL ( __cdecl* )( BOOL ) >											s_pNMCO_SetPatchOption;
	wapp::CDynamicFunctionT< BOOL ( __cdecl* )( BOOL ) >											s_pNMCO_SetUseFriendModuleOption;
	wapp::CDynamicFunctionT< BOOL ( __cdecl* )( LPCTSTR ) >											s_pNMCO_SetVersionFileUrl;
	wapp::CDynamicFunctionT< BOOL ( __cdecl* )( BOOL ) >											s_pNMCO_SetUseNGMOption;
	wapp::CDynamicFunctionT< BOOL ( __cdecl* )( NMLOCALEID ) >										s_pNMCO_SetLocale;
	wapp::CDynamicFunctionT< BOOL ( __cdecl* )( NMLOCALEID, NMREGIONCODE ) >						s_pNMCO_SetLocaleAndRegion;
#endif

CNMManager::CNMManager(void)
{
	::ZeroMemory( &this->m_cs, sizeof( this->m_cs ) );
	::InitializeCriticalSection( &this->m_cs );

	this->m_pRetData	= new BYTE[ DEFAULT_BUFFER_SIZE ];
	this->m_codePage	= CP_ACP;
}

CNMManager::~CNMManager(void)
{
	::DeleteCriticalSection( &this->m_cs );

	if ( this->m_pRetData != NULL )
		delete []	this->m_pRetData;
}

void CNMManager::Init( void )
{
}

void CNMManager::Finalize( void )
{
}

BOOL CNMManager::CallNMFunc( CNMFunc* pFunc )
{
	BOOL	bRet = FALSE;

	if ( pFunc == NULL )
		return FALSE;

	NMCSLOCK( this->m_cs )
	{
		CNMSimpleStream		ssStream;
		_UInt32_			uRetDataLen		= DEFAULT_BUFFER_SIZE;
		BYTE*				pBuffer			= this->m_pRetData;

		ssStream.SetCodePage( this->GetCodePage() );

		pFunc->SetCalling();
		pFunc->Serialize( ssStream );

		ssStream.Encrypt( ssStream.GetLatestCryptorVersion() );

#ifdef	_NM_DYNAMIC_LOAD
		if (!s_pNMCO_CallNMFunc2.IsValid() || !s_pNMCO_MemoryFree.IsValid())
		{
			s_pNMCO_MemoryFree.Load( NMCONEW_DLL_NAME, "NMCO_MemoryFree" );
			s_pNMCO_CallNMFunc2.Load( NMCONEW_DLL_NAME, "NMCO_CallNMFunc2" );
		}

		if (!s_pNMCO_CallNMFunc2.IsValid() || !s_pNMCO_MemoryFree.IsValid())
		{
			return FALSE;
		}

		if ((*s_pNMCO_CallNMFunc2)(pFunc->GetCode(), ssStream.GetBufferPtr(), ssStream.GetStreamVersion(), &pBuffer, uRetDataLen) == TRUE)
#else
		if ( TRUE == ::NMCO_CallNMFunc2( pFunc->GetCode(), ssStream.GetBufferPtr(), ssStream.GetStreamVersion(), &pBuffer, uRetDataLen ) )
#endif
		{
			ssStream.SetBuffer( pBuffer );

			pFunc->SetReturn();
			pFunc->DeSerialize( ssStream );

			bRet	= pFunc->bSuccess;

			if ( pBuffer != this->m_pRetData )
			{
#ifdef	_NM_DYNAMIC_LOAD
				(*s_pNMCO_MemoryFree)(pBuffer);
#else
				::NMCO_MemoryFree( pBuffer );
#endif
			}
		}
	}

	return bRet;
}


void CNMManager::MemFree( LPVOID pBuffer )
{
#ifdef	_NM_DYNAMIC_LOAD
	if (!s_pNMCO_MemoryFree.IsValid())
	{
		s_pNMCO_MemoryFree.Load(NMCONEW_DLL_NAME, "NMCO_MemoryFree");
	}
	if (s_pNMCO_MemoryFree.IsValid())
	{
		(*s_pNMCO_MemoryFree)(pBuffer);
	}
#else
	::NMCO_MemoryFree( pBuffer );
#endif
}

BOOL CNMManager::SetPatchOption( BOOL bPatch )
{
#if defined(_NM_DYNAMIC_LOAD)
	if (!s_pNMCO_SetPatchOption.IsValid())
	{
		s_pNMCO_SetPatchOption.Load(NMCONEW_DLL_NAME, "NMCO_SetPatchOption");
	}
	if (s_pNMCO_SetPatchOption.IsValid())
	{
		return (*s_pNMCO_SetPatchOption)(bPatch);
	}
	return FALSE;
#else
	return ::NMCO_SetPatchOption(bPatch);
#endif
}

BOOL CNMManager::SetUseFriendModuleOption( BOOL bUseFriendModule )
{
#if defined(_NM_DYNAMIC_LOAD)
	if (!s_pNMCO_SetUseFriendModuleOption.IsValid())
	{
		s_pNMCO_SetUseFriendModuleOption.Load(NMCONEW_DLL_NAME, "NMCO_SetUseFriendModuleOption");
	}
	if (s_pNMCO_SetUseFriendModuleOption.IsValid())
	{
		return (*s_pNMCO_SetUseFriendModuleOption)(bUseFriendModule);
	}
	return FALSE;
#else
	return ::NMCO_SetUseFriendModuleOption(bUseFriendModule);
#endif
}

BOOL CNMManager::SetVersionFileUrl( LPCTSTR pszVersionFileUrl )
{
#if defined(_NM_DYNAMIC_LOAD)
	if (!s_pNMCO_SetVersionFileUrl.IsValid())
	{
		s_pNMCO_SetVersionFileUrl.Load(NMCONEW_DLL_NAME, "NMCO_SetVersionFileUrl");
	}
	if (s_pNMCO_SetVersionFileUrl.IsValid())
	{
		return (*s_pNMCO_SetVersionFileUrl)(pszVersionFileUrl);
	}
	return FALSE;
#else
	return ::NMCO_SetVersionFileUrl(pszVersionFileUrl);
#endif
}

BOOL CNMManager::SetUseNGMOption( BOOL bUseNGM )
{
#if defined(_NM_DYNAMIC_LOAD)
	if (!s_pNMCO_SetUseNGMOption.IsValid())
	{
		s_pNMCO_SetUseNGMOption.Load(NMCONEW_DLL_NAME, "NMCO_SetUseNGMOption");
	}
	if (s_pNMCO_SetUseNGMOption.IsValid())
	{
		return (*s_pNMCO_SetUseNGMOption)(bUseNGM);
	}
	return FALSE;
#else
	return ::NMCO_SetUseNGMOption(bUseNGM);
#endif
}

BOOL CNMManager::SetLocale( NMLOCALEID uLocaleID )
{
#if defined(_NM_DYNAMIC_LOAD)
	if (!s_pNMCO_SetLocale.IsValid())
	{
		s_pNMCO_SetLocale.Load(NMCONEW_DLL_NAME, "NMCO_SetLocale");
	}
	if (s_pNMCO_SetLocale.IsValid())
	{
		return (*s_pNMCO_SetLocale)( uLocaleID );
	}
	return FALSE;
#else
	return ::NMCO_SetLocale( uLocaleID );
#endif
}

BOOL CNMManager::SetLocaleAndRegion( NMLOCALEID uLocaleID, NMREGIONCODE uRegionCode )
{
#if defined( _NM_DYNAMIC_LOAD )
	if( !s_pNMCO_SetLocaleAndRegion.IsValid() )
		s_pNMCO_SetLocaleAndRegion.Load( NMCONEW_DLL_NAME, "NMCO_SetLocaleAndRegion" );

	if( s_pNMCO_SetLocaleAndRegion.IsValid() )
		return ( *s_pNMCO_SetLocaleAndRegion )( uLocaleID, uRegionCode );

	return FALSE;
#else
	return ::NMCO_SetLocaleAndRegion( uLocaleID, uRegionCode );
#endif
}

BOOL CNMManager::SetCodePage( UINT32 uCodePage )
{
	this->m_codePage = uCodePage;
	return TRUE;
}

UINT32 CNMManager::GetCodePage()
{	
	return this->m_codePage;
}
