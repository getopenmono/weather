// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_iso_h
#define __com_openmono_iso_h
#include <stdint.h>
#include <time.h>

/**
 * Convertion functions.
 */
struct Iso
{
    static time_t timeZoneToUnixTimeStampDiff (char const * timeZone);
};

#endif // __com_openmono_iso_h
