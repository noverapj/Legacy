#pragma once

class ioUI3DRender;
class ioCreateChar;
class ioBaseChar;

class ioINILoader;

struct CHARACTER;

class io3DSkillCharIcon
{
private:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_pAvata;

	int	m_iXPos;
	int	m_iYPos;

	int m_iXOffSet;
	int m_iYOffSet;
	ioHashString m_szCharMotion;

	DWORD m_dwSetStart;

public:
	void Initialize();
	void SetAvata( const CHARACTER &rkInfo );
	void FaceChange( const char *szTexPostfix, int iDuration );

	void Update( float fTimePerSec );
	void Render( int iXPos, int iYPos );

protected:
	void Load();

public:
	io3DSkillCharIcon();
	~io3DSkillCharIcon();
};

