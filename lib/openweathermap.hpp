// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#if !defined(__com_openmono_openweathermap_h)
#define __com_openmono_openweathermap_h
#include "ibytebuffer.hpp"
#include "jsonparser.hpp"
#include "weather.hpp"
#include <vector>

namespace openweathermap {

using namespace weather;
using namespace json;

Condition translateIcon (uint8_t const * icon)
{
    char const * iconTable[] =
    {
        // See http://openweathermap.org/weather-conditions
        "01d","02d","03d","04d","10d","09d","11d","13d","50d",
        "01n","02n","03n","04n","10n","09n","11n","13n","50n",
        0
    };
    for (size_t i = 0; iconTable[i] != 0; ++i)
    {
        if (strcmp((char const *)icon,iconTable[i]) == 0)
            return Condition(i);
    }
    return Unknown;
}

Entry parseCurrent (IByteBuffer const & buffer)
{
    Json json (buffer);
    Entry entry;
    entry.city = json.lookup("/name");
    entry.temperatureK = json.lookup("/main/temp");
    entry.cloudPercentage = json.lookup("/clouds/all");
    entry.humidity = json.lookup("/main/humidity");
    entry.pressureHpa = json.lookup("/main/pressure");
    entry.windSpeedMs = json.lookup("/wind/speed");
    entry.windDirection = json.lookup("/wind/deg");
    entry.timeUnix = json.lookup("/dt");
    entry.condition = translateIcon(json.lookup("/weather/0/icon"));
    uint8_t const * rainNode = json.lookup("/rain/3h");
    if (0 == rainNode)
        entry.rain = (uint8_t const *)"";
    else
        entry.rain = rainNode;
    return entry;
}

std::vector<Entry> parseForecast (IByteBuffer const & buffer, size_t entries)
{
    struct State
    {
        Json json;
        State (IByteBuffer const & buffer)
        :
            json(buffer)
        {
            city = json.lookup("/city/name");
            strcpy(path,"/list");
        }
        std::vector<Entry> entries;
        char path[128];
        char const * indexedPath (size_t index, char const * slug)
        {
            strcpy(path,"/list/");
            char const * intSize = (sizeof(size_t)==sizeof(unsigned)) ? "%u" : "%lu";
            size_t indexLength = sprintf(path+6,intSize,index);
            strcpy(path+6+indexLength,slug);
            return path;
        }
        uint8_t const * city;
        Condition extractCondition (size_t index)
        {
            uint8_t const * icon = json.lookup(indexedPath(index,"/weather/0/icon"));
            return translateIcon(icon);
        }
        void extractEntry (size_t index)
        {
            Entry entry;
            entry.city = city;
            entry.temperatureK = json.lookup(indexedPath(index,"/main/temp"));
            entry.cloudPercentage = json.lookup(indexedPath(index,"/clouds/all"));
            entry.humidity = json.lookup(indexedPath(index,"/main/humidity"));
            entry.pressureHpa = json.lookup(indexedPath(index,"/main/pressure"));
            entry.windSpeedMs = json.lookup(indexedPath(index,"/wind/speed"));
            entry.windDirection = json.lookup(indexedPath(index,"/wind/deg"));
            entry.timeUnix = json.lookup(indexedPath(index,"/dt"));
            entry.condition = extractCondition(index);
            uint8_t const * rainNode = json.lookup(indexedPath(index,"/rain/3h"));
            if (0 == rainNode)
                entry.rain = (uint8_t const *)"";
            else
                entry.rain = rainNode;
            entries.push_back(entry);
        }
    };
    State state(buffer);
    for (size_t i = 0; i < entries; ++i) state.extractEntry(i);
    return state.entries;
}

} // openweathermap

#endif // __com_openmono_openweathermap_h
