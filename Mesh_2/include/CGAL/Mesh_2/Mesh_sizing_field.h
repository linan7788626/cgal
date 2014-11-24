
#ifndef CGAL_MESH_2_MESH_SIZING_FIELD_H
#define CGAL_MESH_2_MESH_SIZING_FIELD_H

#include <CGAL/Mesh_2/Sizing_field_2.h>

namespace CGAL {

namespace Mesh_2
{
/**
 * @class Mesh_sizing_field
 */
template <typename Tr, bool Need_vertex_update = true>
class Mesh_sizing_field
  : public virtual Sizing_field_2<Tr>
{
  // Types
  typedef typename Tr::Geom_traits   Gt;
  typedef typename Tr::Point         Point_2;
  typedef typename Gt::FT            FT;

  typedef typename Tr::Vertex_handle      Vertex_handle;
  typedef typename Tr::Face_handle        Face_handle;
  typedef typename Tr::Edge               Edge;
  
public:
  // update vertices of mesh triangulation ? 
  static const bool is_vertex_update_needed = Need_vertex_update;
  
public:
  /**
   * Constructor
   */
  Mesh_sizing_field(Tr& tr)
  : tr_(tr)
  , last_face_()
  {
    init();
  }

  /**
   * Returns size at point \c p.
   */
  FT operator()(const Point_2& p) const
  { return this->operator()(p, last_face_); }

  /**
   * Returns size at point \c p, using \c v to accelerate \c p location
   * in triangulation
   */
  FT operator()(const Point_2& p, const Vertex_handle& v) const
  { return this->operator()(p, v->face()); }
  
  /**
   * Returns size at point \c p.
   */
  FT operator()(const Point_2& p, const Face_handle& c) const
  {
    const Face_handle fh = tr_.locate(p,c);
    last_face_ = fh;
  
    if ( !tr_.is_infinite(fh) )
      return interpolate_on_face_vertices(p,fh);
    else
      return interpolate_on_edge_vertices(p,fh);
  }

  /**
   * Fill sizing field with actual size inside the triangulation
   */
  void init()
  {
    for(typename Tr::Finite_vertices_iterator
        vit = tr_.finite_vertices_begin() ;
        vit != tr_.finite_vertices_end() ;
        ++vit )
    {
      vit->set_sizing_info(average_incident_edge_length(vit));
    }
  }

private:
  /**
   * Returns size at point \c p, by interpolation inside facet
   */
  FT interpolate_on_face_vertices(const Point_2& p,
                                  const Face_handle& f) const
  {
    // Interpolate value using tet vertices values
    const FT& sa = f->vertex(0)->sizing_info();
    const FT& sb = f->vertex(1)->sizing_info();
    const FT& sc = f->vertex(2)->sizing_info();
    return ( (sa + sb + sc) / 3. );
  }

  /**
   * Returns size at point \c p, by interpolation inside edge
   * (\c e f is assumed to be an infinite face)
   */
  FT interpolate_on_edge_vertices(const Point_2& p,
                                  const Face_handle& f) const
  {
    return 1.;//todo
  }

  FT average_incident_edge_length(const Vertex_handle& v) const
  {
    typename Tr::Edge_circulator ec = tr_.incident_edges(v);
    typename Tr::Edge_circulator end = ec;

    FT sum_len(0.);
    FT nb = 0.;
    do
    {
      Edge e = *ec;
      if(tr_.is_infinite(e))
        continue;

      Face_handle f1 = e.first;
      Face_handle f2 = e.first->neighbor(e.second);
      if(f1->is_in_domain() || f2->is_in_domain())
      {
        sum_len += length(e);
        ++nb;
      }
    }
    while(++ec != end);
    // nb == 0 could happen if there is an isolated point.
    if( 0 != nb )
      return sum_len/nb;
    else
     // Use outside faces to compute size of point
      return 1.;//todo
  }

  FT length(const Edge& e) const
  {
    Point_2 p1 = e.first->vertex(Tr::cw(e.second))->point();
    Point_2 p2 = e.first->vertex(Tr::ccw(e.second))->point();
    return CGAL::sqrt(CGAL::squared_distance(p1, p2));
  }
  
private:
  /// The triangulation
  Tr& tr_;
  /// A face_handle that is used to accelerate location queries
  mutable Face_handle last_face_;
};

} // end namespace Mesh_2

} //namespace CGAL

#endif // CGAL_MESH_2_MESH_SIZING_FIELD_H
