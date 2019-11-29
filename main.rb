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

data_parser.sections.each_with_index do |section, index|
    puts "\nSection #{index+1}:\n#{section}\n"
end
