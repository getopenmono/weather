// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_staticconfiguration_h
#define __com_openmono_staticconfiguration_h
#include "iconfiguration.hpp"
#include <vector>

/**
 * Configuration store initilised with a static list of key-value pairs.
 */
class StaticConfiguration
:
    public IConfiguration
{
public:
    enum Status
    {
        Configuration_OK,
        Configuration_InvalidKey,
        Configuration_MissingValue
    };

	/**
	 * @param keyValuePairs list of alternating key and value strings, expected
     *                      to live as long as the configuration
	 */
    StaticConfiguration (std::vector<uint8_t const *> const & keyValuePairs);

    /**
     * @return number of key-value pairs in the configuration
     */
    virtual size_t entries () const;

    virtual uint8_t const * get (uint8_t const * key) const;

    /**
     * @return operational status of the configuration store
     */
    Status status () const;

private:
    Status _status;
    std::vector<uint8_t const *> keyValuePairs;
};

#endif // __com_openmono_staticconfiguration_h
