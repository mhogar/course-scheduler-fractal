require 'watir'
require 'webdrivers'
require 'nokogiri'
require './data_parser'

search_fields = {
    term: 'W20',
    subject: 'CIS',
    course_code: '2750'
}

browser = Watir::Browser.new :chrome, headless: true
browser.window.resize_to 1024, 768 # set the size to fix issues when running in headless mode

# navigate to the 'search for sections' page
browser.goto("https://webadvisor.uoguelph.ca")
browser.link(text: 'Students').click
browser.link(text: 'Search for Sections').click

# fill in the search form
browser.select_list(id: 'VAR1').select(search_fields[:term])
browser.select_list(id: 'LIST_VAR1_1').select(search_fields[:subject])
browser.text_field(id: 'LIST_VAR3_1').set(search_fields[:course_code])
browser.input(name: 'SUBMIT2').click

# extract the html from the search results page
doc = Nokogiri::HTML(browser.html)

# close the browser now we are done with it
browser.close

data_parser = DataParser.new

# locate the results table and parse its data
table = doc.search('table')[2]
table.search('tr').each do |tr|
    cells = tr.search('td')
    next if cells.length == 0

    section_data = {}
    cells.each do |cell|
        el = cell.at('input')
        unless el.nil?
            data = el.values[2]
            case el.values[1]
            when /LIST.VAR1_/
                section_data[:status] = data
            when /SEC.MEETING.INFO_/
                section_data[:timeslots] = data
            when /SEC.FACULTY.INFO_/
                section_data[:prof] = data
            when /LIST.VAR5_/
                section_data[:capacity] = data
            when /SEC.MIN.CRED_/
                section_data[:credits] = data
            end
        else
            el = cell.at('a')
            if !el.nil?
                section_data[:name] = el.text
            end
        end
    end

    data_parser.parse(section_data)
end

data_parser.sections.each_with_index do |section, index|
    puts "\nSection #{index+1}:\n#{section}\n"
end
