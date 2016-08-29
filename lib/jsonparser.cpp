// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "jsonparser.hpp"
#include "SmallJSONParser.h"
#include "weather.hpp"
#include <cstdlib>

//#define DEBUG
#if defined(DEBUG)
#include <iostream>
#endif

namespace {

/**
 * Functions passed to SmallJSONParser API.
 */
extern "C" bool inputProvider (JSONParser * parser, void * context)
{
    json::Json * owner = (json::Json *)context;
    return owner->provideMoreInput();
}

} // namespace {

namespace json {

struct Searcher
{
    Json & parent;
    char const * path;
    size_t pathIndex;
    char key[MAX_KEYSIZE];
    Searcher (char const * path_, Json & parent_)
    :
        parent(parent_),
        path(path_),
        pathIndex(0)
    {
        parent.restart();
    }
    JSONToken lookupValue ()
    {
        JSONToken token;
        while (! isAtEndOfPath())
        {
            nextKey();
#           if defined(DEBUG)
            std::cout << "looking for key " << key << std::endl;
#           endif
            token = NextJSONTokenWithProvider(&parent.parser,&parent.provider);
            if (JSONTokenType(token) == StartArrayJSONToken)
            {
                size_t i = atoi(key);
                while (i > 0)
                {
                    --i;
                    if (! SkipJSONValueWithProvider(&parent.parser,&parent.provider)) break;
                }
            }
            else if (!SkipUntilJSONObjectKeyWithProvider(&parent.parser,&parent.provider,key))
            {
#               if defined(DEBUG)
                std::cout << "not found" << std::endl;
#               endif
                token.typeandflags = ParseErrorJSONToken;
                return token;
                break;
            }
        }
        token = NextJSONTokenWithProvider(&parent.parser,&parent.provider);
#       if defined(DEBUG)
        std::cout << JSONTokenType(token) << std::endl;
#       endif
        uint8_t buf[SizeOfUnescapingBufferForJSONStringToken(token)];
        UnescapeJSONStringToken(token,buf,0);
#       if defined(DEBUG)
        //std::cout << buf << std::endl;
#       endif
        return token;
    }
    bool isAtEndOfPath () const
    {
        return ('\0' == path[pathIndex]);
    }
    char const * nextKey ()
    {
        if (isAtEndOfPath()) return 0;
        if (path[pathIndex] != '/') return 0;
        size_t i = 0;
        for (++pathIndex; path[pathIndex] != '/' && !isAtEndOfPath(); ++i, ++pathIndex)
        {
            key[i] = path[pathIndex];
        }
        key[i] = '\0';
        return key;
    }
};

Json::Json (IByteBuffer const & byteBuffer_)
:
    byteBuffer(byteBuffer_),
    nextChunkIndex(0)
{
}

Json::~Json ()
{
}

uint8_t const * Json::lookup (char const * path)
{
#   if defined(DEBUG)
    std::cout << "path " << path << std::endl;
#   endif
    Searcher state(path,*this);
    JSONToken token = state.lookupValue();
    int type = JSONTokenType(token);
    if
    (
        type != StringJSONToken && type != NumberJSONToken && type != TrueJSONToken &&
        type != FalseJSONToken && type != NullJSONToken
    ){
        return 0;
    }
#   if defined(DEBUG)
    std::cout << "  buffer size " << SizeOfUnescapingBufferForJSONStringToken(token) << std::endl;
#   endif
    uint8_t * buf = new uint8_t[SizeOfUnescapingBufferForJSONStringToken(token)];
    UnescapeJSONStringToken(token,buf,0);
    return buf;
}

size_t Json::lookupArraySize (char const * path)
{
    Searcher state(path,*this);
    JSONToken token = state.lookupValue();
    if (JSONTokenType(token) != StartArrayJSONToken) return 0;
    size_t length = 0;
    while (JSONTokenType(token) != EndArrayJSONToken)
    {
        if (! SkipJSONValueWithProvider(&parser,&provider)) break;
        ++length;
    }
    return length;
}

void Json::restart ()
{
    nextChunkIndex = 0;
    InitialiseJSONParser(&parser);
    InitialiseJSONProvider(&provider,inputProvider,this,valueBuffer,sizeof(valueBuffer));
}

bool Json::provideMoreInput ()
{
#   if defined(DEBUG)
    std::cout << "(more input needed)" << std::endl;
#   endif
    if (nextChunkIndex >= byteBuffer.chunks())
    {
#      if defined(DEBUG)
        std::cout << "(no more input)" << std::endl;
#       endif
        return false;
    }
#   if defined(DEBUG)
    std::cout << std::string((char const *)byteBuffer.chunk(nextChunkIndex),byteBuffer.chunkBytes(nextChunkIndex)) << std::endl;
#   endif
    ProvideJSONInput(&parser,byteBuffer.chunk(nextChunkIndex),byteBuffer.chunkBytes(nextChunkIndex));
    ++nextChunkIndex;
    return true;
}

} // json
