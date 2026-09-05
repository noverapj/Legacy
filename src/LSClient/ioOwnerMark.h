#pragma once

class ioUIRenderImage;

class ioOwnerMark
{
protected:
	enum 
	{
		MAX_FRAME = 8,
	};
protected:
	ioUIRenderImage *m_pOwnerMark;

	TeamType         m_TeamType;

	DWORD            m_dwFrameTimer;
	int              m_iFrame;
	
public:
	void SetTeam( TeamType eTeam );

public:
	void Update();
	void Render( float fXPos, float fYPos );

private:
	bool IsResouceOK() const;
	void ClearResource();

public:
	ioOwnerMark(void);
	virtual ~ioOwnerMark(void);
};

