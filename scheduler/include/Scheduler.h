#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>

class Scheduler
{
public:
    struct Section;
    struct Timeslot;

    struct Course
    {
        size_t Id;
        std::vector<Section> Sections;
    };

    struct Section
    {
        size_t Id;
        Course* Parent;
        std::vector<Timeslot> Timeslots;

        bool HasConflict(const Section& other) const;
    };

    struct Timeslot
    {
        size_t Id;
        Section* Parent;
        uint16_t StartTime = 0;
        uint16_t EndTime = 0;
        uint8_t Days = 0;

        bool HasConflict(const Timeslot& other) const;
    };

    Scheduler();
    ~Scheduler();
};

#endif
