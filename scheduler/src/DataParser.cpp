#include "DataParser.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <cstdio>
#include <iostream>

using namespace rapidjson;

DataParser::DataParser()
{
}

DataParser::~DataParser()
{
}

void DataParser::Validate(bool result, std::string message)
{
    if (!result) {
        throw DataParser::ParseException(message);
    }
}

uint16_t DataParser::ParseTime(const char* timeStr)
{
    return 0;
}

std::string DataParser::CreateParseErrorMessage(const Scheduler::Course& course, const char* message)
{
    return "Course " + std::to_string(course.Id) + ": " + message;
}

std::string DataParser::CreateParseErrorMessage(const Scheduler::Section& section, const char* message)
{
    return CreateParseErrorMessage(*section.Parent) + "Section " + std::to_string(section.Id) + ": " + message;
}

std::string DataParser::CreateParseErrorMessage(const Scheduler::Timeslot& timeslot, const char* message)
{
    return CreateParseErrorMessage(*timeslot.Parent) + "Timeslot " + std::to_string(timeslot.Id) + ": " + message;
}

std::vector<Scheduler::Course> DataParser::Parse(const char* filename, bool ignoreClosed)
{
    //parse the json from the file
    Document doc;
    {
        FILE* fPtr = fopen(filename, "r");
        Validate(fPtr, "Error opening file.");

        char buffer[65536];
        FileReadStream stream(fPtr, buffer, sizeof(buffer));
        doc.ParseStream(stream);

        fclose(fPtr);
    }

    Validate(doc.IsArray(), "Root element is not an array.");
    auto coursesArray = doc.GetArray();

    //init courses vector and set size
    std::vector<Scheduler::Course> courses;
    courses.reserve(coursesArray.Size());

    for (auto& courseElement : coursesArray)
    {
        //create course
        courses.push_back(Scheduler::Course());
        Scheduler::Course& course = courses.back();

        //init the course
        course.Id = courses.size() - 1;

        Validate(courseElement.IsArray(), CreateParseErrorMessage(course, "Course is not an array."));
        auto sectionsArray = courseElement.GetArray();

        //init the sections vector and set size
        auto& sections = course.Sections;
        sections.reserve(sectionsArray.Size());

        for (auto& sectionElement : sectionsArray)
        {
            //create the section
            sections.push_back(Scheduler::Section());
            Scheduler::Section& section = sections.back();

            //init the section
            section.Id = sections.size() - 1;
            section.Parent = &course;
            
            Validate(sectionElement.IsArray(), CreateParseErrorMessage(section, "Section is not an array."));
            auto timeslotsArray = sectionElement.GetArray();

            //init timeslots vector and set size
            auto& timeslots = section.Timeslots;
            timeslots.reserve(timeslotsArray.Size());

            for (auto& timeslotElement : timeslotsArray)
            {
                //create the timeslot
                timeslots.push_back(Scheduler::Timeslot());
                Scheduler::Timeslot& timeslot = timeslots.back();

                //init the timeslot
                timeslot.Id = timeslots.size() - 1;
                timeslot.Parent = &section;

                Validate(timeslotElement.IsObject(), CreateParseErrorMessage(timeslot, "Scheduler::Timeslot is not an object."));
                auto timeslotObj = timeslotElement.GetObject();

                //get start time
                Validate(timeslotObj.HasMember("startTime"), CreateParseErrorMessage(timeslot, "Scheduler::Timeslot does not have member 'startTime'."));
                Validate(timeslotObj["startTime"].IsString(), CreateParseErrorMessage(timeslot, "Scheduler::Timeslot.startTime is not a string."));
                timeslot.StartTime = ParseTime(timeslotObj["startTime"].GetString());

                //get end time
                Validate(timeslotObj.HasMember("endTime"), CreateParseErrorMessage(timeslot, "Scheduler::Timeslot does not have member 'endTime'."));
                Validate(timeslotObj["endTime"].IsString(), CreateParseErrorMessage(timeslot, "Scheduler::Timeslot.endTime is not a string."));
                timeslot.EndTime = ParseTime(timeslotObj["endTime"].GetString());

                //get days
                Validate(timeslotObj.HasMember("days"), CreateParseErrorMessage(timeslot, "Scheduler::Timeslot does not have member 'days'."));
                Validate(timeslotObj["days"].IsArray(), CreateParseErrorMessage(timeslot, "Scheduler::Timeslot.days is not an array."));
                auto daysArray = timeslotObj["days"].GetArray();

                uint8_t& days = timeslot.Days;
                for (auto& dayElement : daysArray)
                {
                    Validate(dayElement.IsString(), CreateParseErrorMessage(timeslot, "Day is not a string."));
                    auto day = std::string(dayElement.GetString());

                    if (day == "Mon") {
                        days |= Scheduler::Timeslot::DayEnum::MON;
                    }
                    else if (day == "Tues") {
                        days |= Scheduler::Timeslot::DayEnum::TUES;
                    }
                    else if (day == "Wed") {
                        days |= Scheduler::Timeslot::DayEnum::WED;
                    }
                    else if (day == "Thur") {
                        days |= Scheduler::Timeslot::DayEnum::THUR;
                    }
                    else if (day == "Fri") {
                        days |= Scheduler::Timeslot::DayEnum::FRI;
                    }
                    else {
                        throw ParseException(CreateParseErrorMessage(timeslot, "Invalid day string."));
                    }
                }
            }
        }
    }

    return courses;
}
