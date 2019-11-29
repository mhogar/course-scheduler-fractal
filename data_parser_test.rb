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
        test_string = 'LEC Mon, Wed 05:30PM - 06:50PM, ROZH, Room 102'
        timeslot = @data_parser.parse_timeslots_string(test_string)[0]

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
end
