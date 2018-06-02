#ifndef BBOX_HEADER_FILE
#define BBOX_HEADER_FILE

#include <initializer_list>
#include <iostream>

#include "types.hxx"
#include "../algo/bounding.hxx"

class aabb // axis-aligned bounding box
{
private:
	range_t data;
	aabb(const range_t& range) : data(range) {}

	inline static bool does_axis_int(
		const coord_t& min,
		const coord_t& bmin,
		const coord_t& max,
		const coord_t& bmax)
	{
		return (bmin >= min && bmin <= max)
			|| (bmax >= min && bmax <= max)
			|| (min >= bmin && min <= bmax)
			|| (max >= bmin && max <= bmax); 
	}

public:
	aabb(const model_t& mod) : data(calculate_aabb(mod)) {}
	aabb(const vertex_set_t& vertex_set) : data(calculate_aabb(vertex_set)) {}
	aabb(std::initializer_list<vertex_t>& list) : data(calculate_aabb(list)) {}
	aabb(const aabb& box) : data(box.data) {}
	~aabb() {}

	const aabb& operator=(const aabb& box) { data = box.data; }

	const bool operator==(const aabb& box) const { return box.data == data; }
	const bool operator!=(const aabb& box) const { return !(*this == box); }

	// contains
	const bool operator>(const vertex_t& vert) const
	{
		return vert.x() > min_x()
			&& vert.y() > min_y()
			&& vert.z() > min_z()
			&& vert.x() < max_x()
			&& vert.y() < max_y()
			&& vert.z() < max_z();
	}
	const bool operator>(const vertex_set_t& vert) const
	{
		return std::all_of(
			vert.begin(),
			vert.end(),
			[this] (const vertex_t& v) { return *this > v; }
		);
	}
	const bool operator>(const model_t& mod) const
	{ return *this > mod.vertex_set(); }
	const bool operator>(const aabb& box) const
	{ return *this > box.min() && *this > box.max(); }

	// contains or equal
	const bool operator>=(const aabb& box) const
	{ return *this == box || (*this > box.min() && *this > box.max()); }

	// contained
	const bool operator<(const aabb& box) const
	{ return box > *this; }

	// contained or equal
	const bool operator<=(const aabb& box) const
	{ return box >= *this; }

	// intersects
	const bool operator&&(const aabb& box) const
	{
		return does_axis_int(min().x(), box.min().x(), max().x(), box.max().x())
			&& does_axis_int(min().y(), box.min().y(), max().y(), box.max().y())
			&& does_axis_int(min().z(), box.min().z(), max().z(), box.max().z());
	}
	// excludes
	const bool operator||(const aabb& box) const
	{ return !(*this && box); }
	
	// union
	const aabb operator|(const aabb& box) const
	{ return aabb( { min(), max(), box.min(), box.max() } ); }
	const aabb operator|(const vertex_t& v) const
	{ return aabb( { min(), max(), v } ); }
	const aabb operator|(const vertex_set_t& vs) const
	{ return *this | aabb(vs); }
	// union-assign
	const aabb& operator|=(const aabb& box)
	{ *this = (*this | box); return *this; }
	const aabb& operator|=(const vertex_t& v)
	{ *this = (*this | v); return *this; }
	const aabb& operator|=(const vertex_set_t& vs)
	{ *this = (*this | vs); return *this; }

	// intersection
	const aabb operator&(const aabb& box) const
	{
		vertex_t mn = { {
			std::max(min().x(), box.min().x()),
			std::max(min().y(), box.min().y()),
			std::max(min().z(), box.min().z())
		} };
		vertex_t mx = { {
			std::min(max().x(), box.max().x()),
			std::min(max().y(), box.max().y()),
			std::min(max().z(), box.max().z())
		} };
		return aabb(range_t(mn, mx));
	}
	// intersection-assign
	const aabb& operator&=(const aabb& box)
	{ *this = (*this & box); return *this; }

	// is empty
	const bool operator!() const
	{
		return min_x() >= max_x()
			|| min_y() >= max_y()
			|| min_z() >= max_z();
	}

	const range_t& range() const { return data; };

	const vertex_t& min() const { return std::get<0>(data); };
	const vertex_t& max() const { return std::get<1>(data); };

	const coord_t min_x() const { return min().x(); }
	const coord_t min_y() const { return min().y(); }
	const coord_t min_z() const { return min().z(); }

	const coord_t max_x() const { return max().x(); }
	const coord_t max_y() const { return max().y(); }
	const coord_t max_z() const { return max().z(); }

	friend std::ostream& operator<<(std::ostream& o, const aabb& bbox)
	{ return o << "[" << bbox.min() << " - " << bbox.max() << "]"; }

};


#endif // BBOX_HEADER_FILE
