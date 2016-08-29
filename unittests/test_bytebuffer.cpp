// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "catch.hpp"
#include "util.hpp"
#include "heapbytebuffer.hpp"

TEST_CASE("heapbytebuffer","")
{
    SECTION("no input should give empty buffer")
    {
        // Arrange
        HeapByteBuffer sut;
        // Assert
        REQUIRE( sut.bytes() == 0 );
    }
    SECTION("no input should give null values for every index")
    {
        // Arrange
        HeapByteBuffer sut;
        // Assert
        REQUIRE( sut[0] == 0 );
        REQUIRE( sut[1000] == 0 );
    }
    SECTION("several parts should give complete buffer")
    {
        // Arrange
        HeapByteBuffer sut;
        std::string part1 = readFile(FIXTUREDIR "/current-part1.json");
        std::string part2 = readFile(FIXTUREDIR "/current-part2.json");
        std::string part3 = readFile(FIXTUREDIR "/current-part3.json");
        std::string complete = readFile(FIXTUREDIR "/current.json");
        // Act
        sut.add(castToBytes(part1),part1.size());
        sut.add(castToBytes(part2),part2.size());
        sut.add(castToBytes(part3),part3.size());
        // Assert
        uint32_t length = sut.bytes();
        REQUIRE( length == 672 );
        for (uint32_t i = 0; i < length; ++i)
        {
            REQUIRE( sut[i] == complete[i] );
        }
        REQUIRE( sut[10000] == 0 );
    }
    SECTION("clearing buffer restarts concatenation")
    {
        // Arrange
        HeapByteBuffer sut;
        sut.add(castToBytes("ABC"),3);
        uint8_t const * complete = (uint8_t const *)"1234567890.";
        // Act
        sut.clear();
        sut.add(castToBytes("1"),1);
        sut.add(castToBytes("23"),2);
        sut.add(castToBytes("456"),3);
        sut.add(castToBytes("7890"),4);
        sut.add(castToBytes("."),1);
        // Assert
        for (uint32_t i = 0; i < sut.bytes(); ++i)
        {
            REQUIRE( sut[i] == complete[i] );
        }
    }
    SECTION("chunks should be copied")
    {
        // Arrange
        HeapByteBuffer sut;
        uint8_t * chunk[3] = {castToBytes("1"),castToBytes("23"),castToBytes("456")};
        // Act
        sut.add(chunk[0],1);
        sut.add(chunk[1],2);
        sut.add(chunk[2],3);
        // Assert
        REQUIRE( sut.chunks() == 3 );
        for (size_t i = 0; i < sut.chunks(); ++i)
        {
            REQUIRE( sut.chunk(i) != chunk[i] );
            REQUIRE( sut.chunkBytes(i) == (i+1) );
        }
    }
}
