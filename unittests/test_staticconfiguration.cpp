// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "catch.hpp"
#include "util.hpp"
#include "staticconfiguration.hpp"

#define STREQUAL(x,y) std::string((char*)x) == std::string((char*)y)

TEST_CASE("staticconfiguration","")
{
    SECTION("empty configuration")
    {
        // Arrange
        std::vector<uint8_t const *> kvs;
        // Act
        StaticConfiguration sut(kvs);
        // Assert
        REQUIRE( sut.status() == StaticConfiguration::Configuration_OK );
        REQUIRE( sut.entries() == 0 );
        REQUIRE( sut.get(castToBytes("/city")) == 0 );
    }
    SECTION("key/values must be in pairs")
    {
        // Arrange
        std::vector<uint8_t const *> kvs;
        kvs.push_back(castToBytes("/ssid"));
        kvs.push_back(castToBytes("rufus"));
        kvs.push_back(castToBytes("/city"));
        // Act
        StaticConfiguration sut(kvs);
        // Assert
        REQUIRE( sut.status() == StaticConfiguration::Configuration_MissingValue );
        REQUIRE( sut.entries() == 0 );
    }
    SECTION("keys must be non-empty")
    {
        // Arrange
        std::vector<uint8_t const *> kvs;
        kvs.push_back(castToBytes(""));
        kvs.push_back(castToBytes("rufus"));
        // Act
        StaticConfiguration sut(kvs);
        // Assert
        REQUIRE( sut.status() == StaticConfiguration::Configuration_InvalidKey );
        REQUIRE( sut.entries() == 0 );
    }
    SECTION("keys must be alpha-numeric ASCII path")
    {
        // Arrange
        std::vector<uint8_t const *> kvs;
        kvs.push_back(castToBytes("/kø.txt"));
        kvs.push_back(castToBytes("rufus"));
        // Act
        StaticConfiguration sut(kvs);
        // Assert
        REQUIRE( sut.status() == StaticConfiguration::Configuration_InvalidKey );
        REQUIRE( sut.entries() == 0 );
    }
    SECTION("keys must be rooted")
    {
        // Arrange
        std::vector<uint8_t const *> kvs;
        kvs.push_back(castToBytes("ssid"));
        kvs.push_back(castToBytes("rufus"));
        // Act
        StaticConfiguration sut(kvs);
        // Assert
        REQUIRE( sut.status() == StaticConfiguration::Configuration_InvalidKey );
        REQUIRE( sut.entries() == 0 );
    }
    SECTION("proper keys")
    {
        // Arrange
        std::vector<uint8_t const *> kvs;
        kvs.push_back(castToBytes("/ssid.txt"));
        kvs.push_back(castToBytes("rufus"));
        kvs.push_back(castToBytes("/in-app/my_city"));
        kvs.push_back(castToBytes("København"));
        // Act
        StaticConfiguration sut(kvs);
        // Assert
        REQUIRE( sut.status() == StaticConfiguration::Configuration_OK );
        REQUIRE( sut.entries() == 2 );
        REQUIRE(STREQUAL( sut.get(kvs[0]), "rufus" ));
        REQUIRE(STREQUAL( sut.get(kvs[2]), "København" ));
    }
}
