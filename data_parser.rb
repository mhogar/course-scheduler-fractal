require 'time'

# test_data = {:status=>"Open", :name=>"CIS*2750*0101 (6778) Software System Dvlmt & Intgrn", 
#     :timeslots=>"2020/01/06-2020/04/20 LEC Mon, Wed 05:30PM - 06:50PM, ROZH, Room 102\n2020/01/06-2020/04/20 LAB Wed 08:30AM - 10:20AM, THRN, Room 2418", 
#     :prof=>"D. Nikitenko", :capacity=>"1 / 19", :credits=>"0.75"}

class CourseInfo
    def initialize
        @name = ''
        @code = ''
        @prof = ''
        @credits = ''
    end

    attr_accessor :name
    attr_accessor :code
    attr_accessor :prof
    attr_accessor :credits

    def to_str
        to_s
    end

    def to_s
        "Name: #{@name}\nCode: #{@code}\nProf: #{@prof}\nCredits: #{@credits}"
    end
end

class CourseSection
    def initialize
        @code = ''
        @is_open = false
        @capacity = ''
        @course_info = nil
        @timeslots = []
    end
    
    attr_accessor :code
    attr_accessor :is_open
    attr_accessor :capacity
    attr_accessor :course_info
    attr_accessor :timeslots

    def to_str
        to_s
    end

    def to_s
        str = "Code: #{@code}\nIs Open: #{@is_open}\nCapacity: #{@capacity}\n#{course_info}"
        timeslots.each_with_index { |timeslot, index| str += "\nTimeslot #{index+1}:\n#{timeslot}" }
        return str
    end
end

class TimeSlot
    def initialize
        @type = ''
        @days = []
        @start_time = nil
        @end_time = nil
        @location = ''
    end

    attr_accessor :type
    attr_accessor :days
    attr_accessor :start_time
    attr_accessor :end_time
    attr_accessor :location

    def to_str
        to_s
    end

    def to_s
        "Type: #{@type}\nDays: #{@days.join(', ')}\n" \
        "Time: #{@start_time.strftime("%I:%M%p")} - #{@end_time.strftime("%I:%M%p")}\nLocation: #{@location}"
    end
end

class DataParser
    def initialize
        @course_infos = Hash.new
        @sections = []
    end

    def parse(sections_data)
        # convert to array if it is not already
        sections_data = [sections_data] unless sections_data.kind_of?(Array)

        sections_data.each do |section_data|
            section = create_section(section_data)
            section.course_info = create_course_info(section_data)
            section.timeslots = create_timeslots(section_data)

            @sections << section
        end
    end

    attr_reader :sections

    def parse_capicity_string(capacity_str)
        regex = /
            (?<available>\d+)\W+(?<total>\d+)
        /x

        matches = match_all(capacity_str, regex)
        raise 'Invalid capacity string' if matches.empty?

        return matches[0]
    end

    def parse_name_string(name_str)
        regex = /
            (?<course_code>[A-Z]{3,4}\*\d{4})\*
            (?<section_code>\d{4})(\W+\w*\W+|\W+)
            (?<name>.*)
        /x
        
        matches = match_all(name_str, regex)
        raise 'Invalid name string' if matches.empty?

        return matches[0]
    end

    def parse_timeslots_string(timeslots_str)
        regex = /
            (?<type>LEC|LAB)
            (?<days>(\W+(Mon|Tues|Wed|Thurs|Fri))+)\W+
            (?<times>(\d{2}:\d{2}(AM|PM)(\W*-\W*)?){2})\W+
            (?<building>[A-Z]{4})\W+Room\W+
            (?<room>\d+)
        /x

        matches = match_all(timeslots_str, regex)
        raise 'Invalid timeslots string' if matches.empty?

        return matches
    end

    private
    def match_all(str, regex)
        str.to_enum(:scan, regex).map { Regexp.last_match }
    end

    def create_section(src_data)
        section = CourseSection.new

        section.code = parse_name_string(src_data[:name])[:section_code]
        section.is_open = (src_data[:status] == "Open")
        section.capacity = parse_capicity_string(src_data[:capacity])[:available]

        return section
    end

    def create_course_info(src_data)
        match = parse_name_string(src_data[:name])

        # check if this course already exists, and return it if it does
        course_code = match[:course_code]
        course_info = @course_infos[course_code]
        return course_info unless course_info.nil?

        # create a new course info using the data
        course_info = CourseInfo.new
        course_info.code = course_code
        course_info.name = match[:name]
        course_info.prof = src_data[:prof]
        course_info.credits = src_data[:credits]

        # add the new course info to the hash
        @course_infos[course_code] = course_info
        return course_info
    end

    def create_timeslots(src_data)
        matches = parse_timeslots_string(src_data[:timeslots])

        # create the timeslots from the matches
        timeslots = []
        matches.each do |match|
            timeslot = TimeSlot.new
            timeslot.type = match[:type]
            match[:days].strip.split(/\W+/).each { |day| timeslot.days << day }

            times = match[:times].strip.split(/\W*-\W*/)
            timeslot.start_time = Time.parse(times[0])
            timeslot.end_time = Time.parse(times[1])
            
            timeslot.location = "#{match[:building]} #{match[:room]}"

            timeslots << timeslot
        end

        return timeslots
    end
end
