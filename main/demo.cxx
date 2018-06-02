#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <numeric>
#include <map>

#include "types.hxx"
#include "bbox.hxx"
#include "../parser/parser.hxx"
#include "../algo/surface.hxx"
#include "../algo/volume.hxx"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

void to_cout(const std::vector<std::string> &v)
{
  std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>{
    std::cout, "\n"});
}

std::map<std::string, model_t> parse_files(const std::vector<std::string>& files)
{
	std::map<std::string, model_t> models;

	std::for_each(
		files.begin(),
		files.end(),
		[ &models ] (const std::string& patt)
		{
			if(fs::is_directory(patt))
			{			
				std::cerr << "Is a directory: '" << patt << "'\n";
				return;
			}
			if(fs::is_regular_file(patt))
			{

				std::ifstream in(patt);
				if(!in.is_open())
				{
					std::cerr << "Can't open '" << patt << "'\n";
					return;
				}

				auto res = parse(in);
				bool syntax = std::get<1>(res);
				bool semantics = std::get<2>(res);

				if(!(syntax && semantics && std::get<0>(res).mesh().size() > 0))
				{
					std::cerr << "Unable to extract model from '" << patt << "'\n";
				}

				models.emplace(
					fs::path(patt).filename().string(),
					std::move(std::get<0>(res)));
			}
		}
	);
	return std::move(models);
}

int main(int argc, const char *argv[])
{
	try
	{

		po::options_description desc{"Options"};
		desc.add_options()
			("help,h", "Help screen")
			("input,i",
		 		po::value<std::vector<std::string>>()
				->multitoken()
				->zero_tokens()
				->composing(), "Input file(s)");

		po::positional_options_description pos_desc;
		pos_desc.add("input", -1);
 
		po::command_line_parser parser{argc, argv};
		parser.options(desc).positional(pos_desc).allow_unregistered().style(
			po::command_line_style::default_style |
	 		po::command_line_style::allow_slash_for_short);
		po::parsed_options parsed_options = parser.run();

		po::variables_map vm;
		po::store(parsed_options, vm);
		po::notify(vm);

		std::map<std::string, model_t> models;
		if(vm.count("help"))
		{
			std::cout << desc << '\n';
		}
		else if(vm.count("input"))
		{
			models = parse_files(vm["input"].as<std::vector<std::string>>());
		}
		else
		{
			std::cout << "No input file(s) provided\n";
		}

		if(models.size() <= 0)
		{
			std::cout << "No models loaded.\n";
			return 0;
		}

		std::map<std::string, aabb> boxes;
		std::for_each(
			models.begin(),
			models.end(),
			[ &boxes ] (const std::pair<std::string, model_t>& mod)
			{
				aabb box(mod.second);
				std::cout << mod.first << ":\n";
				std::cout << " aabb: " << box << "\n";
				std::cout << " aabb surface: " << box.surface() << "\n";
				std::cout << " aabb volume: " << box.volume() << "\n";
				std::cout << " object's surface: "  << calculate_surface(mod.second) << ":\n";
				std::cout << " object's volume: "  << calculate_volume(mod.second) << ":\n";
				boxes.emplace(
					mod.first,
					std::move(box));
			}
		);

		if(boxes.size() <= 1)
		{
			std::cout << "Only one model given, no further comparisons performed\n";
			return 0;
		}

		for(auto it = boxes.begin(); it != boxes.end(); it++)
		{
			auto nxt = std::next(it);
			std::for_each(
				nxt, boxes.end(),
				[ &it ] (const std::pair<std::string, aabb>& it2)
				{
					const aabb& a = it -> second;
					const aabb& b = it2.second;
					std::cout << "\n";
					std::cout << "comparing '" << (it -> first) << "' (a) with '" << it2.first << "' (b):\n";
					aabb uni = (a | b);
					std::cout << " a union b: " << uni << "\n"; 
					std::cout << " a union b surface: " << uni.surface() << "\n"; 
					std::cout << " a union b volume: " << uni.volume() << "\n"; 
					aabb inr = (a & b);
					std::cout << " a intersection b: ";
					if(!inr)
					{
						std::cout << "empty\n";
					}
					else
					{
						std::cout << inr << "\n";
						std::cout << " a intersection b surface: " << inr.surface() << "\n"; 
						std::cout << " a intersection b volume: " << inr.volume() << "\n"; 
					}
					std::cout << " a equals b: " << std::boolalpha << (a == b) << "\n";
					std::cout << " a fully contains b: " << std::boolalpha << (a > b) << "\n";
					std::cout << " a fully contained by b: " << std::boolalpha << (a < b) << "\n";
					std::cout << " a contains b: " << std::boolalpha << (a >= b) << "\n";
					std::cout << " a contained by b: " << std::boolalpha << (a <= b) << "\n";
					std::cout << " a intersects b: " << std::boolalpha << (a && b) << "\n";
					std::cout << " a excludes b: " << std::boolalpha << (a || b) << "\n";
				}
			);
		}

		aabb full_uni = boxes.begin() -> second;
		aabb full_inr = boxes.begin() -> second;
		std::for_each(
			boxes.begin(), boxes.end(),
			[ &full_uni, &full_inr ] (const std::pair<std::string, aabb>& it)
			{
				full_uni |= it.second;
				if(!!full_inr)
				{
					full_inr &= it.second;
				}
			}
		);
		std::cout << "\n"; 
		std::cout << "union of all models: " << full_uni << "\n"; 
		std::cout << "intersection of all models: ";
		if(!full_inr)
		{
			std::cout << "empty\n";
		}
		else
		{
			std::cout << full_inr << "\n";
		}
	}
	catch (const po::error &ex)
	{
		std::cerr << ex.what() << '\n';
	}
}
