#include "Scheduler.h"
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

    DataParser();
    ~DataParser();

    std::vector<Scheduler::Course> Parse(const char* filename, bool ignoreClosed=false);

private:
    void Validate(bool result, std::string message);
    uint16_t ParseTime(const char* timeStr);

    std::string CreateParseErrorMessage(const Scheduler::Course& course, const char* message="");
    std::string CreateParseErrorMessage(const Scheduler::Section& section, const char* message="");
    std::string CreateParseErrorMessage(const Scheduler::Timeslot& timeslot, const char* message="");
};
