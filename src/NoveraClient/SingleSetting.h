#pragma once

class SingleSetting
{
private:
	static ModeType m_ModeType;
	static ioHashString m_MapFileName;

public:
	static void LoadINI();

	static ModeType GetModeType() { return m_ModeType; }
	static ioHashString &GetMapFileName() { return m_MapFileName; }

public:
	SingleSetting();
	~SingleSetting();
};