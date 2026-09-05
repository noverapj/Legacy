

#ifndef _ioEtcItemManager_h_
#define _ioEtcItemManager_h_

#include "../Util/Singleton.h"
#include "ioEtcItem.h"

enum ClassShopType
{
	CST_MIN	=	0,
	CST_MAX	=	1,
	CST_END	=	2
};

class ioEtcItemManager : public Singleton< ioEtcItemManager >
{
public:
	enum 
	{
		CLASS_ETC_ITEM_TYPE = 1, 
		USE_TYPE_POS        = 1000000, 
		FIRST_TYPE_POS      = 100000, 
		SECOND_TYPE_POS     = 100,
	};
protected:
	typedef std::map< DWORD, ioEtcItem* > EtcItemMap;
	EtcItemMap m_EtcItemMap;

public:
	void LoadEtcItem( const char *szFileName, bool bCreateLoad = true );

protected:
	void ParseEtcItem( ioINILoader &rkLoader, int iIndex, bool bCreateLoad );
	DWORD GetEtcItemTypeExceptClass( DWORD dwTypeWithClass );
	ioEtcItem *CreateEtcItem( DWORD dwType );
	void Clear();

public:
	ioEtcItem* FindEtcItem( DWORD dwType );
	const ioEtcItem* GetEtcItem( int iIdx ) const;

public:
	int  GetEtcItemCount() const;
	bool IsRightClass( DWORD dwType );
	void SendJoinUser( User *pExistUser, User *pNewUser );

	bool IsBlockEtcItem( DWORD dwType );
	bool IsUsableClassShopLevel( int UserLevel );
public:
	static ioEtcItemManager& GetSingleton();

public:
	ioEtcItemManager();
	virtual ~ioEtcItemManager();
private:
	int	m_arrClassShopPermitLv[CST_END];
};

#define g_EtcItemMgr ioEtcItemManager::GetSingleton()

#endif
