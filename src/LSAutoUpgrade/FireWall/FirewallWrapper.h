#ifndef __Firewallwrapper_h__
#define __Firewallwrapper_h__

// 컴파일을 위해서는 Microsoft Platform SDK for Windows Server 2003 R2 필요함

struct INetFwProfile;
class FirewallWrapper
{
	INetFwProfile*  m_pFwProfile;
	HRESULT         m_hrComInit;

	HRESULT InitFirewallProfile();
	FirewallWrapper();



	static FirewallWrapper* Create();
	~FirewallWrapper();

	BOOL    FirewallPresent() 
	{ return m_pFwProfile != NULL; } 

	BOOL    IsFirewallActive();
	HRESULT AddAuthorizedApp(
		IN const wchar_t* szFwProcessImageFileName,
		IN const wchar_t* szFwFriendlyName
		);
	HRESULT RemoveAuthorizedApp(
		IN const wchar_t* szFwProcessImageFileName
		);
	BOOL IsAppEnabled( 
		IN const wchar_t* szFwProcessImageFileName
		);
	BOOL AreExceptionsAllowed();

public:
	static BOOL OnInstallApplication( IN const wchar_t* szFwProcessImageFileName, IN const wchar_t* szFwFriendlyName );
	static BOOL OnUninstallApplication( IN const wchar_t* szFwProcessImageFileName );
	static BOOL CanHostMultiplayer( IN const wchar_t* szFwProcessImageFileName );

};

#endif // __FireWallWrapper_h__
