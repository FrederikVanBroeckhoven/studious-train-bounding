#ifndef BOUNDING_HEADER_FILE
#define BOUNDING_HEADER_FILE

#include <limits>
#include <algorithm>
#include <utility>
#include <functional>
#include <initializer_list>

#include "../main/types.hxx"

template<typename It>
const range_t calculate_aabb(const It& begin, const It& end)
{
	range_t minmax(
		vertex_t( // minimum 
			std::numeric_limits<coord_t>::infinity()
		),
		vertex_t( // maximum
			-std::numeric_limits<coord_t>::infinity()
		));

	for(It it = begin; it != end; it++)
	{
		minmax.first = std::get<0>(minmax).min(*it);
		minmax.second = std::get<1>(minmax).max(*it);
	}
	return minmax;
}

const range_t calculate_aabb(std::initializer_list<vertex_t> list)
{
	return calculate_aabb(list.begin(), list.end());
}

const range_t calculate_aabb(const vertex_set_t& vertex_set)
{
	return calculate_aabb(vertex_set.begin(), vertex_set.end());
}

const range_t calculate_aabb(const model_t& model)
{
	return calculate_aabb(std::get<vertex_set_t>(model));
}

#endif // BOUNDING_HEADER_FILE


