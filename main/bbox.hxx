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

	aabb(const range_t& range);

	inline static bool does_axis_int(
		const coord_t& min,
		const coord_t& bmin,
		const coord_t& max,
		const coord_t& bmax);
public:
	aabb(const model_t& mod);
	aabb(const vertex_set_t& vertex_set);
	aabb(std::initializer_list<vertex_t>& list);
	aabb(const aabb& box);
	~aabb();

	const aabb& operator=(const aabb& box);

	const bool operator==(const aabb& box) const;
	const bool operator!=(const aabb& box) const;

	// contains
	const bool operator>(const vertex_t& vert) const;
	const bool operator>(const vertex_set_t& vert) const;
	const bool operator>(const model_t& mod) const;
	const bool operator>(const aabb& box) const;

	// contains or equal
	const bool operator>=(const aabb& box) const;

	// contained
	const bool operator<(const aabb& box) const;

	// contained or equal
	const bool operator<=(const aabb& box) const;

	// intersects
	const bool operator&&(const aabb& box) const;
	// excludes
	const bool operator||(const aabb& box) const;
	
	// union
	const aabb operator|(const aabb& box) const;
	const aabb operator|(const vertex_t& v) const;
	const aabb operator|(const vertex_set_t& vs) const;
	// union-assign
	const aabb& operator|=(const aabb& box);
	const aabb& operator|=(const vertex_t& v);
	const aabb& operator|=(const vertex_set_t& vs);

	// intersection
	const aabb operator&(const aabb& box) const;
	// intersection-assign
	const aabb& operator&=(const aabb& box);

	// is empty
	const bool operator!() const;

	const range_t& range() const;

	const vertex_t& min() const;
	const vertex_t& max() const;

	const coord_t min_x() const;
	const coord_t min_y() const;
	const coord_t min_z() const;

	const coord_t max_x() const;
	const coord_t max_y() const;
	const coord_t max_z() const;

	const scalar_t len_x() const;
	const scalar_t len_y() const;
	const scalar_t len_z() const;

	const scalar_t volume() const;
	const scalar_t surface() const;

	friend std::ostream& operator<<(std::ostream& o, const aabb& bbox);

};

#endif // BBOX_HEADER_FILE

