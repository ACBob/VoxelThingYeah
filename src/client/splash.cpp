#include "splash.hpp"

#include <chrono>

#include "logging.hpp"

#include "cvar_clientside.hpp"

// TODO: this is a bit if else()-y, and a bit of a mess.
// But it works!
std::string GetSplashText(std::vector<std::string> const& texts) {
    if (texts.empty()) {
        return "I'm not mad, just disappointed.";        
    }
    
    // Get the current date
    time_t raw_time = time( NULL );
    struct tm * timeinfo = localtime ( &raw_time );
    int day = timeinfo->tm_mday;
    int month = timeinfo->tm_mon + 1;
    int year = timeinfo->tm_year + 1900;

    // con_debug("YEAR: %d\nMONTH: %d\nDAY: %d\n", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);

    // Jen's birthday (30th of November)
    if (month == 11 && day == 30)
        return "Happy birthday, Jenifar Shifaketro!";

    // My birthday (28th of August)
    if (month == 8 && day == 28)
        return "Happy birthday, Baob Koiss!";

    // Anniversary of Me & Jen (24th of May)
    if (month == 5 && day == 24) {
        // Get the years since the last anniversary
        int years = year - 2021; // (we got together in 2021)
        // get ordinal suffix
        std::string suffix = "th";
        if (years % 10 == 1) {
            suffix = "st";
        } else if (years % 10 == 2) {
            suffix = "nd";
        } else if (years % 10 == 3) {
            suffix = "rd";
        }

        return "Happy " + std::to_string(years) + suffix + " anniversary, Baob & Jenifar!";
    }
    
    if (month == 1 && day == 1)
        return "Happy new year!";
    if (month == 2 && day == 14)
        return "Happy Valentine's Day!";
    if (month == 3 && day == 17)
        return "Happy St. Patrick's Day!";
    if (month == 4 && day == 1)
        return "Happy April Fools' Day!";
    if (month == 10 && day == 31)
        return "Happy Halloween!";
    if (month == 12 && day == 31)
        return "Happy New Year's Eve!";

    if (month == 9 && day == 19)
        return "Yarr, Happy speak like a pirate day!";

    // Rememberance day, UK
    if (month == 11 && day == 11)
        return "";
    
    

    return texts[rand() % texts.size()];
}