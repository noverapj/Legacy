#pragma once


class ioFieldItemMgr : public Singleton< ioFieldItemMgr >
{
protected:
	ioHashString m_DefaultItemEffect;
	ioHashString m_DefaultItemEffectRed;
	ioHashString m_DefaultItemEffectBlue;
	ioHashString m_DefaultItemOverEffect;
	ioHashString m_DefaultItemOverEffectRed;
	ioHashString m_DefaultItemOverEffectBlue;

	CEncrypt<float> m_fMoveDropJumpAmt;
	CEncrypt<float> m_fMoveDropGravityAmt;

public:
	void LoadINIData();

public:
	const ioHashString& GetItemEffect( TeamType eTeam ) const;
	const ioHashString& GetItemOverEffect( TeamType eTeam ) const;
	inline float GetItemJumpAmt() const { return m_fMoveDropJumpAmt; }
	inline float GetItemGravityAmt() const { return m_fMoveDropGravityAmt; }

public:
	static ioFieldItemMgr& GetSingleton();

public:   
	ioFieldItemMgr();
	virtual ~ioFieldItemMgr();
};
#define g_FieldItemMgr ioFieldItemMgr::GetSingleton()
