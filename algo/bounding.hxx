#ifndef BOUNDING_HEADER_FILE
#define BOUNDING_HEADER_FILE

#include <limits>
#include <algorithm>
#include <utility>
#include <functional>

#include "../main/types.hxx"

const range_t calculate_aabb(const model_t& model)
{
	range_t minmax(
		vertex_t( // minimum 
			std::numeric_limits<coord_t>::infinity()
		),
		vertex_t( // maximum
			-std::numeric_limits<coord_t>::infinity()
		));

	int i = 0;
	for(mesh_t::const_iterator it = std::get<mesh_t>(model).begin(); it != std::get<mesh_t>(model).end(); it++, i++)
	{
		vertex_t a = std::get<vertex_set_t>(model)[it -> a()];
		minmax.first = std::get<0>(minmax).min(a);
		minmax.second = std::get<1>(minmax).max(a);

		vertex_t b = std::get<vertex_set_t>(model)[it -> b()];
		minmax.first = std::get<0>(minmax).min(b);
		minmax.second = std::get<1>(minmax).max(b);

		vertex_t c = std::get<vertex_set_t>(model)[it -> c()];
		minmax.first = std::get<0>(minmax).min(c);
		minmax.second = std::get<1>(minmax).max(c);
	}
	return minmax;
}

#endif // BOUNDING_HEADER_FILE


