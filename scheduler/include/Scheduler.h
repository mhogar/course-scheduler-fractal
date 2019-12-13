#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cstdint>
#include <vector>

class Scheduler
{
public:
    struct Section;
    struct Timeslot;

    struct Course
    {
        uint32_t Id;
        std::vector<Section> Sections;
    };

    struct Section
    {
        uint32_t Id;
        Course* Parent;
        std::vector<Timeslot> Timeslots;

        bool HasConflict(const Section& other) const;
    };

    struct Timeslot
    {
        enum DayEnum
        {
            NONE = 0,
            MON = 0x1,
            TUES = 0x2,
            WED = 0x4,
            THUR = 0x8,
            FRI = 0x10
        };

        uint32_t Id;
        Section* Parent;
        uint16_t StartTime = 0;
        uint16_t EndTime = 0;
        uint8_t Days = 0;

        bool HasConflict(const Timeslot& other) const;
    };

    struct Schedule
    {
        std::vector<const Section*> Sections;

        bool DoesSectionFit(const Section& section) const;
    };

    Scheduler();
    ~Scheduler();

    std::vector<Schedule> CalculateSchedules(const std::vector<Course>& courses);

private:
    std::vector<Schedule> mSchedules;
    Schedule* mCurrSchedule;

    void CreateNewSchedule();

    void CalculateSchedulesInternal(std::vector<Scheduler::Course>::const_iterator courseItr,
        const std::vector<Scheduler::Course>::const_iterator endCourse);  
};

#endif
