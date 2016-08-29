// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_sdcardconfiguration_h
#define __com_openmono_sdcardconfiguration_h
#include "lib/iconfiguration.hpp"

/**
 * Configuration store that reads key-value pairs from the SD card.
 * Each value is contained in a file with a path that corresponds to the key.
 */
class SdCardConfiguration
:
    public IConfiguration
{
public:
    enum Status
    {
        Configuration_OK, // 0
        Configuration_InvalidKey, // 1
        Configuration_MissingValue, // 2
        Configuration_MissingSdCard // 3
    };

    SdCardConfiguration ();

    virtual uint8_t const * get (uint8_t const * key) const;

    /**
     * @return operational status of the configuration store
     */
    Status status () const;

private:
    mutable Status _status;
};

#endif // __com_openmono_sdcardconfiguration_h
