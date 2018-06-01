#ifndef BBOX_HEADER_FILE
#define BBOX_HEADER_FILE

#include <initializer_list>

#include "types.hxx"
#include "../algo/bounding.hxx"

class aabb // axis-aligned bounding box
{
private:
	range_t data;
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
	// intersection
	const aabb operator&(const aabb& box) const { return *this; }

	const range_t& range() const { return data; };

	const vertex_t& min() const { return std::get<0>(data); };
	const vertex_t& max() const { return std::get<1>(data); };

	const coord_t min_x() const { return min().x(); }
	const coord_t min_y() const { return min().y(); }
	const coord_t min_z() const { return min().z(); }

	const coord_t max_x() const { return max().x(); }
	const coord_t max_y() const { return max().y(); }
	const coord_t max_z() const { return max().z(); }

//	range_t& range() { return range; };
};


#endif // BBOX_HEADER_FILE
