#include "catch2/catch.hpp"
#include "DataParser.h"

void checkThrowsParseException(const char* filename, const char* exceptionMessage)
{
    DataParser dataParser;

    try {
        dataParser.Parse(filename);

        //this line will only be reached if Parse does not throw an exception
        REQUIRE(false);
    }
    catch (DataParser::ParseException& e) {
        REQUIRE(std::string(e.what()) == exceptionMessage);
    }
}

TEST_CASE("Invalid JSON's throw exceptions")
{
    SECTION("Invalid file throws exception")
    {
        checkThrowsParseException("invalid.json", "Error opening file.");
    }

    SECTION("Root element that is not an array throws exception")
    {
        checkThrowsParseException("tests/testData/test1.json", "Root element is not an array.");
    }

    SECTION("Course that is not an array throws exception")
    {
        checkThrowsParseException("tests/testData/test2.json", "Course 0: Course is not an array.");
    }

    SECTION("Section that is not an array throws exception")
    {
        checkThrowsParseException("tests/testData/test3.json", "Course 0: Section 0: Section is not an array.");
    }

    SECTION("Timeslot that is not an array throws exception")
    {
        checkThrowsParseException("tests/testData/test4.json", "Course 0: Section 0: Timeslot 0: Timeslot is not an object.");
    }

    SECTION("Timeslot that does not have member 'startTime' throws exception")
    {
        checkThrowsParseException("tests/testData/test5.json", "Course 0: Section 0: Timeslot 0: Timeslot does not have member 'startTime'.");
    }

    SECTION("Timeslot whose has member 'startTime' that is not a string throws exception")
    {
        checkThrowsParseException("tests/testData/test6.json", "Course 0: Section 0: Timeslot 0: Timeslot.startTime is not a string.");
    }

    SECTION("Timeslot that does not have member 'endTime' throws exception")
    {
        checkThrowsParseException("tests/testData/test7.json", "Course 0: Section 0: Timeslot 0: Timeslot does not have member 'endTime'.");
    }

    SECTION("Timeslot whose has member 'endTime' that is not a string throws exception")
    {
        checkThrowsParseException("tests/testData/test8.json", "Course 0: Section 0: Timeslot 0: Timeslot.endTime is not a string.");
    }

    SECTION("Timeslot that does not have member 'days' throws exception")
    {
        checkThrowsParseException("tests/testData/test9.json", "Course 0: Section 0: Timeslot 0: Timeslot does not have member 'days'.");
    }

    SECTION("Timeslot whose has member 'days' that is not an array throws exception")
    {
        checkThrowsParseException("tests/testData/test10.json", "Course 0: Section 0: Timeslot 0: Timeslot.days is not an array.");
    }

    SECTION("Timeslot whose has member 'days' that is not an array throws exception")
    {
        checkThrowsParseException("tests/testData/test11.json", "Course 0: Section 0: Timeslot 0: Day is not a string.");
    }

    SECTION("Timeslot whose has member 'days' that is not an array throws exception")
    {
        checkThrowsParseException("tests/testData/test12.json", "Course 0: Section 0: Timeslot 0: Invalid day string.");
    }
}

// TEST_CASE("Valid JSON produces correct objects")
// {
// }
