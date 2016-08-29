// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "staticconfiguration.hpp"
#include <string.h>

StaticConfiguration::StaticConfiguration (std::vector<uint8_t const *> const & keyValuePairs_)
:
    _status(Configuration_OK),
    keyValuePairs(keyValuePairs_)
{
    std::vector<uint8_t const *>::const_iterator i = keyValuePairs.begin();
    while (i != keyValuePairs.end())
    {
        if (isInvalidKey(*i))
        {
            _status = Configuration_InvalidKey;
            return;
        }
        ++i;
        if (i == keyValuePairs.end())
        {
            // No value for key.
            _status = Configuration_MissingValue;
            return;
        }
        ++i;
    }
}

StaticConfiguration::Status StaticConfiguration::status () const
{
    return _status;
}

size_t StaticConfiguration::entries () const
{
    if (_status != Configuration_OK) return 0;
    return keyValuePairs.size() / 2;
}

uint8_t const * StaticConfiguration::get (uint8_t const * key) const
{
    if (_status != Configuration_OK) return 0;
    for (size_t i = 0; i < keyValuePairs.size(); ++i)
    {
        if (strcmp((char const *)keyValuePairs[i],(char const *)key) == 0)
            return keyValuePairs[i+1];
        ++i;
    }
    return 0;
}
