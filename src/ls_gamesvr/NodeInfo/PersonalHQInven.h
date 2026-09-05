#pragma once

class User;

class PersonalHQInven
{
public:
	PersonalHQInven();
	virtual ~PersonalHQInven();

	void Init(User* pUser);
	void Destroy();

public:
	void DBtoData( CQueryResultData *query_data );	
	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	int AddBlockCount(const DWORD dwItemCode, const int iCount);
	int DecreaseBlockCount(const DWORD dwItemCode, const int iCount);

	void FillAllBlockInfo(SP2Packet &rkPacket);

	void SendInvenInfo();

public:
	int GetItemCount(const DWORD dwItemCode);

protected:
	typedef boost::unordered_map<DWORD, int> INVENINFO;	//<코드, 아이템수>

protected:
	User* m_pUser;
	INVENINFO m_mUserInvenInfo;
};