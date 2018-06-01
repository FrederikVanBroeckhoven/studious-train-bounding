#ifndef BBOX_HEADER_FILE
#define BBOX_HEADER_FILE

#include <initializer_list>

#include "types.hxx"
#include "../algo/bounding.hxx"

class aabb // axis-aligned bounding box
{
private:
	range_t data;
	aabb(const range_t& range) : data(range) {}

	// helper function for intersection calculation (axis-intersection)
	//   if b.min > min (otherwise swap)
	//     if b.min < max (otherwise empty; no need for extra check though, it will lead to result where min > max anyway)
	//      -> (b.min, max)
	//
	//      +-------------+
	//              +-------------+
	//      ^       ^     ^
	//     min    b.min  max    b.max
	//
	inline static const std::pair<coord_t, coord_t> axis_int(
		const coord_t& min,
		const coord_t& bmin,
		const coord_t& max,
		const coord_t& bmax)
	{
		return std::move(
			(bmin > min)
			? std::make_pair(bmin, max)
			: std::make_pair(min, bmax));
	}

public:
	aabb(const model_t& model) : data(calculate_aabb(model)) {}
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
	const bool operator>(const model_t& model) const
	{
		return std::all_of(
			std::get<vertex_set_t>(model).begin(),
			std::get<vertex_set_t>(model).end(),
			[this] (const vertex_t& v) { return *this > v; }
		);
	}
	const bool operator>(const aabb& box) const
	{ return *this > min() && *this > max(); }

	// contains or equal
	const bool operator>=(const aabb& box) const
	{ return *this == box || (*this > min() && *this > max()); }

	// part-of
	const bool operator<(const aabb& box) const
	{ return box > *this; }

	// part-of or equal
	const bool operator<=(const aabb& box) const
	{ return box >= *this; }

	// intersects
	const bool operator&&(const aabb& box) const
	{ return (*this > box.min()) ^ (*this > box.max()); }
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
/*	const aabb operator+(const aabb& box) const
	{ return *this | box; }
	const aabb operator+(const vertex_t& v) const
	{ return *this | v; }
	const aabb operator+(const vertex_set_t& vs) const
	{ return *this | vs; }*/

	// intersection:
	const aabb operator&(const aabb& box) const
	{
		std::pair<coord_t, coord_t> ix =
			axis_int(min().x(), box.min().x(), max().x(), box.max().x());
		std::pair<coord_t, coord_t> iy =
			axis_int(min().y(), box.min().y(), max().y(), box.max().y());
		std::pair<coord_t, coord_t> iz =
			axis_int(min().z(), box.min().z(), max().z(), box.max().z());
		return aabb(range_t(
			{ { ix.first, iy.first, iz.first } },
			{ { ix.second, iy.second, iz.second } } ));
	}

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

};


#endif // BBOX_HEADER_FILE
