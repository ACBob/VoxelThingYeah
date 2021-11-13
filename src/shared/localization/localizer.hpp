// Localization system
// Loads language files and translates strings
// Langauge definitions are in the "lang.toml" file

#pragma once

#include <vector>
#include <string>
#include <map>

struct Language {
    std::string code;
    std::string friendlyName;
    std::string langFile;
};

class CLocalizer
{
    public:
        CLocalizer();
        ~CLocalizer();

        std::vector<std::pair<std::string, std::string>> ListLanguages();

        // (Re)loads the language files
        void LoadLanguages();

        bool SetLanguage(const char *lang);

        // Gets the translated string
        const char* GetString(const char *key);
        // Gets the translated string, with printf formatting
        const char* GetStringFMT(const char *fmt, ...);

    private:
        // Language to fall back to if the key is not found in the current language
        const std::string m_fallBackLanguage = "en";
        std::string m_currentLanguage = "en";

        std::map<std::string, Language> m_languages; // All languages
        std::map<std::string, std::string> m_strings; // Strings for the current language
        std::map<std::string, std::string> m_fallBackStrings; // Strings for the fallback language
        
        void LoadLangaugeInto(std::string lang, std::map<std::string, std::string>& langMap);
};