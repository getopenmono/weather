// This software is part of OpenMono, see http://developer.openmono.com
// Released under the MIT license, see LICENSE.txt
#include "catch.hpp"
#include "util.hpp"
#include "iso.hpp"

TEST_CASE("iso","")
{
    SECTION("improper timezone strings")
    {
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("Z") == -1 );
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("foo") == -1 );
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("0100") == -1 );
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("-01+23") == -1 );
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("01:234") == -1 );
    }
    SECTION("timezone")
    {
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("+01:00") == 60*60 );
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("-05:00") == -5*60*60 );
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("+11:45") == (11*60+45)*60 );
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("-0600") == -6*60*60 );
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("-1130") == -(11*60+30)*60 );
        REQUIRE( Iso::timeZoneToUnixTimeStampDiff("+0830") == (8*60+30)*60 );
    }
}
