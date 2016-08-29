// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "catch.hpp"
#include "util.hpp"
#include "jsonparser.hpp"
#include "heapbytebuffer.hpp"

#define STREQUAL(x,y) std::string((char*)x) == std::string((char*)y)

TEST_CASE("json","")
{
    SECTION("parse current weather")
    {
        // Arrange
        std::string part1 = readFile(FIXTUREDIR "/current-part1.json");
        std::string part2 = readFile(FIXTUREDIR "/current-part2.json");
        std::string part3 = readFile(FIXTUREDIR "/current-part3.json");
        HeapByteBuffer buffer;
        buffer.add(copyBytes(part1),part1.size());
        buffer.add(copyBytes(part2),part2.size());
        buffer.add(copyBytes(part3),part3.size());
        using namespace json;
        // Act
        Json sut (buffer);
        // Assert
        REQUIRE(STREQUAL( sut.lookup("/name"), "Copenhagen" ));
        REQUIRE(STREQUAL( sut.lookup("/main/temp"), "285.086" ));
        REQUIRE(STREQUAL( sut.lookup("/clouds/all"), "68" ));
        REQUIRE(STREQUAL( sut.lookup("/main/humidity"), "72" ));
        REQUIRE(STREQUAL( sut.lookup("/main/pressure"), "1019.66" ));
        REQUIRE(STREQUAL( sut.lookup("/wind/speed"), "9.85" ));
        REQUIRE(STREQUAL( sut.lookup("/wind/deg"), "317.502" ));
        REQUIRE(STREQUAL( sut.lookup("/dt"), "1463393862" ));
        REQUIRE( sut.lookup("/notexist") == 0 );
    }
    SECTION("parse forecast")
    {
        // Arrange
        std::string part1 = readFile(FIXTUREDIR "/forecast-part1.json");
        std::string part2 = readFile(FIXTUREDIR "/forecast-part2.json");
        std::string part3 = readFile(FIXTUREDIR "/forecast-part3.json");
        HeapByteBuffer buffer;
        buffer.add(copyBytes(part1),part1.size());
        buffer.add(copyBytes(part2),part2.size());
        buffer.add(copyBytes(part3),part3.size());
        using namespace json;
        // Act
        Json json (buffer);
        // Assert
        REQUIRE(STREQUAL( json.lookup("/city/name"), "London" ));
        REQUIRE( json.lookupArraySize("/list") == 37 );
        REQUIRE(STREQUAL( json.lookup("/list/1/main/humidity"), "59" ));
    }
}
