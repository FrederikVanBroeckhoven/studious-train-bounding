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

	const bool operator==(const aabb& box) const { return false; /*box.data == data*/; }
	const bool operator!=(const aabb& box) const { return !(*this == box); }

	// contains
	const bool operator>(const vertex_t& vert) const
	{
		return vert.x() > min_x() && vert.y() > min_y() && vert.z() > min_z()
			&& vert.x() < max_x() && vert.y() < max_y() && vert.z() < max_z();

	}
	const bool operator>(const model_t& model) const
	{
		for(mesh_t::const_iterator it = std::get<mesh_t>(model).begin(); it != std::get<mesh_t>(model).end(); it++)
		{
			if(!(*this > std::get<vertex_set_t>(model)[it -> a()]
				&& *this > std::get<vertex_set_t>(model)[it -> b()]
				&& *this > std::get<vertex_set_t>(model)[it -> c()]))
			{
				return false;
			}
		}
		return true;
	}
	const bool operator>(const aabb& box) const
	{ return *this > min() && *this > max(); }

	// part-of
	const bool operator<(const aabb& box) const {
		return box > (*this);
	}

	// intersects
	const bool operator&&(const aabb& box) const { return false; }
	// excludes
	const bool operator||(const aabb& box) const { return false; }
	
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
