#include <vector>
#include <string>
#include <stdexcept>

class DataParser
{
public:
    struct ParseException : public std::runtime_error
    {
        ParseException(const char* message)
            : std::runtime_error(message) {}

        ParseException(std::string message)
            : std::runtime_error(message) {}
    };
    
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

    DataParser();
    ~DataParser();

    std::vector<Course> Parse(const char* filename, bool ignoreClosed=false);

private:
    void Validate(bool result, std::string message);
    uint16_t ParseTime(const char* timeStr);

    std::string CreateParseErrorMessage(const Course& course, const char* message="");
    std::string CreateParseErrorMessage(const Section& section, const char* message="");
    std::string CreateParseErrorMessage(const Timeslot& timeslot, const char* message="");
};
