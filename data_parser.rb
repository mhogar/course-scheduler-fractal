require 'time'

# t1 = Time.parse('05:30PM')
# t2 = Time.parse('05:30PM')

# puts t1
# puts t2
# puts (t1 == t2)

test_data = {:status=>"Open", :name=>"CIS*2750*0101 (6778) Software System Dvlmt & Intgrn", 
    :timeslots=>"2020/01/06-2020/04/20 LEC Mon, Wed 05:30PM - 06:50PM, ROZH, Room 102\n2020/01/06-2020/04/20 LAB Wed 08:30AM - 10:20AM, THRN, Room 2418", 
    :prof=>"D. Nikitenko", :capacity=>"1 / 19", :credits=>"0.75"}

class CourseInfo
    attr_accessor :name
    attr_accessor :code
    attr_accessor :prof
    attr_accessor :credits
end

class TimeSlot
    def initialize
        @days = []
    end

    attr_accessor :type
    attr_accessor :days
    attr_accessor :start_time
    attr_accessor :end_time
    attr_accessor :location
end

class DataParser
    def initialize(data)
        @status = data[:status]
        @prof = data[:prof]
        @credits = data[:credits]

        parse_capacity(data[:capacity])
        parse_name(data[:name])
    end

    private:
    def parse_capacity(capacity_str)
        CAPACITY_REGEX = %r{(?<available>\d+)\W+(?<total>\d+)}

        match = capacity_str.match(CAPACITY_REGEX)
        raise 'Invalid capacity string' if match.nil?

        return match[:available]
    end

    def parse_name(name_str)
        tokens = name.split(' ')
        code_tokens = token[0].split('*')
        
        @course_code = code_tokens[0] + '*' + code_tokens[1]
        @section_code = code_tokens[2]
    end

    def parse_timeslots(timeslots_str)
        #2020/01/06-2020/04/20 LEC Mon, Wed 05:30PM - 06:50PM, ROZH, Room 102
        TIMESLOT_REGEX = %r{
            (?<type>LEC|LAB)
            (?<days>(\W+(Mon|Tues|Wed|Thurs|Fri))+)
            (?<times>(\W+\d{2}:\d{2}(AM|PM)){2})\W+
            (?<building>[A-Z]{4})\W+Room\W+
            (?<room>\d+)
        }

        matches = timeslots_str.match(TIMESLOT_REGEX)
        raise 'Invalid timeslots string' if matches.nil?

        # create the timeslots from the matches
        timeslots = []
        matches.each do |match|
            timeslot = TimeSlot.new
            timeslot.type = match[:type]
            match[:days].strip.split(", ") { |day| timeslot.days << day }

            times = match[:times].strip.split(" - ")
            timeslot.start_time = times[0]
            timeslot.end_time = times[1]
            
            timeslot.location = "#{match[:building]} #{match[:room]}"

            timeslots << timeslot
        end

        return timeslots
    end
end

#(LEC|LAB)\W+(Mon|Tues|Wed|Thurs|Fri)\W+(\d{2}:\d{2}AM|\d{2}:\d{2}PM)\W+(?<building>[A-Z]{4}), Room (?<room>\d+)