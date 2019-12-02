require './data_parser'
require 'test/unit'

class TestDataParser < Test::Unit::TestCase
    def setup
        @data_parser = DataParser.new
    end

    test "parse capacity string succeds" do
        test_string = '1/2'
        capacity = @data_parser.parse_capicity_string(test_string)

        assert_not_nil(capacity[:available])
        assert_equal('1', capacity[:available])

        assert_not_nil(capacity[:total])
        assert_equal('2', capacity[:total])
    end

    test "parse invalid capacity string raises error" do
        test_string = 'foo'
        assert_raise( RuntimeError ) { @data_parser.parse_capicity_string(test_string) }
    end

    test "parse name string succeds" do
        test_string = 'CIS*2750*1111 (0000) Angel'
        name = @data_parser.parse_name_string(test_string)

        assert_not_nil(name[:course_code])
        assert_equal('CIS*2750', name[:course_code])

        assert_not_nil(name[:section_code])
        assert_equal('1111', name[:section_code])

        assert_not_nil(name[:name])
        assert_equal('Angel', name[:name])
    end

    test "parse invalid name string raises error" do
        test_string = '*1111 (0000) Angel' # no course code
        assert_raise( RuntimeError ) { @data_parser.parse_name_string(test_string) }

        test_string = 'CIS*2750* (0000) Angel' # no section code
        assert_raise( RuntimeError ) { @data_parser.parse_name_string(test_string) }

        test_string = 'CIS*2750* (0000) ' # no name
        assert_raise( RuntimeError ) { @data_parser.parse_name_string(test_string) }
    end

    test "parse timeslots string succeds" do
        test_string = 'LEC Mon, Wed 05:30PM - 06:50PM, ROZH, Room 102\nLAB Thur 08:30AM - 10:20AM, WMEM, Room 101'
        timeslots = @data_parser.parse_timeslots_string(test_string)

        assert_equal(2, timeslots.length)
        
        # first timeslot
        timeslot = timeslots[0]

        assert_not_nil(timeslot[:type])
        assert_equal('LEC', timeslot[:type])

        assert_not_nil(timeslot[:days])
        assert_equal(' Mon, Wed', timeslot[:days])

        assert_not_nil(timeslot[:times])
        assert_equal('05:30PM - 06:50PM', timeslot[:times])

        assert_not_nil(timeslot[:building])
        assert_equal('ROZH', timeslot[:building])

        assert_not_nil(timeslot[:room])
        assert_equal('102', timeslot[:room])

        # second timeslot
        timeslot = timeslots[1]
        
        assert_not_nil(timeslot[:type])
        assert_equal('LAB', timeslot[:type])

        assert_not_nil(timeslot[:days])
        assert_equal(' Thur', timeslot[:days])

        assert_not_nil(timeslot[:times])
        assert_equal('08:30AM - 10:20AM', timeslot[:times])

        assert_not_nil(timeslot[:building])
        assert_equal('WMEM', timeslot[:building])

        assert_not_nil(timeslot[:room])
        assert_equal('101', timeslot[:room])
    end

    test "parse invalid timeslots string raises error" do
        test_string = 'Mon, Wed 05:30PM - 06:50PM, ROZH, Room 102' # no LEC/LAB 
        assert_raise( RuntimeError ) { @data_parser.parse_timeslots_string(test_string) }

        test_string = 'LEC 05:30PM - 06:50PM, ROZH, Room 102' # no days
        assert_raise( RuntimeError ) { @data_parser.parse_timeslots_string(test_string) }

        test_string = 'LEC Mon, Wed, ROZH, Room 102' # no times
        assert_raise( RuntimeError ) { @data_parser.parse_timeslots_string(test_string) }

        test_string = 'LEC Mon, Wed 05:30PM - 06:50PM, Room 102' # no building 
        assert_raise( RuntimeError ) { @data_parser.parse_timeslots_string(test_string) }

        test_string = 'LEC Mon, Wed 05:30PM - 06:50PM, ROZH' # no room 
        assert_raise( RuntimeError ) { @data_parser.parse_timeslots_string(test_string) }
    end

    test "parse succeds and creates correct courses" do
        test_data = [
            {
                status: 'Open',
                name: 'CIS*2750*1111 Angel',
                timeslots: 'LEC Mon, Wed, Fri 05:30PM - 06:20PM, ROZH, Room 102',
                prof: 'Someone',
                capacity: '1/2',
                credits: '0.75'
            },
            {
                status: 'Open',
                name: 'CIS*2750*2222 Angel',
                timeslots: 'LEC Tues, Thur 10:30AM - 11:50AM, THRN, Room 2108',
                prof: 'Someone',
                capacity: '1/2',
                credits: '0.75'
            },
            {
                status: 'Closed',
                name: 'CIS*3110*1111 OS',
                timeslots: 'LEC Mon, Wed, Fri 08:30AM - 09:20AM, ROZH, Room 102\nLAB Thur 08:30AM - 10:20AM, THRN, Room 2108',
                prof: 'Someone Else',
                capacity: '2/20',
                credits: '0.5'
            }
        ]
        @data_parser.parse(test_data)

        # validate the number of courses
        courses = @data_parser.courses
        assert_equal(2, courses.length)

        ### validate the first course ###
        course = courses[0]
        assert_equal('Angel', course.name)
        assert_equal('CIS*2750', course.code)
        assert_equal('Someone', course.prof)
        assert_equal('0.75', course.credits)

        # validate the number of sections
        sections = course.sections
        assert_equal(2, sections.length)

        ## validate the first section ##
        section = sections[0]
        assert_equal('1111', section.code)
        assert(section.is_open)
        assert_equal('1', section.capacity)

        # validate the number of timeslots
        timeslots = section.timeslots
        assert_equal(1, timeslots.length)

        # validate the timeslot
        timeslot = timeslots[0]
        assert_equal('LEC', timeslot.type)
        assert_equal('ROZH 102', timeslot.location)
        assert_equal('05:30PM', timeslot.start_time)
        assert_equal('06:20PM', timeslot.end_time)

        # validate the days
        days = timeslot.days
        assert_equal(3, days.length)
        assert_equal('Mon', days[0])
        assert_equal('Wed', days[1])
        assert_equal('Fri', days[2])

        ## validate the second section ##
        section = sections[1]
        assert_equal('2222', section.code)
        assert(section.is_open)
        assert_equal('1', section.capacity)

        # validate the number of timeslots
        timeslots = section.timeslots
        assert_equal(1, timeslots.length)

        # validate the timeslot
        timeslot = timeslots[0]
        assert_equal('LEC', timeslot.type)
        assert_equal('THRN 2108', timeslot.location)
        assert_equal('10:30AM', timeslot.start_time)
        assert_equal('11:50AM', timeslot.end_time)

        # validate the days
        days = timeslot.days
        assert_equal(2, days.length)
        assert_equal('Tues', days[0])
        assert_equal('Thur', days[1])

        ### validate the second course ###
        course = courses[1]
        assert_equal('OS', course.name)
        assert_equal('CIS*3110', course.code)
        assert_equal('Someone Else', course.prof)
        assert_equal('0.5', course.credits)

        # validate the number of sections
        sections = course.sections
        assert_equal(1, sections.length)

        ## validate the section ##
        section = sections[0]
        assert_equal('1111', section.code)
        assert_false(section.is_open)
        assert_equal('2', section.capacity)

        # validate the number of timeslots
        timeslots = section.timeslots
        assert_equal(2, timeslots.length)

        # validate the first timeslot
        timeslot = timeslots[0]
        assert_equal('LEC', timeslot.type)
        assert_equal('ROZH 102', timeslot.location)
        assert_equal('08:30AM', timeslot.start_time)
        assert_equal('09:20AM', timeslot.end_time)

        # validate the days
        days = timeslot.days
        assert_equal(3, days.length)
        assert_equal('Mon', days[0])
        assert_equal('Wed', days[1])
        assert_equal('Fri', days[2])

        # validate the second timeslot
        timeslot = timeslots[1]
        assert_equal('LAB', timeslot.type)
        assert_equal('THRN 2108', timeslot.location)
        assert_equal('08:30AM', timeslot.start_time)
        assert_equal('10:20AM', timeslot.end_time)

        # validate the days
        days = timeslot.days
        assert_equal(1, days.length)
        assert_equal('Thur', days[0])
    end
end
