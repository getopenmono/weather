// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "sdcardbytebuffer.hpp"
#include <consoles.h>
#include "sdcard.hpp"

#define CHUNK_SIZE 0x100

SdCardByteBuffer::SdCardByteBuffer ()
:
    _status(SdBuffer_NotInitialised),
    bytesTotal(0)
{
}

SdCardByteBuffer::~SdCardByteBuffer ()
{
}

SdCardByteBuffer::Status SdCardByteBuffer::status () const
{
    return _status;
}

void SdCardByteBuffer::attach (mono::String const & fileName)
{
    path = SdCard::get().fullPath(fileName);
    printf(String::Format("Opening %s\r\n",path())());
    FILE * file = fopen(path(),"r");
    if (0 == file)
    {
        printf(String::Format("Could not open %s\r\n",path())());
        return clear();
    }
    fseek(file,0,SEEK_END);
    bytesTotal = ftell(file);
    printf("Bytes in file: %d\r\n",bytesTotal);
    fclose(file);
    _status = SdBuffer_OK;
}

void SdCardByteBuffer::addString (mono::String const & string)
{
    add((uint8_t const *)string.stringData,string.Length());
}

void SdCardByteBuffer::add (uint8_t const * data, size_t length)
{
    FILE * file = fopen(path(),"a");
    if (0 == file)
    {
        _status = SdBuffer_FileNotWritable;
        return;
    }
    size_t written = fwrite(data,1,length,file);
    if (written != length)
    {
        _status = SdBuffer_FileSystemFull;
        fclose(file);
        return;
    }
    bytesTotal += written;
    fclose(file);
}

size_t SdCardByteBuffer::bytes () const
{
    return bytesTotal;
}

size_t SdCardByteBuffer::chunks () const
{
    size_t count = bytes();
    size_t chunks = count / CHUNK_SIZE;
    if (chunks * CHUNK_SIZE < count) return chunks + 1;
    return chunks;
}

size_t SdCardByteBuffer::chunkBytes (size_t index) const
{
    size_t count = chunks();
    if (index < (count-1)) return CHUNK_SIZE;
    if (index >= count) return 0;
    return bytes() % CHUNK_SIZE;
}

uint8_t SdCardByteBuffer::operator[] (size_t index) const
{
    uint8_t buffer = 0;
    FILE * file = fopen(path(),"r");
    if (0 == file)
    {
        _status = SdBuffer_FileNotReadable;
        return 0;
    }
    if (fseek(file,index,SEEK_SET) != 0)
    {
        fclose(file);
        return 0;
    }
    fread(&buffer,1,1,file);
    fclose(file);
    return buffer;
}

uint8_t const * const SdCardByteBuffer::chunk (size_t index) const
{
    static uint8_t buffer[CHUNK_SIZE];
    FILE * file = fopen(path(),"r");
    if (0 == file)
    {
        _status = SdBuffer_FileNotReadable;
        return 0;
    }
    if (fseek(file,index*CHUNK_SIZE,SEEK_SET) != 0)
    {
        fclose(file);
        return 0;
    }
    fread(&buffer,1,CHUNK_SIZE,file);
    fclose(file);
    return buffer;
}

void SdCardByteBuffer::clear ()
{
    bytesTotal = 0;
    SdCard::Status sdStatus = SdCard::get().mkdirForFullPath(path);
    if (sdStatus != SdCard::SdCard_OK)
    {
        _status = SdBuffer_FileNotWritable;
        return;
    }
    FILE * file = fopen(path(),"w");
    if (0 == file)
    {
        _status = SdBuffer_FileNotWritable;
        return;
    }
    _status = SdBuffer_OK;
    fclose(file);
}
