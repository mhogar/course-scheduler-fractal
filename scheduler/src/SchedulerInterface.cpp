#include "SchedulerInterface.h"
#include "Scheduler.h"
#include "DataParser.h"
#include <iostream>

void SchedulerInterface::CreateSchedules(const char* filename, bool ignoreClosed)
{
    //parse the courses from the input json file
    std::vector<Scheduler::Course> courses = DataParser::Instance()->Parse(filename, ignoreClosed);

    //create the schedules
    Scheduler scheduler;
    scheduler.GenerateSchedules(courses);
    std::vector<Scheduler::Schedule> schedules = scheduler.GetSchedules();
}
