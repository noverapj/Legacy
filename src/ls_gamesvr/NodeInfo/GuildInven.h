#pragma once

class GuildInven
{
public:
	GuildInven();
	virtual ~GuildInven();

	void Init();
	void Destroy();

	void DBToData();

public:
	BOOL IsExistItem(const DWORD dwItemIndex);
	BOOL AddItem(const DWORD dwItemIndex, const DWORD dwItemCode);
	BOOL DeleteItem(const DWORD dwItemIndex);

protected:
	typedef boost::unordered_map<DWORD, DWORD> ITEMINFOS;	// <index, code>

protected:
	ITEMINFOS m_mGuildInvenInfos;
};