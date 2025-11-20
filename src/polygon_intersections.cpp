#include <vector>
#include <utility>
#include <set>
#include <iterator>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_linear_traits_2.h>
#include <CGAL/Arrangement_2.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

// ----------------------
// CGAL typedefs
// ----------------------
typedef CGAL::Exact_predicates_exact_constructions_kernel   Kernel;
typedef CGAL::Arr_linear_traits_2<Kernel>                   Traits_2;
typedef CGAL::Arrangement_2<Traits_2>                       Arrangement_2;

typedef Traits_2::Point_2                Point_2;
typedef Traits_2::X_monotone_curve_2     Curve_2;
typedef Kernel::Segment_2                Segment_2;
typedef Kernel::Line_2                   Line_2;

typedef Arrangement_2::Vertex_handle     Vertex_handle;
typedef Arrangement_2::Halfedge_handle   Halfedge_handle;



int count_intersections_with_diag(const std::vector<std::pair<double,double>>& vertices)
{
    Arrangement_2 arr;

    // Insert polygon edges
    std::vector<Curve_2> curves;
    curves.reserve(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        size_t j = (i + 1) % vertices.size();
        Point_2 p(vertices[i].first, vertices[i].second);
        Point_2 q(vertices[j].first, vertices[j].second);
        curves.emplace_back(Segment_2(p, q));
    }
    CGAL::insert(arr, curves.begin(), curves.end());

    // Diagonal y = x  => x - y = 0
    Line_2 diag(1, -1, 0);
    Curve_2 diag_curve(diag);

    // zone
    std::vector<CGAL::Object> zone_elems;
    CGAL::zone(arr, diag_curve, std::back_inserter(zone_elems));

    // Dedup sets
    std::set<Vertex_handle>    seen_vertices;
    std::set<Halfedge_handle>  seen_edges;
    std::set<Halfedge_handle>  coincident_edges;

    auto is_on_diag = [](const Point_2& p) {
        return (p.x() == p.y());
    };

    //collect everything 
    for (const auto& obj : zone_elems)
    {
        // Vertex
        Vertex_handle vh;
        if (CGAL::assign(vh, obj))
        {
            const Point_2& p = vh->point();
            if (is_on_diag(p))
                seen_vertices.insert(vh);
            continue;
        }

        // Half-Edge
        Halfedge_handle hh;
        if (CGAL::assign(hh, obj))
        {
            Halfedge_handle canon =
                (hh->direction() == CGAL::ARR_LEFT_TO_RIGHT ? hh : hh->twin());

            if (seen_edges.insert(canon).second)
            {
                const Point_2& s = canon->source()->point();
                const Point_2& t = canon->target()->point();

                if (is_on_diag(s) && is_on_diag(t))
                {
                    coincident_edges.insert(canon);
                }
            }
            continue;
        }
    }

    // remove endpoints of coincident edges --------
    for (Halfedge_handle he : coincident_edges)
    {
        seen_vertices.erase(he->source());
        seen_vertices.erase(he->target());
    }

    // Final count
    return int(seen_vertices.size() + seen_edges.size());
}


//                  PYBIND11 MODULE

PYBIND11_MODULE(polygon_intersections, m)
{
    m.doc() = "CGAL polygon intersection counter";
    m.def("count_intersections_with_diag",
          &count_intersections_with_diag,
          py::arg("vertices"));
}
