require './scrapper'
require './data_parser'

search_fields = {
    term: 'W20',
    rows: [
        {
            subject: 'CIS',
            course_code: '2750'
            #section:
        }
    ]
}

scrapper = Scrapper.new
sections_data = scrapper.scrape(search_fields)

data_parser = DataParser.new
data_parser.parse(sections_data)

# data_parser.courses.each_with_index do |course, index|
#     puts "\nCourse #{index+1}:\n#{course}\n"
# end

# write the data json to the file
courses = []
data_parser.courses.each do |course|
    sections = []
    course.sections.each do |section|
        timeslots = []
        section.timeslots.each do |timeslot|
            timeslots << {
                startTime: timeslot.start_time,
                endTime: timeslot.end_time,
                days: timeslot.days
            }
        end
        sections << timeslots
    end
    courses << sections
end
File.open("public/data.json", "w") { |f| f.write(courses.to_json) }
