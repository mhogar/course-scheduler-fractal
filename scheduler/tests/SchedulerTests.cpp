#include "catch2/catch.hpp"
#include "Scheduler.h"

TEST_CASE("Timeslot conflicts")
{
    Scheduler::Timeslot timeslot1;
    timeslot1.StartTime = 510; //8:30AM
    timeslot1.EndTime = 560; //9:20AM
    timeslot1.Days = Scheduler::Timeslot::DayEnum::MON | Scheduler::Timeslot::DayEnum::WED;

    Scheduler::Timeslot timeslot2;
    timeslot2.StartTime = 510; //8:30AM
    timeslot2.EndTime = 560; //9:20AM
    timeslot2.Days = Scheduler::Timeslot::DayEnum::WED | Scheduler::Timeslot::DayEnum::FRI;

    SECTION("At least one common day yields conflicts")
    {
        REQUIRE(timeslot1.HasConflict(timeslot2) == true);
        REQUIRE(timeslot2.HasConflict(timeslot1) == true);
    }

    SECTION("Different days yields no conflicts")
    {
        timeslot1.Days = Scheduler::Timeslot::DayEnum::MON;
        timeslot2.Days = Scheduler::Timeslot::DayEnum::FRI;

        REQUIRE(timeslot1.HasConflict(timeslot2) == false);
        REQUIRE(timeslot2.HasConflict(timeslot1) == false);
    }

    SECTION("No overlap yields conflicts")
    {
        timeslot2.StartTime = 570; //9:30AM
        timeslot2.EndTime = 620; //10:20AM

        REQUIRE(timeslot1.HasConflict(timeslot2) == false);
        REQUIRE(timeslot2.HasConflict(timeslot1) == false);
    }

    SECTION("Partial overlap yields conflicts")
    {
        timeslot2.EndTime = 590; //9:50AM

        REQUIRE(timeslot1.HasConflict(timeslot2) == true);
        REQUIRE(timeslot2.HasConflict(timeslot1) == true);
    }

    SECTION("Full overlap yields conflicts")
    {
        timeslot2.StartTime = 520; //8:50AM
        timeslot2.EndTime = 550; //9:10AM

        REQUIRE(timeslot1.HasConflict(timeslot2) == true);
        REQUIRE(timeslot2.HasConflict(timeslot1) == true);
    }
}
