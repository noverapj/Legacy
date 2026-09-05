#ifndef _SuperGashponLimitManager_h_
#define _SuperGashponLimitManager_h_

class ServerNode;
class SuperGashponLimitManager : public Singleton< SuperGashponLimitManager >
{

protected:
	struct SuperGashponLimit
	{
		DWORD m_dwEtcItemCode;		
		DWORD m_dwLimit;

		SuperGashponLimit()
		{
			m_dwEtcItemCode	 = 0;
			m_dwLimit		 = 0;
		}
	};
	typedef std::vector<SuperGashponLimit> vSuperGashponLimit;

protected:
	vSuperGashponLimit m_vSuperGashponLimit;

public:
	void ServerDownAllSave();

	void InitLimit();
	void SaveLimit();
	
	SuperGashponLimitManager::SuperGashponLimit& GetLimitData( DWORD dwEtcItemCode );

public:
	bool IncraseLimit( DWORD dwEtcItemCode, DWORD dwMaxLimit );
	void DecraseLimit( DWORD dwEtcItemCode );
	DWORD GetLimit( DWORD dwEtcItemCode );

	void LimitReset( DWORD dwEtcItemCode );

public:
	static SuperGashponLimitManager& GetSingleton();

public:
	SuperGashponLimitManager();
	virtual ~SuperGashponLimitManager();
};

#define g_SuperGashponLimitManager SuperGashponLimitManager::GetSingleton()
#endif