#include "Scheduler.h"

bool Scheduler::Timeslot::HasConflict(const Scheduler::Timeslot& other) const
{
    //first check days
    if ((this->Days & other.Days) == 0) {
        return false;
    }

    //check times for overlap
    if (this->StartTime < other.EndTime && this->EndTime > other.StartTime) {
        return true;
    }

    return false;
}

bool Scheduler::Section::HasConflict(const Scheduler::Section& other) const
{
    //check all this section's timeslots against all of the other section's timeslots for conflicts
    for (const Timeslot& timeslot1 : this->Timeslots)
    {
        for (const Timeslot& timeslot2 : other.Timeslots)
        {
            if (timeslot1.HasConflict(timeslot2)) {
                return true;
            }
        }
    }

    return false;
}

bool Scheduler::Schedule::DoesSectionFit(const Section& section) const
{
    //check the section against this schedule's sections for conflicts
    for (const Section* currSection : this->Sections)
    {
        if (currSection->HasConflict(section)) {
            return false;
        }
    }

    return true;
}

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
}

std::vector<Scheduler::Schedule> Scheduler::CalculateSchedules(const std::vector<Scheduler::Course>& courses)
{
    //reset the schedules
    mSchedules.clear();
    CreateNewSchedule();

    CalculateSchedulesInternal(courses.begin(), courses.end());

    return mSchedules;
}

void Scheduler::CreateNewSchedule()
{
    mSchedules.push_back(Schedule());
    mCurrSchedule = &mSchedules.back();
}

void Scheduler::CalculateSchedulesInternal(std::vector<Scheduler::Course>::const_iterator currentCourse, 
    const std::vector<Scheduler::Course>::const_iterator endCourse)
{
    //base case
    if (currentCourse == endCourse)
    {
        //cache the old schedule ptr and create a new schedule
        Schedule* oldSchedule = mCurrSchedule;
        CreateNewSchedule();

        //copy the contents of the old schedule to the new one
        mCurrSchedule->Sections.reserve(oldSchedule->Sections.size());
        for (const Section* section : oldSchedule->Sections) {
            mCurrSchedule->Sections.push_back(section);
        }

        return;
    }

    //create a new branch for every section that fits
    for (const Section& section : currentCourse->Sections)
    {
        if (mCurrSchedule->DoesSectionFit(section))
        {
            mCurrSchedule->Sections.push_back(&section);
            ++currentCourse;
            CalculateSchedulesInternal(currentCourse, endCourse);
            --currentCourse;
            mCurrSchedule->Sections.pop_back();
        }
    }
}
