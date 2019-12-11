#include "DataParser.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <cstdio>
#include <iostream>

using namespace rapidjson;

DataParser* DataParser::mInstance = nullptr;

DataParser* DataParser::Instance()
{
    if (mInstance == nullptr) {
        mInstance = new DataParser();
    }

    return mInstance;
}

DataParser::DataParser()
{
}

DataParser::~DataParser()
{
}

Scheduler::Timeslot::DayEnum DataParser::ParseDay(const char* day)
{
    std::string dayStr = day;

    if (dayStr == "Mon") {
        return Scheduler::Timeslot::DayEnum::MON;
    }

    if (dayStr == "Tues") {
        return Scheduler::Timeslot::DayEnum::TUES;
    }

    if (dayStr == "Wed") {
        return Scheduler::Timeslot::DayEnum::WED;
    }

    if (dayStr == "Thur") {
        return Scheduler::Timeslot::DayEnum::THUR;
    }

    if (dayStr == "Fri") {
        return Scheduler::Timeslot::DayEnum::FRI;
    }

    return Scheduler::Timeslot::DayEnum::NONE;
}

void DataParser::Validate(bool result, std::string message)
{
    if (!result) {
        throw DataParser::ParseException(message);
    }
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

                Validate(timeslotElement.IsObject(), CreateParseErrorMessage(timeslot, "Timeslot is not an object."));
                auto timeslotObj = timeslotElement.GetObject();

                //get start time
                Validate(timeslotObj.HasMember("startTime"), CreateParseErrorMessage(timeslot, "Timeslot does not have member 'startTime'."));
                Validate(timeslotObj["startTime"].IsInt(), CreateParseErrorMessage(timeslot, "Timeslot.startTime is not an integer."));
                timeslot.StartTime = timeslotObj["startTime"].GetInt();

                //get end time
                Validate(timeslotObj.HasMember("endTime"), CreateParseErrorMessage(timeslot, "Timeslot does not have member 'endTime'."));
                Validate(timeslotObj["endTime"].IsInt(), CreateParseErrorMessage(timeslot, "Timeslot.endTime is not an integer."));
                timeslot.EndTime = timeslotObj["endTime"].GetInt();

                //get days
                Validate(timeslotObj.HasMember("days"), CreateParseErrorMessage(timeslot, "Timeslot does not have member 'days'."));
                Validate(timeslotObj["days"].IsArray(), CreateParseErrorMessage(timeslot, "Timeslot.days is not an array."));
                auto daysArray = timeslotObj["days"].GetArray();

                uint8_t& days = timeslot.Days;
                for (auto& dayElement : daysArray)
                {
                    Validate(dayElement.IsString(), CreateParseErrorMessage(timeslot, "Day is not a string."));
                    auto day = ParseDay(dayElement.GetString());

                    if (day == Scheduler::Timeslot::DayEnum::NONE) { 
                        throw ParseException(CreateParseErrorMessage(timeslot, "Invalid day string."));
                    }
                    else {
                        days |= day;
                    }
                }
            }
        }
    }

    return courses;
}
