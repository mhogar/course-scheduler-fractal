#include "Scheduler.h"

Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
}

bool Scheduler::Timeslot::HasConflict(const Scheduler::Timeslot& other) const
{
    //first check days
    if ((this->Days & other.Days) == 0) {
        return false;
    }

    //TODO: check times
    return false;
}

bool Scheduler::Section::HasConflict(const Scheduler::Section& other) const
{
    //check all this sectionElement's timeslots against all of the other sectionElement's timeslots for conflicts
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
