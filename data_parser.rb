class Course
    def initialize
        @name = ''
        @code = ''
        @prof = ''
        @credits = ''
        @sections = []
    end

    attr_accessor :name
    attr_accessor :code
    attr_accessor :prof
    attr_accessor :credits
    attr_accessor :sections

    def to_str
        to_s
    end

    def to_s
        str = "Name: #{@name}\nCode: #{@code}\nProf: #{@prof}\nCredits: #{@credits}"
        sections.each_with_index { |section, index| str += "\n\nSection #{index+1}:\n#{section}" }
        return str;
    end
end

class CourseSection
    def initialize
        @code = ''
        @is_open = false
        @capacity = ''
        @timeslots = []
    end
    
    attr_accessor :code
    attr_accessor :is_open
    attr_accessor :capacity
    attr_accessor :timeslots

    def to_str
        to_s
    end

    def to_s
        str = "Code: #{@code}\nIs Open: #{@is_open}\nCapacity: #{@capacity}\nTimeslots:"
        timeslots.each { |timeslot| str += "\n#{timeslot}" }
        return str
    end
end

class TimeSlot
    def initialize
        @type = ''
        @days = []
        @start_time = ''
        @end_time = ''
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
        "Type: #{@type}\nDays: #{@days.join(', ')}\nTime: #{@start_time} - #{@end_time}\nLocation: #{@location}"
    end
end

class DataParser
    def initialize
        @courses = Hash.new
    end

    def courses
        @courses.values
    end
    
    def parse(sections_data)
        sections_data.each do |section_data|
            course = get_or_create_course(section_data)
            course.sections << create_section(section_data)
        end
    end

    def parse_capicity_string(capacity_str)
        regex = /
            (?<available>\d+)\W+(?<total>\d+)
        /x

        matches = match_all(capacity_str, regex)
        raise "Invalid capacity string: '#{capacity_str}'" if matches.empty?

        return matches[0]
    end

    def parse_name_string(name_str)
        regex = /
            (?<course_code>[A-Z]{3,4}\*\d{4})\*
            (?<section_code>\d{4})(\W+\w*\W+|\W+)
            (?<name>.*)
        /x
        
        matches = match_all(name_str, regex)
        raise "Invalid name string: '#{name_str}'" if matches.empty?

        return matches[0]
    end

    def parse_timeslots_string(timeslots_str)
        regex = /
            (?<type>LEC|LAB)
            (?<days>(\W+(Mon|Tues|Wed|Thur|Fri))+)\W+
            (?<times>(\d{2}:\d{2}(AM|PM)(\W*-\W*)?){2})\W+
            (?<building>[A-Z]{4})\W+Room\W+
            (?<room>\d+)
        /x

        matches = match_all(timeslots_str, regex)
        raise "Invalid timeslots string: '#{timeslots_str}'" if matches.empty?

        return matches
    end

    private
    def match_all(str, regex)
        str.to_enum(:scan, regex).map { Regexp.last_match }
    end

    def get_or_create_course(src_data)
        match = parse_name_string(src_data[:name])

        # check if this course already exists, and return it if it does
        course_code = match[:course_code]
        course = @courses[course_code]
        return course unless course.nil?

        # create a new course info using the data
        course = Course.new
        course.code = course_code
        course.name = match[:name]
        course.prof = src_data[:prof]
        course.credits = src_data[:credits]

        # add the new course info to the hash
        @courses[course_code] = course
        return course
    end

    def create_section(src_data)
        section = CourseSection.new

        # set the base section data
        section.code = parse_name_string(src_data[:name])[:section_code]
        section.is_open = (src_data[:status] == "Open")
        section.capacity = parse_capicity_string(src_data[:capacity])[:available]

        # create the timeslots
        matches = parse_timeslots_string(src_data[:timeslots])
        matches.each do |match|
            timeslot = TimeSlot.new
            timeslot.type = match[:type]
            match[:days].strip.split(/\W+/).each { |day| timeslot.days << day }

            times = match[:times].strip.split(/\W*-\W*/)
            timeslot.start_time = times[0]
            timeslot.end_time = times[1]
            
            timeslot.location = "#{match[:building]} #{match[:room]}"

            section.timeslots << timeslot
        end

        return section
    end
end
