require './scrapper'
require './data_parser'

search_fields = {
    term: 'W20',
    subject: 'CIS',
    course_code: '2750'
}

scrapper = Scrapper.new
sections_data = scrapper.scrape(search_fields)

data_parser = DataParser.new
data_parser.parse(sections_data)

data_parser.courses.each_with_index do |course, index|
    puts "\nCourse #{index+1}:\n#{course}\n"
end
