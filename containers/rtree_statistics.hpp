#include <algorithm>
#include <tuple>

#include <boost/geometry/index/detail/rtree/node/node_elements.hpp>
#include <boost/geometry/index/detail/rtree/node/variant_visitor.hpp>
#include <boost/geometry/index/detail/rtree/utilities/view.hpp>

namespace boost { namespace geometry { namespace index { namespace detail { namespace rtree { namespace utilities {

namespace visitors {

template <typename MembersHolder>
struct statistics
    : public MembersHolder::visitor_const
{
    typedef typename MembersHolder::internal_node internal_node;
    typedef typename MembersHolder::leaf leaf;

    inline statistics()
        : level(0)
        , levels(1) // count root
        , nodes(0)
        , leaves(0)
        , values(0)
        , values_min(0)
        , values_max(0)
        , size(0)
    {}

    inline void operator()(internal_node const& n)
    {
        typedef typename rtree::elements_type<internal_node>::type elements_type;
        elements_type const& elements = rtree::elements(n);

        ++nodes; // count node

        size_t const level_backup = level;
        ++level;

        levels += level++ > levels ? 1 : 0; // count level (root already counted)

        for (typename elements_type::const_iterator it = elements.begin();
            it != elements.end(); ++it)
        {
            rtree::apply_visitor(*this, *it->second);
        }
        size += elements.size() * sizeof(elements[0]);

        level = level_backup;
    }

    inline void operator()(leaf const& n)
    {
        typedef typename rtree::elements_type<leaf>::type elements_type;
        elements_type const& elements = rtree::elements(n);

        ++leaves; // count leaves

        std::size_t const v = elements.size();
        // count values spread per node and total
        values_min = (std::min)(values_min == 0 ? v : values_min, v);
        values_max = (std::max)(values_max, v);
        values += v;
        size += v * sizeof(elements[0]);
    }

    std::size_t level;
    std::size_t levels;
    std::size_t nodes;
    std::size_t leaves;
    std::size_t values;
    std::size_t values_min;
    std::size_t values_max;
    std::size_t size;
};

} // namespace visitors

template <typename Rtree> inline
std::tuple<std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t>
statistics(Rtree const& tree)
{
    typedef utilities::view<Rtree> RTV;
    RTV rtv(tree);

    visitors::statistics<
        typename RTV::members_holder
    > stats_v;

    rtv.apply_visitor(stats_v);

    return std::make_tuple(stats_v.levels, stats_v.nodes, stats_v.leaves, stats_v.values, stats_v.values_min, stats_v.values_max, stats_v.size);
}

}}}}}} // namespace boost::geometry::index::detail::rtree::utilities
