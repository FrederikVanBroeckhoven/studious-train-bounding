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

namespace udata = boost::unit_test::data;

#define TEST_PARSER
#define TEST_ALGO

#ifdef TEST_PARSER

namespace filesystem = boost::filesystem;
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
#include "../main/bbox.hxx"

BOOST_AUTO_TEST_SUITE(algo)

	std::vector<vertex_t> generate_vertices(size_t num, scalar_t min = 0.0000001, scalar_t max = 1.0)
	{
		std::vector<vertex_t> data;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> dis(min, max);
		
		for(int i = 0; i < num; i++)
		{
			data.push_back( { { dis(gen), dis(gen), dis(gen) } } );
		}

		return data;
	}

	std::vector<vertex_t> generate_out_vertices(size_t num_per_quad)
	{
		std::vector<vertex_t> data;
		std::random_device rd;
		std::mt19937 gen(rd());
		
		std::uniform_real_distribution<double> dis[3] = {
			std::uniform_real_distribution<double>(-1., 0.),
			std::uniform_real_distribution<double>(0.0000001, 1.),
			std::uniform_real_distribution<double>(1.0000001, 2.)
		};
		
		for(int i = 0; i < num_per_quad; i++)
		{
			for(int a = 0; a < 3; a++)
			{
				for(int b = 0; b < 3; b++)
				{
					for(int c = 0; c < 3; c++)
					{
						if(a == 1 && b == 1 && c == 1)
						{
							continue;
						}
						data.push_back( { { dis[a](gen), dis[b](gen), dis[c](gen) } } );
					}
				}
			}
		}
		return data;
	}

	struct bounding_box_fixture {
		bounding_box_fixture() : bbox( {
			vertex_t { { 0.0,  0.0,  0.0 } },
			vertex_t { { 0.0,  0.0,  1.0 } },
			vertex_t { { 0.0,  1.0,  0.0 } },
			vertex_t { { 0.0,  1.0,  1.0 } },
			vertex_t { { 1.0,  0.0,  0.0 } },
			vertex_t { { 1.0,  0.0,  1.0 } },
			vertex_t { { 1.0,  1.0,  0.0 } },
			vertex_t { { 1.0,  1.0,  1.0 } }
		} ) {}
		~bounding_box_fixture() {}
		aabb bbox;
	};

	BOOST_DATA_TEST_CASE_F(
		bounding_box_fixture,
		excludes_vertex,
		udata::make(generate_out_vertices(5)),
		vert)
	{
		BOOST_TEST(!(bbox > vert));
	}

	BOOST_DATA_TEST_CASE_F(
		bounding_box_fixture,
		includes_vertex,
		udata::make(generate_vertices(5)),
		vert)
	{
		BOOST_TEST(bbox > vert);
	}

	BOOST_FIXTURE_TEST_CASE(includes_excludes_vertex_sets, bounding_box_fixture)
	{
		vertex_set_t vs1 = generate_out_vertices(5);

		BOOST_TEST(!(bbox > vs1));

		vertex_set_t vs2 = generate_vertices(5);

		BOOST_TEST((bbox > vs2));
	}

	BOOST_FIXTURE_TEST_CASE(includes_excludes_model, bounding_box_fixture)
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

		model_t equal = model_t(std::get<0>(res));
		model_t bigger = model_t(std::get<0>(res));
		std::transform(
			std::get<vertex_set_t>(bigger).begin(),
			std::get<vertex_set_t>(bigger).end(),
			std::get<vertex_set_t>(bigger).begin(),
			[] (vertex_t& v) { return vertex_t { { v.x() * 3 - 1, v.y() * 3 - 1, v.z() * 3 - 1  } }; } );
		model_t smaller = model_t(std::get<0>(res));
		std::transform(
			std::get<vertex_set_t>(smaller).begin(),
			std::get<vertex_set_t>(smaller).end(),
			std::get<vertex_set_t>(smaller).begin(),
			[] (vertex_t& v) { return vertex_t { { v.x() * 0.5 + 0.25, v.y() * 0.5 + 0.25, v.z() * 0.5 + 0.25  } }; } );
		model_t cutting = model_t(std::get<0>(res));
		std::transform(
			std::get<vertex_set_t>(cutting).begin(),
			std::get<vertex_set_t>(cutting).end(),
			std::get<vertex_set_t>(cutting).begin(),
			[] (vertex_t& v) { return vertex_t { { v.x() - 0.5, v.y() - 0.5, v.z() - 0.5  } }; } );
		model_t outside = model_t(std::get<0>(res));
		std::transform(
			std::get<vertex_set_t>(outside).begin(),
			std::get<vertex_set_t>(outside).end(),
			std::get<vertex_set_t>(outside).begin(),
			[] (vertex_t& v) { return vertex_t { { v.x() + 2, v.y() + 2, v.z() + 2  } }; } );


		BOOST_TEST((bbox > smaller));
		BOOST_TEST(!(bbox > bigger));
		BOOST_TEST(!(bbox > equal));
		BOOST_TEST(!(bbox > cutting));
		BOOST_TEST(!(bbox > outside));
	}

	BOOST_FIXTURE_TEST_CASE(logic_operators, bounding_box_fixture)
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

		model_t equal = model_t(std::get<0>(res));
		model_t bigger = model_t(std::get<0>(res));
		std::transform(
			std::get<vertex_set_t>(bigger).begin(),
			std::get<vertex_set_t>(bigger).end(),
			std::get<vertex_set_t>(bigger).begin(),
			[] (vertex_t& v) { return vertex_t { { v.x() * 3 - 1, v.y() * 3 - 1, v.z() * 3 - 1  } }; } );
		model_t smaller = model_t(std::get<0>(res));
		std::transform(
			std::get<vertex_set_t>(smaller).begin(),
			std::get<vertex_set_t>(smaller).end(),
			std::get<vertex_set_t>(smaller).begin(),
			[] (vertex_t& v) { return vertex_t { { v.x() * 0.5 + 0.25, v.y() * 0.5 + 0.25, v.z() * 0.5 + 0.25  } }; } );
		model_t cutting = model_t(std::get<0>(res));
		std::transform(
			std::get<vertex_set_t>(cutting).begin(),
			std::get<vertex_set_t>(cutting).end(),
			std::get<vertex_set_t>(cutting).begin(),
			[] (vertex_t& v) { return vertex_t { { v.x() - 0.5, v.y() - 0.5, v.z() - 0.5  } }; } );
		model_t outside = model_t(std::get<0>(res));
		std::transform(
			std::get<vertex_set_t>(outside).begin(),
			std::get<vertex_set_t>(outside).end(),
			std::get<vertex_set_t>(outside).begin(),
			[] (vertex_t& v) { return vertex_t { { v.x() + 2, v.y() + 2, v.z() + 2  } }; } );

		aabb sbbox(smaller);
		aabb bbbox(bigger);
		aabb ebbox(equal);
		aabb cbbox(cutting);
		aabb obbox(outside);

		BOOST_TEST((bbox != sbbox));
		BOOST_TEST((bbox > sbbox));
		BOOST_TEST((bbox >= sbbox));
		BOOST_TEST((sbbox < bbox));
		BOOST_TEST((sbbox <= bbox));
		BOOST_TEST((sbbox && bbox));
		BOOST_TEST((bbox && sbbox));

		BOOST_TEST((bbox == ebbox));
		BOOST_TEST((bbox <= ebbox));
		BOOST_TEST((bbox >= ebbox));
		BOOST_TEST((ebbox && bbox));
		BOOST_TEST((bbox && ebbox));

		BOOST_TEST((bbox != bbbox));
		BOOST_TEST((bbox < bbbox));
		BOOST_TEST((bbox <= bbbox));
		BOOST_TEST((bbbox > bbox));
		BOOST_TEST((bbbox >= bbox));
		BOOST_TEST((bbbox && bbox));
		BOOST_TEST((bbox && bbbox));

		BOOST_TEST((bbox != cbbox));
		BOOST_TEST(!(bbox < cbbox));
		BOOST_TEST(!(bbox <= cbbox));
		BOOST_TEST(!(bbox > cbbox));
		BOOST_TEST(!(bbox >= cbbox));
		BOOST_TEST((cbbox && bbox));
		BOOST_TEST((bbox && cbbox));

		BOOST_TEST((bbox != obbox));
		BOOST_TEST(!(bbox < obbox));
		BOOST_TEST(!(bbox <= obbox));
		BOOST_TEST(!(bbox > obbox));
		BOOST_TEST(!(bbox >= obbox));
		BOOST_TEST((obbox || bbox));
		BOOST_TEST((bbox || obbox));
	}

	BOOST_AUTO_TEST_CASE(union_intersection)
	{
		// generate a 'cloud' of vertices
		std::vector<vertex_t> verts = generate_vertices(300, 2, 20);

		// generate two random aabb's that overlap and both contain
		// all the generated vertices in combination
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> dis(12, 15);
		aabb box1( vertex_set_t { vertex_t(1), vertex_t(dis(gen)) } );
		dis = std::uniform_real_distribution<double>(5, 8);
		aabb box2( vertex_set_t { vertex_t(dis(gen)), vertex_t(21) } );

		// split the cloud in based on where they are;
		std::vector<vertex_t> both;
		std::vector<vertex_t> in_b1;
		std::vector<vertex_t> in_b2;
		std::vector<vertex_t> none;

		std::for_each(verts.begin(), verts.end(),
			[ &box1, &box2, &both, &in_b1, &in_b2, &none ] (const vertex_t& v)
			{
				if(box1 > v)
				{
					if(box2 > v)
					{
						both.push_back(v);
					}
					else
					{
						in_b1.push_back(v);
					}
				}
				else
				{
					if(box2 > v)
					{
						in_b2.push_back(v);
					}
					else
					{
						none.push_back(v);
					}
				}
			}
		);
	
		// some basic checks	
		BOOST_TEST((box1 > both));
		BOOST_TEST((box1 > in_b1));
		BOOST_TEST(!(box1 > in_b2));
		BOOST_TEST(!(box1 > none));

		BOOST_TEST((box2 > both));
		BOOST_TEST((box2 > in_b2));
		BOOST_TEST(!(box2 > in_b1));
		BOOST_TEST(!(box2 > none));

		// check the intersection (should only contain from both)
		aabb in = box1 & box2;
		BOOST_TEST((in > both));
		BOOST_TEST(!(in > none));
		BOOST_TEST(!(in > in_b1));
		BOOST_TEST(!(in > in_b2));

		// check the union (should contain from all)
		aabb un = box1 | box2;
		BOOST_TEST((un > both));
		BOOST_TEST((un > in_b1));
		BOOST_TEST((un > in_b2));

	}

	BOOST_AUTO_TEST_CASE(surface_volume)
	{
		// load cube
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

		scalar_t cube_edge = 1;
		model_t cube = std::get<0>(res);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> dis(0.5, 3);
		scalar_t scale = dis(gen);

		std::transform(
			std::get<vertex_set_t>(cube).begin(),
			std::get<vertex_set_t>(cube).end(),
			std::get<vertex_set_t>(cube).begin(),
			[ scale ] (vertex_t& v) {
				return vertex_t { { v.x() * scale, v.y() * scale, v.z() * scale } };
			}
		);

		scalar_t vol_cube = std::pow(cube_edge * scale, 3);
		scalar_t surf_cube = 6 * std::pow(cube_edge * scale, 2);

		BOOST_TEST((std::abs(surf_cube - calculate_surface(cube)) <= 0.00000001)); // close enough
		BOOST_TEST((std::abs(vol_cube - calculate_volume(cube)) <= 0.00000001)); // close enough

		// load octahedron
		in = std::ifstream("./test/data/obj/correct/octahedron.OBJ");
		if(!in.is_open())
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		res = parse(in);

		if(!(std::get<1>(res) && std::get<2>(res)))
		{
			BOOST_ERROR("Can't open file");
			return;
		}

		scalar_t oct_edge = std::sqrt(2);
		model_t oct = std::get<0>(res);

		scale = dis(gen);

		std::transform(
			std::get<vertex_set_t>(oct).begin(),
			std::get<vertex_set_t>(oct).end(),
			std::get<vertex_set_t>(oct).begin(),
			[ scale ] (vertex_t& v) {
				return vertex_t { { v.x() * scale, v.y() * scale, v.z() * scale } };
			}
		);

		scalar_t vol_oct = std::sqrt(2) * std::pow(oct_edge * scale, 3) / 3;
		scalar_t surf_oct = 2 * std::sqrt(3) * std::pow(oct_edge * scale, 2);

		BOOST_TEST((std::abs(surf_oct - calculate_surface(oct)) <= 0.00000001)); // close enough
		BOOST_TEST((std::abs(vol_oct - calculate_volume(oct)) <= 0.00000001)); // close enough
	}

BOOST_AUTO_TEST_SUITE_END()

#endif // TEST_ALGO

