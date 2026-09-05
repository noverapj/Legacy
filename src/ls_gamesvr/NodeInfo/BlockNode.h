#pragma once

class BlockNode
{
public:
	BlockNode(void);
	~BlockNode(void);

	void Init();
	void Destroy();

public:
	void Reset();

	void SetBlockFlag(const bool bBlockFlag);
	bool IsBlocked();

protected:
	bool m_bBlockFlag;

};