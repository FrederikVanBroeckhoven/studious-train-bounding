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
	return std::accumulate(
		begin, end,
		range_t(
			vertex_t(std::numeric_limits<coord_t>::infinity()),
			vertex_t(-std::numeric_limits<coord_t>::infinity())
		),
		[] (range_t minmax, const vertex_t& v)
		{
			return std::move(
				range_t(
					std::move(std::get<0>(minmax).min(v)),
					std::move(std::get<1>(minmax).max(v))
				)
			);
		}
	);
}

inline const range_t calculate_aabb(std::initializer_list<vertex_t> il)
{
	return std::move(calculate_aabb(il.begin(), il.end()));
}

inline const range_t calculate_aabb(const vertex_set_t& vs)
{
	return std::move(calculate_aabb(vs.begin(), vs.end()));
}

inline const range_t calculate_aabb(const model_t& m)
{
	return std::move(calculate_aabb(m.vertex_set()));
}

#endif // BOUNDING_HEADER_FILE


