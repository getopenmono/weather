// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#ifndef __com_openmono_sdcardbytebuffer_h
#define __com_openmono_sdcardbytebuffer_h
#include "lib/ibytebuffer.hpp"
#include <mono.h>

/**
 * SdCardByteBuffer can be used to collect data that arrives in chunks, such as
 * an HTTP response.  The data is maintained on the SD card, so there
 * is no memory overhead.
 */
class SdCardByteBuffer
:
    public IByteBuffer
{
public:
    enum Status
    {
        SdBuffer_NotInitialised,
        SdBuffer_OK,
        SdBuffer_FileNotWritable,
        SdBuffer_FileSystemFull,
        SdBuffer_FileNotReadable
    };
    SdCardByteBuffer ();
    virtual ~SdCardByteBuffer ();
    Status status () const;
    /**
     * Attach to a file on the SD card. The the file is created if needed.
     * The contents of the file is used as initial data.
     * @param fileName rooted or relative path to file on SD card.
     */
    void attach (mono::String const & fileName);
    void addString (mono::String const & string);
    virtual void add (uint8_t const * chunk, size_t length);
    virtual size_t bytes () const;
    virtual size_t chunks () const;
    virtual size_t chunkBytes (size_t index) const;
    virtual uint8_t operator[] (size_t position) const;
    virtual uint8_t const * const chunk (size_t index) const;
    virtual void clear ();
private:
    void mkdirs ();
    mutable Status _status;
    mono::String path;
    size_t bytesTotal;
};

#endif // __com_openmono_sdcardbytebuffer_h
