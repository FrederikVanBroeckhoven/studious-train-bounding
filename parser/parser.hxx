
#ifndef PARSER_HEADER_FILE
#define PARSER_HEADER_FILE

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>

#include "../main/types.hxx"

namespace spirit = boost::spirit;
namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace encoding = qi::ascii;

///////////////////////////////////////////
//       some config values              //
///////////////////////////////////////////

// const int vertex_dim = 3;
//const int face_dim = 3;

///////////////////////////////////////////
//       (simple) AST definitions        //
///////////////////////////////////////////

template<typename T, int N>
struct ast_param
{
	int n;
	T num[N];

	ast_param(const T& c) : n(1)
	{
		num[0] = c;
	}

	void add(const T& c)
	{
		int i = n++;
		if(i < N)
		{
			num[i] = c;
		} 		
	}
};

typedef ast_param<double, 4> ast_vert;
typedef ast_param<int, 3> ast_face;

struct ast_obj
{
	std::vector<ast_vert> verts;
	std::vector<ast_face> faces;
};

///////////////////////////////////////////
//         parser definitions            //
///////////////////////////////////////////

// parser to define anything that must be skipped (comments)
template <typename It>
class skipper final : public qi::grammar<It>
{
public:
	skipper() : skipper::base_type(rule) {}

private:
	// important *not* to consume the end of line of a comment
	// -> it serves as a marker for the actual parser
	const qi::rule<It> rule =
		encoding::char_('#') >> *(encoding::char_ - qi::eol);
};

// the actual OBJ parser
template <typename It, typename Skipper>
class obj_parser final : public qi::grammar<It, ast_obj(), Skipper>
{
private:
	qi::rule<It, ast_obj(),  Skipper> obj;
	qi::rule<It, int(),  Skipper> face_param;

public:


	obj_parser() : obj_parser::base_type(obj)
	{
		using spirit::_1;
		using spirit::_val;

		face_param =
			qi::int_				[ _val = _1 ] 
			// the following is parsed, but ignored (if present)
			// we do not need normals or UV coordinates
			>> -(

				encoding::char_('/')
				>> -qi::int_
				>> -(
					encoding::char_('/')
					>> -qi::int_
				) 
			);

		obj = *(
			(encoding::char_('v') 
				>> +encoding::blank // now we know it is a vertex definition
				>> qi::double_			[ phoenix::bind(
									// lamda-binding as semantic action
									// here to initialize a new vertex record
									// in the AST...
									[] (ast_obj& dat, const double& p)
									{
										dat.verts.push_back(ast_vert(p));
									}, _val, _1
								) ]					
				>> *( +encoding::blank
					>> qi::double_		[ phoenix::bind(
									// ... and here to update the vertex record
									[] (ast_obj& dat, const double& p) {
										dat.verts.back().add(p);
									}, _val, _1
								) ]
				)
				>> (*encoding::blank >> qi::eol)// restrict to one definition per line
			)
			| (encoding::char_('f')
				>> +encoding::blank // now we know it is a face definition 
				>> face_param			[ phoenix::bind(
									// lamda-binding as semantic action
									// here to initialize a new face record
									// in the AST...
									[] (ast_obj& dat, const int p) {
										dat.faces.push_back(ast_face(p));
									}, _val, _1
								) ]					
				>> *( +encoding::blank
					>> face_param		[ phoenix::bind(
									// ... and here to update the face record
									[] (ast_obj& dat, const int p) {
										dat.faces.back().add(p);
									}, _val, _1
								) ]
				)
				>> (*encoding::blank >> qi::eol) // restrict to one definition per line
			)
			// 'capture' the rest and to nothing with it, meaning: ignore everything else
			| (encoding::char_)
		);

	}

};


