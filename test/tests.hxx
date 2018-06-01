#define BOOST_TEST_MODULE bounding tests

#include <string>
#include <vector>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
//#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "../parser/parser.hxx"

// #define TEST_PARSER
#define TEST_ALGO

#ifdef TEST_PARSER

namespace filesystem = boost::filesystem;
namespace udata = boost::unit_test::data;
namespace spirit = boost::spirit;

BOOST_AUTO_TEST_SUITE(parser)

	bool parseString(const std::string& input, bool semantics_check)
	{
		auto b = input.begin();
		auto res = parse(b, input.end());
		return std::get<1>(res) && (std::get<2>(res) || !semantics_check);
	}

	BOOST_AUTO_TEST_CASE(parse_line)
	{
		// check simple syntax
		BOOST_TEST(parseString("", false));
		BOOST_TEST(parseString("\n\n\n", false));
		BOOST_TEST(parseString("# only comment", false));
		BOOST_TEST(parseString("# one comment\n    # two comment", false));

		BOOST_TEST(parseString("v 1.5 3 2 5.6", false));
		BOOST_TEST(parseString("v 1.5 5.6 3      ", false));
		BOOST_TEST(parseString("v 1.5    5.6 3   \n v 3 4 5", false));

		BOOST_TEST(parseString("# start comment\nv 1 2 3", false));
		BOOST_TEST(parseString("v 1 2 3\n#end comment", false));
		BOOST_TEST(parseString("#first enclosing comment\nv 1 2 3\n#second enclosing comment", false));
		BOOST_TEST(parseString("v 1 2 3 # partial comment\nv 4 5 6", false));

	}

	std::vector<filesystem::path> fetch_test_data(const std::string& root_str)
	{
		std::vector<filesystem::path> data;
		filesystem::path root(root_str);
		if(!(filesystem::exists(root) && filesystem::is_directory(root)))
		{
			BOOST_FAIL("Can't access root for test-data. Expected location: " + root.string());
		}
		for(auto& obj: filesystem::recursive_directory_iterator(root))
		{
			if(!(filesystem::is_regular_file(obj.status()) &&
				boost::iequals(obj.path().extension().string(), ".obj"))) {
				continue;
			}
			data.push_back(obj.path());
		}
		return data;
	}

	BOOST_DATA_TEST_CASE(
		parse_correct,
		udata::make(fetch_test_data("./test/data/obj/correct")),
		file)
	{
		std::ifstream in(file.string());
		if(!in.is_open())
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		std::cout << "checking '" << file.string() << "'\n";

		auto res = parse(in);
		bool syntax = std::get<1>(res);
		bool semantics = std::get<2>(res);

		BOOST_TEST(syntax);
		BOOST_TEST(semantics);

	}

	BOOST_DATA_TEST_CASE(
		parse_semantics_fail,
		udata::make(fetch_test_data("./test/data/obj/semantics_fail")),
		file)
	{
		std::ifstream in(file.string());
		if(!in.is_open())
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		std::cout << "checking '" << file.string() << "'\n";

		auto res = parse(in);
		bool syntax = std::get<1>(res);
		bool semantics = std::get<2>(res);

		BOOST_TEST(syntax);
		BOOST_TEST(!semantics);

	}

BOOST_AUTO_TEST_SUITE_END()

#endif // TEST_PARSER

#ifdef TEST_ALGO

#include "../algo/bounding.hxx"
#include "../algo/surface.hxx"
#include "../algo/volume.hxx"

BOOST_AUTO_TEST_SUITE(algo)

	BOOST_AUTO_TEST_CASE(bounding)
	{

	}

	BOOST_AUTO_TEST_CASE(surface)
	{

		std::ifstream in("./test/data/obj/correct/cube.OBJ");
		if(!in.is_open())
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		auto res = parse(in);

		if(!(std::get<1>(res) && std::get<2>(res)))
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		double surface = calculate_surface(std::get<0>(res)); 
		double volume = calculate_volume(std::get<0>(res)); 
		range_t minmax = calculate_aabb(std::get<0>(res));

		std::cout << "Surface of cube = " << surface << "\n";
		std::cout << "Volume of cube = " << volume << "\n";
		std::cout << "Range of cube = " << minmax.first << " - " << minmax.second << "\n";

		std::ifstream in2("./test/data/obj/correct/octahedron.OBJ");
		if(!in2.is_open())
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		res = parse(in2);

		if(!(std::get<1>(res) && std::get<2>(res)))
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		surface = calculate_surface(std::get<0>(res)); 
		volume = calculate_volume(std::get<0>(res)); 
		minmax = calculate_aabb(std::get<0>(res));

		std::cout << "Surface of octahedron = " << surface << "\n";
		std::cout << "Volume of octahedron = " << volume << "\n";
		std::cout << "Range of octahedron = " << minmax.first << " - " << minmax.second << "\n";

		std::ifstream in3("./test/data/obj/correct/tetrahedron.OBJ");
		if(!in3.is_open())
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		res = parse(in3);

		if(!(std::get<1>(res) && std::get<2>(res)))
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		surface = calculate_surface(std::get<0>(res)); 
		volume = calculate_volume(std::get<0>(res)); 
		minmax = calculate_aabb(std::get<0>(res));

		std::cout << "Surface of tetrahedron = " << surface << "\n";
		std::cout << "Volume of tetrahedron = " << volume << "\n";
		std::cout << "Range of tertrahedron = " << minmax.first << " - " << minmax.second << "\n";
	}

BOOST_AUTO_TEST_SUITE_END()

#endif // TEST_ALGO

