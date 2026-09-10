#pragma once

#include "ioAIDefinitions.h"
#include "ioAIConfigData.h"

#include <map>
#include <string>
class ioAISymbolTable : public Singleton< ioAISymbolTable >
{
public:
	ioAISymbolTable();
	~ioAISymbolTable();

	void	Init();
	void	InitTextTable();
	void	InitSymbolTable();
	void	InitRegExTable();

	bool	LoadConfiguration( const std::string& configfile );
	bool	LoadRegExFile( const std::string& filename, const std::string& version = "" );
	bool	LoadTextFile( const std::string& filename, const std::string& version = "" );
	bool	LoadSymbolTableFile( const std::string& filename, const std::string& version = "" );

	void	BuildRegEx( ioXMLElement* node );

	bool	IsDuplicatedSymbolValue( const std::string& sec, const std::string& key );
	bool	IsDuplicatedSymbolString( const std::string& key );
	bool	IsClientSymbol( const int& loadtype );
	bool	IsForceIntSet( const int& loadtype );

	typedef union	
	{
		AIDatabaseType		dt;
		AIBvTNodeType		btnt;
		AIBvTHaltType		btht;
		AIBvTRoleType		btrt;
		AIDefElemType		det;
		AIBrainElemType		bet;
		AIKeyInputType		kit;
		AIKeyStateType		kst;
		AIConfigType		ct;
		CharState			cs;
		AIExecVariableType	avt;
		AIExecOperatorType	aot;
		AIExecMethodType	amt;
		AICompareType		cmpt;
		int					n;
	} SymbolValue;

	int			GetSymbolValue( const std::string& sec, const std::string& key );	

	CharState			GetCharStateType(		const std::string& key );
	AIDatabaseType		GetDatabaseType(		const std::string& key );
	AIBvTNodeType		GetBvTNodeType(			const std::string& key );
	AIBvTHaltType		GetBvTHaltType(			const std::string& key );
	AIBvTRoleType		GetBvTRoleType(			const std::string& key );
	AIKeyInputType		GetKeyInputType(		const std::string& key );
	AIKeyStateType		GetKeyStateType(		const std::string& key );
	AIBrainElemType		GetBrainElemType(		const std::string& key );
	AIDefElemType		GetDefElemType(			const std::string& key );
	AIConfigType		GetConfigurationType(	const std::string& key );
	AIExecVariableType	GetExecVariableType(	const std::string& key );
	AIExecOperatorType	GetExecOperatorType(	const std::string& key );
	AIExecMethodType	GetExecMethodType(		const std::string& key );
	AICompareType		GetCompareType(			const std::string& key );
	int					GetSymbolValue(			const std::string& key );	
	std::string			GetSymbolString(		const std::string& key );	
	
	// find string from table using inverted table
	std::string	GetSymbolKey( const std::string& sec, const int& val );	

	std::string GetSymbolKey( const CharState&			val );
	std::string GetSymbolKey( const AIDatabaseType&		val );
	std::string GetSymbolKey( const AIBvTNodeType&		val );
	std::string GetSymbolKey( const AIBvTHaltType&		val );
	std::string GetSymbolKey( const AIBvTRoleType&		val );
	std::string GetSymbolKey( const AIKeyInputType&		val );
	std::string GetSymbolKey( const AIKeyStateType&		val );
	std::string GetSymbolKey( const AIBrainElemType&	val );
	std::string GetSymbolKey( const AIDefElemType&		val );
	std::string GetSymbolKey( const AIConfigType&		val );
	std::string GetSymbolKey( const AIExecVariableType&	val );
	std::string GetSymbolKey( const AIExecOperatorType&	val );
	std::string GetSymbolKey( const AIExecMethodType&	val );
	std::string GetSymbolKey( const AICompareType&		val );
	std::string GetSymbolKey( const int&				val );

	void SetSymbolTable( const std::string& sect, const std::string& key, const int& val, bool force = false );

	void SetSymbolTable( const std::string& key, const CharState&			val );
	void SetSymbolTable( const std::string& key, const AIDatabaseType&		val );
	void SetSymbolTable( const std::string& key, const AIBvTNodeType&		val );
	void SetSymbolTable( const std::string& key, const AIBvTHaltType&		val );
	void SetSymbolTable( const std::string& key, const AIBvTRoleType&		val );
	void SetSymbolTable( const std::string& key, const AIKeyInputType&		val );
	void SetSymbolTable( const std::string& key, const AIKeyStateType&		val );
	void SetSymbolTable( const std::string& key, const AIBrainElemType&		val );
	void SetSymbolTable( const std::string& key, const AIDefElemType&		val );
	void SetSymbolTable( const std::string& key, const AIConfigType&		val );
	void SetSymbolTable( const std::string& key, const AIExecVariableType&	val );
	void SetSymbolTable( const std::string& key, const AIExecOperatorType&	val );
	void SetSymbolTable( const std::string& key, const AIExecMethodType&	val );
	void SetSymbolTable( const std::string& key, const AICompareType&		val );
	void SetSymbolTable( const std::string& key, const int&					val );
	void SetSymbolTable( const std::string& key, const std::string&			val );

public:
	static ioAISymbolTable&		GetSingleton();

	typedef	std::map<std::string,std::string>	StringTable;
	typedef	std::map<std::string,SymbolValue>	SymbolTable;
	typedef	std::map<int,std::string>			InvSymTable;
	typedef	std::map<std::string,SymbolTable>	SymbolMap;
	typedef	std::map<std::string,InvSymTable>	InvSymMap;

	SymbolTable&	GetSymbolTable( const std::string& sect );
	InvSymTable&	GetInvSymTable( const std::string& sect );
	SymbolMap&		GetSymbolMap();
	InvSymMap&		GetInvSymMap();

	typedef	std::vector<ioAIConfigData*>		ConfigList;
	
	ConfigList&		GetConfigList();

private:
	StringTable	m_mapAIStringMap;
	SymbolMap	m_mapAISymbolMap;
	InvSymMap	m_mapAIInvSymMap;
	int			m_nIndex;

	ConfigList	m_vConfigs;
};	

#define g_AISymbolTable ioAISymbolTable::GetSingleton()