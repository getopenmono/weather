// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "catch.hpp"
#include "util.hpp"
#include "bytestring.hpp"

#define STREQUAL(x,y) std::string((char*)x) == std::string((char*)y)

TEST_CASE("bytestring","")
{
    SECTION("empty construction")
    {
        // Act
        ByteString sut;
        // Assert
        REQUIRE( sut == "" );
        REQUIRE( sut != (char const *)0 );
    }
    SECTION("empty assignment")
    {
        // Act
        ByteString sut = "";
        // Assert
        REQUIRE( sut == "" );
    }
    SECTION("real string")
    {
        // Arrange
        char const * fixed = "æøå";
        // Act
        ByteString sut(fixed);
        // Assert
        REQUIRE( sut == fixed );
        REQUIRE( sut == "æøå" );
    }
    SECTION("reassingment")
    {
        // Arrange
        ByteString sut;
        char const * fixed = "æøå";
        // Act
        sut = fixed;
        // Assert
        REQUIRE( sut == fixed );
    }
    SECTION("conversion to char")
    {
        // Arrange
        char const * fixed = "hello char";
        // Act
        ByteString sut = fixed;
        // Assert
        REQUIRE( strcmp(sut.c_str(),fixed) == 0 );
    }
}
