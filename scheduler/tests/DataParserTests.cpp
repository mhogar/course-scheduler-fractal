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
}

// TEST_CASE("Valid JSON produces correct objects")
// {
// }
