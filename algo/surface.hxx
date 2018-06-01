#ifndef SURFACE_HEADER_FILE
#define SURFACE_HEADER_FILE

#include <cmath>

#include "../main/types.hxx"

const scalar_t calculate_surface(const model_t& model)
{
	scalar_t surf = 0;
	int i = 0;
	for(mesh_t::const_iterator it = std::get<mesh_t>(model).begin(); it != std::get<mesh_t>(model).end(); it++, i++)
	{
		vertex_t v1 = std::get<vertex_set_t>(model)[it -> a()];
		vertex_t v2 = std::get<vertex_set_t>(model)[it -> b()];
		vertex_t v3 = std::get<vertex_set_t>(model)[it -> c()];

		vertex_t sc = (v1.to(v2)).cross(v1.to(v3));
		scalar_t ts =	
			std::sqrt((sc.x() * sc.x())
			+ (sc.y() * sc.y())
			+ (sc.z() * sc.z())) / 2;

		surf += ts;
	}
	return surf;
}


#endif // SURFACE_HEADER_FILE
