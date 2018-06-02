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

const scalar_t calculate_volume(const model_t& m)
{
	return std::accumulate(
		m.mesh().begin(),
	       	m.mesh().end(),
		scalar_t(0),
		[ &m ] (scalar_t vol, const face_t& f)
		{
			return vol + signed_volume(
				m.vertex_set()[f.a()],
				m.vertex_set()[f.b()],
				m.vertex_set()[f.c()]);
		}
	);
}


#endif // VOLUME_HEADER_FILE
