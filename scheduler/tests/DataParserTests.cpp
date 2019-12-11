#include "catch2/catch.hpp"
#include "DataParser.h"

TEST_CASE("Parsing invalid JSONs throw exceptions")
{
    SECTION("Invalid file throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("invalid.json"), 
            DataParser::ParseException, Catch::Message("Error opening file."));
    }

    SECTION("Root element that is not an array throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test1.json"), 
            DataParser::ParseException, Catch::Message("Root element is not an array."));
    }

    SECTION("Course that is not an array throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test2.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Course is not an array."));
    }

    SECTION("Section that is not an array throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test3.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Section is not an array."));
    }

    SECTION("Timeslot that is not an array throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test4.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Timeslot 0: Timeslot is not an object."));
    }

    SECTION("Timeslot that does not have member 'startTime' throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test5.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Timeslot 0: Timeslot does not have member 'startTime'."));
    }

    SECTION("Timeslot whose has member 'startTime' that is not an integer throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test6.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Timeslot 0: Timeslot.startTime is not an integer."));
    }

    SECTION("Timeslot that does not have member 'endTime' throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test7.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Timeslot 0: Timeslot does not have member 'endTime'."));
    }

    SECTION("Timeslot whose has member 'endTime' that is not an integer throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test8.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Timeslot 0: Timeslot.endTime is not an integer."));
    }

    SECTION("Timeslot that does not have member 'days' throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test9.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Timeslot 0: Timeslot does not have member 'days'."));
    }

    SECTION("Timeslot whose has member 'days' that is not an array throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test10.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Timeslot 0: Timeslot.days is not an array."));
    }

    SECTION("Timeslot whose has member 'days' that is not an array throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test11.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Timeslot 0: Day is not a string."));
    }

    SECTION("Timeslot whose has member 'days' that is not an array throws exception")
    {
        REQUIRE_THROWS_MATCHES(DataParser::Instance()->Parse("tests/testData/test12.json"), 
            DataParser::ParseException, Catch::Message("Course 0: Section 0: Timeslot 0: Invalid day string."));
    }
}

TEST_CASE("ParseDay works")
{
    SECTION("Can parse Monday")
    {
        REQUIRE(DataParser::Instance()->ParseDay("Mon") == Scheduler::Timeslot::DayEnum::MON);
    }

    SECTION("Can parse Tuesday")
    {
        REQUIRE(DataParser::Instance()->ParseDay("Tues") == Scheduler::Timeslot::DayEnum::TUES);
    }

    SECTION("Can parse Wednesday")
    {
        REQUIRE(DataParser::Instance()->ParseDay("Wed") == Scheduler::Timeslot::DayEnum::WED);
    }

    SECTION("Can parse Thursday")
    {
        REQUIRE(DataParser::Instance()->ParseDay("Thur") == Scheduler::Timeslot::DayEnum::THUR);
    }

    SECTION("Can parse Friday")
    {
        REQUIRE(DataParser::Instance()->ParseDay("Fri") == Scheduler::Timeslot::DayEnum::FRI);
    }

    SECTION("Invalid day string returns 'NONE'")
    {
        REQUIRE(DataParser::Instance()->ParseDay("invalid") == Scheduler::Timeslot::DayEnum::NONE);
    }
}

TEST_CASE("Valid JSON produces correct objects")
{
    std::vector<Scheduler::Course> courses = DataParser::Instance()->Parse("tests/testData/test13.json");
    REQUIRE(courses.size() == 2);

    SECTION("Course 1 is correct")
    {
        Scheduler::Course& course = courses[0];
        REQUIRE(course.Id == 0);
        REQUIRE(course.Sections.size() == 2);

        SECTION("Section 1 is correct")
        {
            Scheduler::Section& section = course.Sections[0];
            REQUIRE(section.Id == 0);
            REQUIRE(section.Parent == &course);
            REQUIRE(section.Timeslots.size() == 2);

            SECTION("Timeslot 1 is correct")
            {
                Scheduler::Timeslot& timeslot = section.Timeslots[0];
                REQUIRE(timeslot.Id == 0);
                REQUIRE(timeslot.Parent == &section);
                REQUIRE(timeslot.StartTime == 100);
                REQUIRE(timeslot.EndTime == 200);
                REQUIRE((timeslot.Days & timeslot.MON) != 0);
                REQUIRE((timeslot.Days & timeslot.WED) != 0);
                REQUIRE((timeslot.Days & timeslot.FRI) != 0);
            }

            SECTION("Timeslot 2 is correct")
            {
                Scheduler::Timeslot& timeslot = section.Timeslots[1];
                REQUIRE(timeslot.Id == 1);
                REQUIRE(timeslot.Parent == &section);
                REQUIRE(timeslot.StartTime == 300);
                REQUIRE(timeslot.EndTime == 400);
                REQUIRE((timeslot.Days & timeslot.THUR) != 0);
            }
        }

        SECTION("Section 2 is correct")
        {
            Scheduler::Section& section = course.Sections[1];
            REQUIRE(section.Id == 1);
            REQUIRE(section.Parent == &course);
            REQUIRE(section.Timeslots.size() == 1);

            SECTION("Timeslot 1 is correct")
            {
                Scheduler::Timeslot& timeslot = section.Timeslots[0];
                REQUIRE(timeslot.Id == 0);
                REQUIRE(timeslot.Parent == &section);
                REQUIRE(timeslot.StartTime == 500);
                REQUIRE(timeslot.EndTime == 600);
                REQUIRE((timeslot.Days & timeslot.MON) != 0);
            }
        }
    }

    SECTION("Course 2 is correct")
    {
        Scheduler::Course& course = courses[1];
        REQUIRE(course.Id == 1);
        REQUIRE(course.Sections.size() == 1);

        SECTION("Section 1 is correct")
        {
            Scheduler::Section& section = course.Sections[0];
            REQUIRE(section.Id == 0);
            REQUIRE(section.Parent == &course);
            REQUIRE(section.Timeslots.size() == 1);

            SECTION("Timeslot 1 is correct")
            {
                Scheduler::Timeslot& timeslot = section.Timeslots[0];
                REQUIRE(timeslot.Id == 0);
                REQUIRE(timeslot.Parent == &section);
                REQUIRE(timeslot.StartTime == 700);
                REQUIRE(timeslot.EndTime == 800);
                REQUIRE((timeslot.Days & timeslot.TUES) != 0);
            }
        }
    }
}
