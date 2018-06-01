#ifndef TYPES_HEADER_FILE
#define TYPES_HEADER_FILE

#include <vector>
#include <tuple>
#include <utility>

// generic vertex wrapper

template <typename CT, typename VT, typename SuperT>
struct base_vertex3d
{
public:
	VT coord;

	base_vertex3d(const VT& crd) : coord(crd) {}
	base_vertex3d(const CT& crd) : coord( { crd, crd, crd } ) {}

	typedef SuperT super_t;

	template<typename VT2>
	const super_t cross(const VT2& v) const
	{
		const super_t& u = *(static_cast<const SuperT*>(this));
		return { { // assume coords can be initialized with init-lists
			u.y() * v.z() - u.z() * v.y(),
			u.x() * v.z() - u.z() * v.x(),
			u.x() * v.y() - u.y() * v.x()
		} };
	}

	template<typename VT2>
	const super_t to(const VT2& v) const // calculates vector from this to v
	{
		const super_t& u = *(static_cast<const SuperT*>(this));
		return { { // assume coords can be initialized with init-lists
			v.x() - u.x(),
			v.y() - u.y(),
			v.z() - u.z()
		} };
	}

	template<typename VT2>
	const super_t min(const VT2& v) const // calculates vertex with minimal coordinates
	{
		const super_t& u = *(static_cast<const SuperT*>(this));
		return { { // assume coords can be initialized with init-lists
			std::min(v.x(), u.x()),
			std::min(v.y(), u.y()),
			std::min(v.z(), u.z())
		} };
	}

	template<typename VT2>
	const super_t max(const VT2& v) const // calculates vertex with maximal coordinates
	{
		const super_t& u = *(static_cast<const SuperT*>(this));
		return { { // assume coords can be initialized with init-lists
			std::max(v.x(), u.x()),
			std::max(v.y(), u.y()),
			std::max(v.z(), u.z())
		} };
	}

};

template <typename CT2, typename VT2, typename SuperT2>
static std::ostream& operator<<(std::ostream& o, const base_vertex3d<CT2, VT2, SuperT2>& bv)
{
	const SuperT2& v = *(static_cast<const SuperT2*>(&bv));
	return o << "(" << v.x() << "," << v.y() << "," << v.z() << ")";
}

template <typename CT, typename VT>
struct vertex3d : public base_vertex3d<CT, VT, vertex3d<CT, VT>>
{
public:
	typedef base_vertex3d<CT, VT, vertex3d<CT, VT>> base_t;

	template<typename CT2>
	vertex3d(const CT2& crd) : base_t(crd) {}

	const CT& x() const { return base_t::coord[0]; }
	const CT& y() const { return base_t::coord[1]; }
	const CT& z() const { return base_t::coord[2]; }
	CT& x() { return base_t::coord[0]; }
	CT& y() { return base_t::coord[1]; }
	CT& z() { return base_t::coord[2]; }
};

template <typename CT>
struct vertex3d<CT, std::tuple<CT, CT, CT>>
: public base_vertex3d<CT, std::tuple<CT, CT, CT>, vertex3d<CT, std::tuple<CT, CT, CT>>>
{
public:
	typedef base_vertex3d<CT, std::tuple<CT, CT, CT>, vertex3d<CT, std::tuple<CT, CT, CT>>> base_t;

	vertex3d(const std::tuple<CT, CT, CT>& crd) : base_t(crd) {}
	vertex3d(const CT& crd) : base_t(crd) {}

	const CT& x() const { return std::get<0>(base_t::coord); }
	const CT& y() const { return std::get<1>(base_t::coord); }
	const CT& z() const { return std::get<2>(base_t::coord); }
	CT& x() { return std::get<0>(base_t::coord); }
	CT& y() { return std::get<1>(base_t::coord); }
	CT& z() { return std::get<2>(base_t::coord); }
};

// generic tri_face wrapper

template <typename PT, typename FT, typename SuperT>
struct base_tri_face
{
public:
	FT pnts;

	base_tri_face(const FT& crd) : pnts(crd) {}

	typedef SuperT super_t;

	// this could be made more elaborate by including functions
	// to calculate surface, signed volume etc..., but this requires
	// deeper knowledge of type of PT (embedded scalar type, wether PT
	// is an index or an actual vertex, etc...)
	// For this simple case, those calculations are done on a more
	// specific level
};

template <typename PT, typename FT>
struct tri_face : public base_tri_face<PT, FT, tri_face<PT, FT>>
{
public:
	typedef base_tri_face<PT, FT, tri_face<PT, FT>> base_t;

	tri_face(const FT& crd) : base_t(crd) {}

	const PT& a() const { return base_t::pnts[0]; }
	const PT& b() const { return base_t::pnts[1]; }
	const PT& c() const { return base_t::pnts[2]; }
	PT& a() { return base_t::pnts[0]; }
	PT& b() { return base_t::pnts[1]; }
	PT& c() { return base_t::pnts[2]; }
};

template <typename PT>
struct tri_face<PT, std::tuple<PT, PT, PT>>
: public base_tri_face<PT, std::tuple<PT, PT, PT>, tri_face<PT, std::tuple<PT, PT, PT>>>
{
public:
	typedef base_tri_face<PT, std::tuple<PT, PT, PT>, tri_face<PT, std::tuple<PT, PT, PT>>> base_t;

	tri_face(const std::tuple<PT, PT, PT>& crd) : base_t(crd) {}

	const PT& a() const { return std::get<0>(base_t::pnts); }
	const PT& b() const { return std::get<1>(base_t::pnts); }
	const PT& c() const { return std::get<2>(base_t::pnts); }
	PT& a() { return std::get<0>(base_t::pnts); }
	PT& b() { return std::get<1>(base_t::pnts); }
	PT& c() { return std::get<2>(base_t::pnts); }
};

typedef double							scalar_t;
typedef scalar_t						coord_t;
typedef size_t							index_t;

typedef vertex3d<coord_t, std::tuple<coord_t, coord_t, coord_t>>
								vertex_t;
typedef tri_face<index_t, std::tuple<index_t, index_t, index_t>>
								face_t;
typedef std::pair<vertex_t, vertex_t>				range_t;
typedef std::vector<vertex_t>					vertex_set_t;
typedef std::vector<face_t>					mesh_t;
typedef std::pair<vertex_set_t, mesh_t>				model_t;

#endif // TYPES_HEADER_FILE
