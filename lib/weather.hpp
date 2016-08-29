// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#if !defined(__com_openmono_weather_h)
#define __com_openmono_weather_h
#include <string.h>

namespace weather {

enum Condition
{
    Day_ClearSky, // <10%, 01d
    Day_FewClouds, // <50%, 02d
    Day_ScatteredClouds, // >50%, 03d
    Day_Overcast, // >90%, 04d
    Day_Rain, // 10d
    Day_ShowerRain, // 09d
    Day_Thunder, // 11d
    Day_Snow, // 13d
    Day_Mist, // 50d
    Night_ClearSky, // <10%, 01n
    Night_FewClouds, // <50%, 02n
    Night_ScatteredClouds, // >50%, 03n
    Night_Overcast, // >90%, 04n
    Night_Rain, // 10n
    Night_ShowerRain, // 09n
    Night_Thunder, // 11n
    Night_Snow, // 13n
    Night_Mist, // 50n
    Unknown
};

struct Entry
{
    uint8_t const * city;
    uint8_t const * temperatureK;
    uint8_t const * cloudPercentage;
    uint8_t const * windSpeedMs;
    uint8_t const * windDirection;
    uint8_t const * humidity;
    uint8_t const * pressureHpa;
    uint8_t const * timeUnix;
    uint8_t const * rain;
    Condition condition;
    bool operator == (Entry const & rhs) const
    {
#       define different(x,y) (strcmp((char const *)x,(char const *)y)!=0)
        if (different(city,rhs.city)) return false;
        if (different(temperatureK,rhs.temperatureK)) return false;
        if (different(cloudPercentage,rhs.cloudPercentage)) return false;
        if (different(windSpeedMs,rhs.windSpeedMs)) return false;
        if (different(windDirection,rhs.windDirection)) return false;
        if (different(humidity,rhs.humidity)) return false;
        if (different(pressureHpa,rhs.pressureHpa)) return false;
        if (different(timeUnix,rhs.timeUnix)) return false;
        if (condition != rhs.condition) return false;
        if (rain != rhs.rain) return false;
        return true;
    }
    bool operator != (Entry const & rhs) const
    {
        return !(operator==(rhs));
    }
};

} // weather

#endif // __com_openmono_weather_h
