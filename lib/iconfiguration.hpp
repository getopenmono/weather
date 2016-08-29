// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_iconfiguration_h
#define __com_openmono_iconfiguration_h
#include <stdint.h>
#include <stddef.h>

#define CONF_KEY_ROOT "/mono"

/**
 * IConfiguration is an abstract key-value store.
 */
struct IConfiguration
{
    /**
     * Check validity of a key.  A key is valid if it is a rooted path using
     * only forward slashes, dashes, underscores, dots and alpha-numeric characters.
     * @return     true if the key is invalid
     */
    static bool isInvalidKey (uint8_t const * key);

    virtual ~IConfiguration ();

    /**
     * Retrieve the value for a specific key.
     * @param  key key to retrieve
     * @return pointer to the value, possibly in temporary storage, or 0 if not found
     */
    virtual uint8_t const * get (uint8_t const * key) const = 0;
};

#endif // __com_openmono_iconfiguration_h
