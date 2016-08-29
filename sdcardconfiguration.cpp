// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "sdcardconfiguration.hpp"
#include "sdcard.hpp"

SdCardConfiguration::SdCardConfiguration ()
:
    _status(Configuration_MissingSdCard)
{
    // Initialise the SD card reader.
    if (SdCard::get().status() == SdCard::SdCard_OK)
    {
        _status = Configuration_OK;
    }
}

uint8_t const * SdCardConfiguration::get (uint8_t const * key) const
{
    String path = SdCard::get().fullPath((char const *)key);
    FILE * file = fopen(path(),"r");
    if (0 == file)
    {
        _status = Configuration_MissingValue;
        return 0;
    }
    static uint8_t buffer[0x100];
    size_t bytes = fread(&buffer,1,0x100,file);
    fclose(file);
    buffer[bytes] = '\0';
    return buffer;
}

SdCardConfiguration::Status SdCardConfiguration::status () const
{
    return _status;
}
