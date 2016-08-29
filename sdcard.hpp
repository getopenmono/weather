// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_sdcard_h
#define __com_openmono_sdcard_h
#include <SDFileSystem.h>
#include <mono.h>

using mono::String;

namespace mono { namespace io {

    /**
     * This is a encapsulating class for the default mbed SDFileSystem class
     * It exposes the `initialize_card` method, to enable support for power cycles.
     */
    class SDFileSystem : public ::SDFileSystem {
    public:

        SDFileSystem(PinName mosi, PinName miso, PinName clk, PinName cs, const char *path) :
            ::SDFileSystem(mosi,miso,clk,cs,path)
        {}

        void initialize_card() { this->initialise_card(); }
    };

}}

/**
 * Singleton that must be used by all classes that want to use the SD card reader.
 */
class SdCard : mono::power::IPowerAware
{
public:
    enum Status
    {
        SdCard_OK, // 0
        SdCard_MissingSdCard // 1
    };

    /**
     * Access the CD card reader.
     * @return SD Card singleton.
     */
    static SdCard & get ();

    /**
     * Constructs a path that can be used to open a file on the SD card.
     * @param  path rooted path.
     * @return      rooted path prefixed with the mount point for the SD card file system.
     */
    String fullPath (String const & path);

    /**
     * Create all directories in a path.
     * @param  fullPath full path as returned by fullPath().
     * @return          SdCard_OK on success.
     */
    Status mkdirForFullPath (String const & fullPath);

    /**
     * @return operational status of the SD card.
     */
    Status status () const;


    // MARK: Power Aware Interface

    void onSystemPowerOnReset();
    void onSystemEnterSleep();
    void onSystemWakeFromSleep();
    void OnSystemBatteryLow();

private:
    SdCard ();
    Status _status;
    mono::io::SDFileSystem fs;
};

#endif // __com_openmono_sdcard_h
