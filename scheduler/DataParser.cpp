#include "DataParser.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <cstdio>
#include <iostream>

using namespace rapidjson;

bool DataParser::Timeslot::HasConflict(const DataParser::Timeslot& other) const
{
    //first check days
    if ((this->Days & other.Days) == 0) {
        return false;
    }

    //TODO: check times
    return false;
}

bool DataParser::Section::HasConflict(const DataParser::Section& other) const
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

std::string DataParser::CreateParseErrorMessage(const Course& course, const char* message)
{
    return "Course: " + std::to_string(course.Id) + ": " + message;
}

std::string DataParser::CreateParseErrorMessage(const Section& section, const char* message)
{
    return CreateParseErrorMessage(*section.Parent) + "Section: " + std::to_string(section.Id) + ": " + message;
}

std::string DataParser::CreateParseErrorMessage(const Timeslot& timeslot, const char* message)
{
    return CreateParseErrorMessage(*timeslot.Parent) + "Timeslot: " + std::to_string(timeslot.Id) + ": " + message;
}

std::vector<DataParser::Course> DataParser::Parse(const char* filename, bool ignoreClosed)
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
    std::vector<Course> courses;
    courses.reserve(coursesArray.Size());

    for (auto& courseElement : coursesArray)
    {
        //create course
        courses.push_back(Course());
        Course& course = courses.back();

        //init the course
        course.Id = courses.size();

        Validate(courseElement.IsArray(), CreateParseErrorMessage(course, "Course is not an array."));
        auto sectionsArray = courseElement.GetArray();

        //init the sections vector and set size
        auto& sections = course.Sections;
        sections.reserve(sectionsArray.Size());

        for (auto& sectionElement : sectionsArray)
        {
            //create the section
            sections.push_back(Section());
            Section& section = sections.back();

            //init the section
            section.Id = sections.size();
            section.Parent = &course;
            
            Validate(sectionElement.IsArray(), CreateParseErrorMessage(section, "Section is not an array."));
            auto timeslotsArray = sectionElement.GetArray();

            //init timeslots vector and set size
            auto& timeslots = section.Timeslots;
            timeslots.reserve(timeslotsArray.Size());

            for (auto& timeslotElement : timeslotsArray)
            {
                //create the timeslot
                timeslots.push_back(Timeslot());
                Timeslot& timeslot = timeslots.back();

                //init the timeslot
                timeslot.Id = timeslots.size();
                timeslot.Parent = &section;

                Validate(timeslotElement.IsObject(), CreateParseErrorMessage(timeslot, "Timeslot is not an object."));
                auto timeslotObj = timeslotElement.GetObject();

                //get start time
                Validate(timeslotObj.HasMember("startTime"), CreateParseErrorMessage(timeslot, "Timeslot does not have member 'startTime'."));
                Validate(timeslotObj["startTime"].IsString(), CreateParseErrorMessage(timeslot, "Timeslot.startTime is not a string."));
                timeslot.StartTime = ParseTime(timeslotObj["startTime"].GetString());

                //get end time
                Validate(timeslotObj.HasMember("endTime"), CreateParseErrorMessage(timeslot, "Timeslot does not have member 'endTime'."));
                Validate(timeslotObj["endTime"].IsString(), CreateParseErrorMessage(timeslot, "Timeslot.endTime is not a string."));
                timeslot.EndTime = ParseTime(timeslotObj["endTime"].GetString());

                //get days
                Validate(timeslotObj.HasMember("days"), CreateParseErrorMessage(timeslot, "Timeslot does not have member 'days'."));
                Validate(timeslotObj["days"].IsArray(), CreateParseErrorMessage(timeslot, "Timeslot.days is not an array."));
                auto daysArray = timeslotObj["days"].GetArray();

                uint8_t& days = timeslot.Days;
                for (auto& dayElement : daysArray)
                {
                    Validate(dayElement.IsString(), CreateParseErrorMessage(timeslot, "Day is not a string."));
                    auto day = std::string(dayElement.GetString());

                    if (day == "Mon") {
                        days |= 0x1;
                    }
                    else if (day == "Tues") {
                        days |= 0x2;
                    }
                    else if (day == "Wed") {
                        days |= 0x4;
                    }
                    else if (day == "Thur") {
                        days |= 0x8;
                    }
                    else if (day == "Fri") {
                        days |= 0x10;
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
