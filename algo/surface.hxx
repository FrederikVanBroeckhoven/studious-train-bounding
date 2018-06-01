#ifndef SURFACE_HEADER_FILE
#define SURFACE_HEADER_FILE

#include <cmath>

#include "../main/types.hxx"

const scalar_t calculate_surface(const model_t& model)
{
	return std::accumulate(
		std::get<mesh_t>(model).begin(),
		std::get<mesh_t>(model).end(),
		scalar_t(0),
		[ &model ] (scalar_t surf, const face_t& f)
		{
			vertex_t v1 = std::get<vertex_set_t>(model)[f.a()];
			vertex_t v2 = std::get<vertex_set_t>(model)[f.b()];
			vertex_t v3 = std::get<vertex_set_t>(model)[f.c()];

			vertex_t sc = (v1.to(v2)).cross(v1.to(v3));
			return surf
				+ std::sqrt(
					(sc.x() * sc.x())
					+ (sc.y() * sc.y())
					+ (sc.z() * sc.z())) / 2;
			
		}
	);
}


#endif // SURFACE_HEADER_FILE
