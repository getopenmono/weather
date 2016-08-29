// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "catch.hpp"
#include "util.hpp"
#include "weather.hpp"
#include "openweathermap.hpp"
#include "heapbytebuffer.hpp"

#define STREQUAL(x,y) std::string((char*)x) == std::string((char*)y)

TEST_CASE("openweathermap","")
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
        using namespace openweathermap;
        using namespace weather;
        // Act
        Entry sut = parseCurrent(buffer);
        // Assert
        REQUIRE(STREQUAL( sut.city, "Copenhagen" ));
        REQUIRE(STREQUAL( sut.temperatureK, "285.086" ));
        REQUIRE(STREQUAL( sut.cloudPercentage, "68" ));
        REQUIRE(STREQUAL( sut.humidity, "72" ));
        REQUIRE(STREQUAL( sut.pressureHpa, "1019.66" ));
        REQUIRE(STREQUAL( sut.windSpeedMs, "9.85" ));
        REQUIRE(STREQUAL( sut.windDirection, "317.502" ));
        REQUIRE(STREQUAL( sut.timeUnix, "1463393862" ));
        REQUIRE( sut.condition == Day_Overcast );
        REQUIRE(STREQUAL( sut.rain, "0.05" ));
    }
    SECTION("parse forecast")
    {
        // Arrange
        std::string forecast = readFile(FIXTUREDIR "/forecast.json");
        HeapByteBuffer buffer;
        buffer.add(copyBytes(forecast),forecast.size());
        using namespace openweathermap;
        using namespace weather;
        // Act
        std::vector<Entry> forecasts = parseForecast(buffer,37);
        // Assert
        REQUIRE( forecasts.size() == 37 );
        {
            Entry const & entry = forecasts[0];
            REQUIRE(STREQUAL( entry.city, "London" ));
            REQUIRE(STREQUAL( entry.timeUnix, "1463400000" ));
            REQUIRE(STREQUAL( entry.temperatureK, "288.78" ));
            REQUIRE(STREQUAL( entry.cloudPercentage, "0" ));
            REQUIRE(STREQUAL( entry.humidity, "76" ));
            REQUIRE(STREQUAL( entry.pressureHpa, "1023.83" ));
            REQUIRE(STREQUAL( entry.windSpeedMs, "2.92" ));
            REQUIRE(STREQUAL( entry.windDirection, "307.51" ));
            REQUIRE( entry.condition == Day_ClearSky );
            REQUIRE(STREQUAL( entry.rain, "" ));
        }
        {
            Entry const & entry = forecasts[3];
            REQUIRE( entry.condition == Night_ScatteredClouds );
        }
        {
            Entry const & entry = forecasts[9];
            REQUIRE( entry.condition == Day_FewClouds );
        }
        {
            Entry const & entry = forecasts[11];
            REQUIRE( entry.condition == Night_Overcast );
        }
        {
            Entry const & entry = forecasts[12];
            REQUIRE( entry.condition == Night_Rain );
            REQUIRE(STREQUAL( entry.rain, "0.05" ));
        }
    }
}
