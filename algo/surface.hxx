#ifndef SURFACE_HEADER_FILE
#define SURFACE_HEADER_FILE

#include <cmath>

#include "../main/types.hxx"

const scalar_t calculate_surface(const model_t& m)
{
	return std::accumulate(
		m.mesh().begin(),
		m.mesh().end(),
		scalar_t(0),
		[ &m ] (scalar_t surf, const face_t& f)
		{
			vertex_t v1 = m.vertex_set()[f.a()];
			vertex_t v2 = m.vertex_set()[f.b()];
			vertex_t v3 = m.vertex_set()[f.c()];

			return surf + ((v1.to(v2)).cross(v1.to(v3))).norm() / 2;
			
		}
	);
}


#endif // SURFACE_HEADER_FILE
