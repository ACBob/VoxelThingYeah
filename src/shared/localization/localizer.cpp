#include "localizer.hpp"

#include "cvar_shared.hpp"

#include "filesystem.hpp"
#include <types.hpp>

#include <tomlcpp.hpp>

#include "logging.hpp"

#include <algorithm>

CLocalizer::CLocalizer()
{
	m_currentLanguage = cvarLanguage->GetString();

	// Load the language definition file

	int64_t fl					= 0;
	bool bSuccess				= false;
	const uchar_t *languageToml = fileSystem::LoadFile( "/assets/lang/languages.toml", fl, bSuccess );

	toml::Result data = toml::parse( bSuccess ? (char *)languageToml : "\0" );

	for ( std::string key : data.table->keys() )
	{
		auto tbl = data.table->getTable( key );

		// language code is the key
		// friendly name is "friendly_name"
		// the actual language file is "lang_file"
		m_languages[key] = {};

		m_languages[key].code		  = key;
		m_languages[key].friendlyName = tbl->getString( "friendly_name" ).second;
		m_languages[key].langFile	  = tbl->getString( "lang_file" ).second;
	}
	delete[] languageToml;

	// For only the current language, load the language file
	if ( m_languages.find( m_currentLanguage ) == m_languages.end() )
	{
		con_error( "Couldn't find language %s, falling back to %s", m_currentLanguage.c_str(),
				   m_fallBackLanguage.c_str() );
		m_currentLanguage = m_fallBackLanguage;
	}

	if ( m_languages.find( m_currentLanguage ) == m_languages.end() )
	{
		// Create a dummy language
		m_languages[m_currentLanguage]				= {};
		m_languages[m_currentLanguage].code			= "error";
		m_languages[m_currentLanguage].friendlyName = "Unknown";
		m_languages[m_currentLanguage].langFile		= "";
	}

	LoadLanguages();
}

void CLocalizer::LoadLangaugeInto( std::string lang, std::map<std::string, std::string> &langMap )
{
	langMap.clear();

	// Load the language file
	int64_t fl	  = 0;
	bool bSuccess = false;
	const uchar_t *languageTomlFile =
		fileSystem::LoadFile( ( "/assets/lang/" + m_languages[lang].langFile ).c_str(), fl, bSuccess );

	if ( !bSuccess )
	{
		// Warn, but continue
		// (in this situation, you will always see the keys)
		con_error( "Couldn't load language file %s", m_languages[lang].langFile.c_str() );
	}

	toml::Result data = toml::parse( bSuccess ? (char *)languageTomlFile : "\0" );

	// The format of the language file is:
	//
	// [group]
	// key = "value"
	//
	// but also supports:
	//
	// group.key = "value"
	// (which is just inherit because of how we mangle the keys together)
	for ( std::string key : data.table->keys() )
	{
		auto tbl = data.table->getTable( key );

		for ( std::string subKey : tbl->keys() )
		{
			langMap[key + "." + subKey] = tbl->getString( subKey ).second;
			// con_debug( "Loaded %s.%s = %s", key.c_str(), subKey.c_str(), langMap[key + "." + subKey].c_str() );
		}
	}
}

void CLocalizer::LoadLanguages()
{
	LoadLangaugeInto( m_currentLanguage, m_strings );
	if ( m_currentLanguage != m_fallBackLanguage )
	{
		LoadLangaugeInto( m_fallBackLanguage, m_fallBackStrings );
	}
	else
	{
		m_fallBackStrings = m_strings;
	}
}

bool CLocalizer::SetLanguage( const char *lang )
{
	if ( m_languages.find( lang ) == m_languages.end() )
	{
		con_error( "Couldn't find language %s", lang );
		return false;
	}
	m_currentLanguage = lang;

	// Update convar
	cvarLanguage->SetString( lang );

	// Reload the language file
	LoadLanguages();

	return true;
}

CLocalizer::~CLocalizer() {}

std::vector<std::pair<std::string, std::string>> CLocalizer::ListLanguages()
{
	std::vector<std::pair<std::string, std::string>> languages;

	for ( auto lang : m_languages )
	{
		languages.push_back( std::make_pair( lang.first, lang.second.friendlyName ) );
	}

	// Sort the languages by friendly name
	std::sort( languages.begin(), languages.end(),
			   []( const std::pair<std::string, std::string> &a, const std::pair<std::string, std::string> &b ) {
				   return a.second < b.second;
			   } );

	return languages;
}

const char *CLocalizer::GetString( const char *key )
{
	// If the key is not found, return the key
	if ( m_strings.find( key ) == m_strings.end() )
	{
		// Test fallback language first
		if ( m_fallBackStrings.find( key ) != m_fallBackStrings.end() )
		{
			return m_fallBackStrings[key].c_str();
		}

		return key;
	}

	return m_strings[key].c_str();
}

// The printf formatting version of GetString
const char *CLocalizer::GetStringFMT( const char *key, ... )
{
	va_list args;
	va_start( args, key );
	const char *str = GetString( key );
	char *buffer	= new char[strlen( str ) + 1];
	vsprintf( buffer, str, args );
	va_end( args );
	return buffer;
}