///////////////////////////////////////////
//            parser actions             //
// return:                               //
//    std::tuple of                      //
// 	model_t,                         //
// 	       -> type of parse-result   //
// 	bool,  -> whether syntax was ok  //
// 	bool   -> whether semantics      //
// 	          were ok                //
///////////////////////////////////////////
template<typename It>
std::tuple<model_t, bool, bool> parse(It& f, It l) // note: first iterator gets updated
{
	static const skipper<It> s = {};
	static const obj_parser<It, skipper<It> > p;

	// the actual data used to create the returned end-result.
	model_t model;

	ast_obj result;
	try
	{
		if(!(qi::phrase_parse(f, l, p, s, result) && f == l))
		{
			return std::tuple<model_t, bool, bool>(model, false, false);
		}
	}
	catch(const qi::expectation_failure<It>& e)
	{
		std::cerr << e.what() << "'\n";
		return std::move(
			std::tuple<model_t, bool, bool>(
				std::move(model), false, false));
	}

	// syntax OK, semantics now
	// first: attempt some preformance increase by preliminary allocating enough space
	std::get<vertex_set_t>(model).reserve(result.verts.size());
	std::get<mesh_t>(model).reserve(result.faces.size());

	// use this to map the index of a vertex from the obj file to the index of the stored vertex
	std::unordered_map<int, index_t> vert_idx_mapping;

	int f_i = 1;
	index_t new_face[3]; // for re-use
	for(std::vector<ast_face>::const_iterator it = result.faces.begin();
		it != result.faces.end();
		it++, f_i++)
	{
		if((it -> n) != 3)
		{
			// too many or too few vertices in current face (ignore this face)
			std::cerr << "**Warning: too many or too few vertices in face " << f_i << " (ignoring this face)\n";
			continue;
		}

		int i = 0;
		for(; i < 3; i++)
		{

			if(abs(it -> num[i]) > result.verts.size())
			{
				// index out of bounds (ignore this face)
				std::cerr
					<< "**Warning: index "
					<< it -> num[i]
					<< " not pointing to valid vertex for face "
					<< f_i << "\n";
				break;
			}
			// calculate the absolute index (in the obj file, indexes are circular and 1-based)
			int idx = 
				it -> num[i] < 0
				? (result.faces.size() - (it -> num[i]))
				: (it -> num[i] - 1);

			// is the vertex already encountered?
			std::unordered_map<int, index_t>::iterator present_one = vert_idx_mapping.find(idx);
			if(present_one == vert_idx_mapping.end())
			{
				// nope, not encountered. Add vertice to local storage, and use that index.
				int local_idx = std::get<vertex_set_t>(model).size();
				std::get<vertex_set_t>(model).push_back(
					{ {
						result.verts[idx].num[0],
						result.verts[idx].num[1],
						result.verts[idx].num[2]
					} }
				);

				auto ins_or_not = vert_idx_mapping.emplace(idx, std::get<vertex_set_t>(model).size()-1);
				if(!ins_or_not.second)
				{
					// something went wrong with the insertion which was not supposed to happen. PANIC!
					std::cerr << "**Error: internal error when evaluating face " << f_i << "\n";
					std::cerr << "Aborting parsing process\n";
					return std::move(
						std::tuple<model_t, bool, bool>(
							std::move(model), false, false));
				}
				present_one = ins_or_not.first;
			}

			// keep the vertex index of the face
			new_face[i] = present_one -> second;

		}

		if(i != 3)
		{
			// some vertice indices are wrong (ignore this face)
			std::cerr
				<< "**Warning: ignoring face "
				<< f_i
				<< " due to aformentioned error(s)\n";
			continue;
		}

		// new face ok, put it into model
		std::get<mesh_t>(model).push_back(
			{ {
				new_face[0],
				new_face[1],
				new_face[2]
			} }
		);

	}

	// request to save up some space
	std::get<vertex_set_t>(model).shrink_to_fit();
	std::get<mesh_t>(model).shrink_to_fit();

	bool semantics_ok =
		std::get<mesh_t>(model).size() == result.faces.size()
		&& std::get<vertex_set_t>(model).size() == result.verts.size();
	// now construct the returning result from the faces/vertices arrays with size faces_num/vertices_num
	return std::move(
		std::tuple<model_t, bool, bool>(
			std::move(model),
			true, // syntax ok
			semantics_ok)); 
}

std::tuple<model_t, bool, bool> parse(std::istream& is)
{
	is.unsetf(std::ios::skipws);
	spirit::istream_iterator start(is);
	spirit::istream_iterator end;

	return parse(start, end);
}

#endif // PARSER_HEADER_FILE
