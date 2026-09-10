#pragma once

#include <map>
#include <vector>

#include "ioAIDefinitions.h"
#include "ioAIKeyBehavior.h"
#include "ioAIXMLDoc.h"

class	ioXMLElement;
class	ioAIBvT;
class	ioNpcAppearance;
class ioAIDatabase  : public Singleton< ioAIDatabase >
{
public:
	ioAIDatabase();
	~ioAIDatabase();

	typedef std::vector<ioAIXMLDoc*>	XMLList;

	void	BuildAIDatabase();
	void	ClearDatabase();

	void	BuildKeySequenceDB( const std::string& filename, const std::string& ver );
	void	BuildBehaviorTreeDB( const std::string& filename, const std::string& ver );
	void	BuildTextItemDB( const std::string& filename, const std::string& ver );
	void	BuildAppearanceDB( const std::string& filename, const std::string& ver );

	ioAIBvT*	GenerateBehaviorTree(const std::string& elemName, int depth = -1 );
	void		CopyKeySequence(	const std::string& keySeqName, 
									std::vector<ioAIKeyBehavior>& keySeq,
									int	inputTypeOverride = -100,
									int	stateTypeOverride = -100,
									unsigned int	durationOverride = 0);
	std::string	GetTextItem( const std::string& sect, const std::string& key );
	std::string	GetRandomTextItem( const std::string& sect );
	
	ioNpcAppearance* GetRandomAppearanceData();
	int	GetRandomResult( ioXMLElement* node );

public:
	static ioAIDatabase&	GetSingleton();

	typedef	std::map<std::string,std::string>	StringArray;
	typedef	std::map<std::string,StringArray>	StringMap;

protected:
	XMLList		m_vKeySeqDefs;
	XMLList		m_vBehaviorDB;
	ioAIXMLDoc*	m_pAppearanceDB;

	StringMap	m_mapTextItem;
};

#define g_AIDB ioAIDatabase::GetSingleton()