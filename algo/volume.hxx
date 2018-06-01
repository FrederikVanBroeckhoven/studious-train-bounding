#ifndef VOLUME_HEADER_FILE
#define VOLUME_HEADER_FILE

#include <cmath>

#include "../main/types.hxx"
#include "surface.hxx"


scalar_t signed_volume(const vertex_t& p1, const vertex_t& p2, const vertex_t& p3)
{
	return (p2.x() * p3.y() * p1.z()
		+ p3.x() * p1.y() * p2.z()
		+ p1.x() * p2.y() * p3.z()
		- p1.x() * p3.y() * p2.z()
		- p2.x() * p1.y() * p3.z()
		- p3.x() * p2.y() * p1.z()) / 6;

}

const scalar_t calculate_volume(const model_t& model)
{
	scalar_t vol = 0;
	int i = 0;
	for(mesh_t::const_iterator it = std::get<mesh_t>(model).begin(); it != std::get<mesh_t>(model).end(); it++, i++)
	{
		vertex_t v1 = std::get<vertex_set_t>(model)[it -> a()];
		vertex_t v2 = std::get<vertex_set_t>(model)[it -> b()];
		vertex_t v3 = std::get<vertex_set_t>(model)[it -> c()];

		vol += signed_volume(v1, v2, v3);
	}
	return vol;
}


#endif // VOLUME_HEADER_FILE
