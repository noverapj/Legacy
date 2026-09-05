#pragma once

class NamedTitleBtn : public ioButton
{
protected:
	ioUIRenderFrame *m_pEffectFrm;
	
	DWORD m_dwCheckTime;
	DWORD m_dwChangeTime;
	int m_nCurAlpha;
	int m_nAddAlpha;
	bool m_bIsAddAlpha;
	bool m_bIsEffect;

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void UpdateEffect();

public:
	void SetStartEffect();
	void SetEndEffect();
	bool IsEffect() const { return m_bIsEffect; }

public:
	NamedTitleBtn();
	virtual ~NamedTitleBtn();
};