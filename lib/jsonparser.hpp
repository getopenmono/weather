// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#if !defined(__com_openmono_jsonparser_h)
#define __com_openmono_jsonparser_h

#include "ibytebuffer.hpp"
#include "SmallJSONParser.h"
#include "weather.hpp"

namespace json {

/**
 * Json lets you look up values in a JSON document by using traditional paths
 * strings.
 *
 * Examples:
 *
 *      lookup("/location/name")) == "Copenhagen"
 *
 *      lookupArraySize("/list") == 5
 *      lookup("/list/4/humidity") == "56"
 *
 * Arrays are indexed from 0.
 * Json must be provided as a stream of data in form of a ByteBuffer.
 */
class Json
{
public:
    Json (IByteBuffer const & byteBuffer);
    ~Json ();
    /**
     * Extract a value from the JSON document.
     * @param  path  rooted path to the JSON element.
     * @return       pointer to the value as string (possibly in temporary storage), or 0 if not found or not a simple value.
     */
    uint8_t const * lookup (char const * path);
    /**
     * Calculate the size of a JSON array element.
     * @param  path rooted path to the JSON element.
     * @return      size of array, or 0.
     */
    size_t lookupArraySize (char const * path);
private:
    #define MAX_KEYSIZE 64
    uint8_t valueBuffer[MAX_KEYSIZE];
    IByteBuffer const & byteBuffer;
    size_t nextChunkIndex;
    JSONParser parser;
    JSONProvider provider;
    void restart ();
    friend struct Searcher;
public:
    // Only for use by global callback C function.
    bool provideMoreInput ();
};

} // json

#endif // __com_openmono_jsonparser_h
