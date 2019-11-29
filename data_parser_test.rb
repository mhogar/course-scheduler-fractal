require './data_parser'
require 'test/unit'

class TestDataParser < Test::Unit::TestCase
    def setup
        @data_parser = DataParser.new
    end

    test "parse capacity string succeds" do
        assert_nothing_raised( RuntimeError ) do
            test_string = '1/2'
            capacity = @data_parser.parse_capicity_string(test_string)

            assert_not_nil(capacity[:available])
            assert_equal('1', capacity[:available])

            assert_not_nil(capacity[:total])
            assert_equal('2', capacity[:total])
        end
    end

    test "parse invalid capacity string raises error" do
        test_string = 'foo'
        assert_raise( RuntimeError ) { @data_parser.parse_capicity_string(test_string) }
    end

    test "parse name string succeds" do
        assert_nothing_raised( RuntimeError ) do
            test_string = 'CIS*2750*1111 (0000) Angel'
            name = @data_parser.parse_name_string(test_string)

            assert_not_nil(name[:course_code])
            assert_equal('CIS*2750', name[:course_code])

            assert_not_nil(name[:section_code])
            assert_equal('1111', name[:section_code])

            assert_not_nil(name[:name])
            assert_equal('Angel', name[:name])
        end
    end

    test "parse invalid name string raises error" do
        test_string = 'foo'
        assert_raise( RuntimeError ) { @data_parser.parse_capicity_string(test_string) }
    end
end
