require 'watir'
require 'webdrivers'
require 'nokogiri'

class Scrapper
    def scrape(search_fields)
        doc = get_search_results_page(search_fields)
        scrape_sections_data(doc)
    end

    private
    def get_search_results_page(search_fields)
        raise 'Too many search rows' if search_fields[:rows].length > 5

        browser = Watir::Browser.new :chrome, headless: true
        browser.window.resize_to 1024, 768 # set the size to fix issues when running in headless mode

        # navigate to the 'search for sections' page
        browser.goto("https://webadvisor.uoguelph.ca")
        browser.link(text: 'Students').click
        browser.link(text: 'Search for Sections').click

        # fill in the search form
        browser.select_list(id: 'VAR1').select(search_fields[:term])
        search_fields[:rows].each_with_index do |row, index|
            browser.select_list(id: "LIST_VAR1_#{index+1}").select(row[:subject]) unless row[:subject].nil?
            browser.text_field(id: "LIST_VAR3_#{index+1}").set(row[:course_code]) unless row[:course_code].nil?
            browser.text_field(id: "LIST_VAR4_#{index+1}").set(row[:section]) unless row[:section].nil?
        end
        browser.input(name: 'SUBMIT2').click

        # extract the html from the search results page
        doc = Nokogiri::HTML(browser.html)

        # close the browser now we are done with it
        browser.close

        return doc
    end

    def scrape_sections_data(doc)
        sections_data = []

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

            sections_data << section_data
        end

        return sections_data
    end
end
