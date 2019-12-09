#include "SchedulerInterface.h"
#include "Scheduler.h"
#include "DataParser.h"
#include <iostream>

void SchedulerInterface::CreateSchedules(const char* filename, bool ignoreClosed)
{
    std::vector<Scheduler::Course> courses = DataParser::Instance()->Parse(filename, ignoreClosed);

    std::cout << "Num Courses: " << courses.size() << std::endl;
    std::cout << "Num Sections: " << courses[0].Sections.size() << std::endl;
    std::cout << "Num Timeslots: " << courses[0].Sections[0].Timeslots.size() << std::endl;
}
