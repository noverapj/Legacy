#pragma once
class UserInfoManager
{
public:
	UserInfoManager(void);
	virtual ~UserInfoManager(void);

public:
	bool    AddUser(int id,TCHAR* userid);
	bool	DelUser(int id);
	int		GetSize(){return m_users.size();}
	int		GetFirstUserId();
	void	DelTimeOutClients();
 

	void	SendTimePacket( UserInfo_* user ) ;
 

	void	TrySendSvrInfo();
	int     GetTiketInfo(int id);

	template <typename T>
	void IncrementVal(T& val) { val++; if(val > 100000000)val =0;}
protected:
	std::list<UserInfo_*> m_users;
 
};
