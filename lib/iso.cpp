// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "iso.hpp"
#include <string.h>
#include <cstdlib>

time_t Iso::timeZoneToUnixTimeStampDiff (char const * timeZone)
{
    time_t errorCode = -1;
    if (0 == timeZone) return errorCode;
    size_t length = strlen(timeZone);
    if (length < 5 || length > 6) return errorCode;
    if (timeZone[0] != '-' && timeZone[0] != '+') return errorCode;
    time_t hours = atoi(timeZone);
    // Missing colon? Interpret as HHMM.
    if (hours < -12 || hours > 12) return ((hours / 100) * 60 + (hours % 100)) * 60;
    // Skip th ecolon.
    if (timeZone[3] != ':') return errorCode;
    time_t minutes = atoi(timeZone+4);
    return (hours * 60 + minutes) * 60;
}
