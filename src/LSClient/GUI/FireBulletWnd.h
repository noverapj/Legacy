#pragma once

class FireBulletWnd : public ioWnd
{
protected:
	int m_iCurBullet;
	int m_iMaxBullet;
	bool m_bLaserItem;

public:
	void UpdateBullet( int iCur, int iMax, bool bLaserItem = false );

protected:
	virtual void OnRender();

public:
	FireBulletWnd();
	virtual ~FireBulletWnd();
};

