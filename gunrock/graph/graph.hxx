#pragma once

#include <cassert>
#include <tuple>

#include <gunrock/util/type_traits.hxx>

#include <gunrock/formats/formats.hxx>

#include <gunrock/graph/properties.hxx>
#include <gunrock/graph/vertex_pair.hxx>

#include <gunrock/graph/detail/graph_base.hxx>
#include <gunrock/graph/csr.hxx>
#include <gunrock/graph/csc.hxx>
#include <gunrock/graph/coo.hxx>


// #include <gunrock/algorithms/search/binary_search.cuh>

namespace gunrock {
namespace graph {

using namespace format;
using namespace detail;

// Variadic inheritence, inherit only what you need
template<typename vertex_t, typename edge_t, typename weight_t, class... graph_view_t> 
class graph_t : public graph_view_t... {

    using vertex_type = vertex_t;
    using edge_type   = edge_t;
    using weight_type = weight_t;

    using graph_base_type   = graph_base_t<vertex_type, edge_type, weight_type>;
    using graph_csr_type    = graph_csr_t<vertex_type, edge_type, weight_type>;
    using first_view_t      = typename std::tuple_element<0, // get first type
                                std::tuple<graph_view_t...> >::type;

    public:
        // XXX: add support for per-view based methods
        // template<typename view_t = first_view_t>
        __host__ __device__ __forceinline__
        edge_type get_neighbor_list_length(const vertex_type& v) const override {
            return first_view_t::get_neighbor_list_length(v);
        }

        template<typename csr_matrix_t>
        void from_csr_t(csr_matrix_t& _csr) {
            graph_base_type::_number_of_vertices = _csr.num_rows;
            graph_base_type::_number_of_edges = _csr.num_nonzeros;

            graph_csr_type::set(_csr.num_rows,
                                _csr.num_columns,
                                _csr.num_nonzeros,
                                _csr.row_offsets, 
                                _csr.column_indices, 
                                _csr.nonzero_values);
        }

        __host__ __device__ __forceinline__
        std::size_t number_of_graph_representations() const {
            return number_of_formats_inherited;
        }

        template<typename view_t>
        constexpr bool contains_representation() {
            return std::disjunction_v<std::is_same<view_t, graph_view_t>...>;
        }

    private:
        static constexpr std::size_t number_of_formats_inherited = sizeof...(graph_view_t);

};  // struct graph_t

/**
 * @brief Get the average degree of a graph.
 * 
 * @tparam graph_type 
 * @param graph 
 * @return double 
 */
template<typename graph_type>
double get_average_degree(graph_type &graph) {
  auto sum = 0;
  for (auto v = 0; v < graph.get_number_of_vertices(); ++v)
    sum += graph.get_neighbor_list_length(v);

  return (sum / graph.get_number_of_vertices());
}

/**
 * @brief Get the degree standard deviation of a graph.
 * This method uses population standard deviation,
 * therefore measuring the standard deviation over
 * the entire population (all nodes). This can be
 * sped up by only taking a small sample and using
 * sqrt(accum / graph.get_number_of_vertices() - 1)
 * as the result.
 * 
 * @tparam graph_type 
 * @param graph 
 * @return double 
 */
template <typename graph_type>
double get_degree_standard_deviation(graph_type &graph) {

  auto average_degree = get_average_degree(graph);

  double accum = 0.0;
  for (auto v = 0; v < graph.get_number_of_vertices(); ++v) {
    double d = graph.get_neighbor_list_length(v);
    accum += (d - average_degree) * (d - average_degree);
  }
  return sqrt(accum / graph.get_number_of_vertices());
}

/**
 * @brief build a log-scale degree histogram of a graph.
 * 
 * @tparam graph_type 
 * @tparam histogram_t 
 * @param graph 
 * @return histogram_t* 
 */
// template <typename graph_type, typename histogram_t>
// histogram_t* get_degree_histogram(graph_type &graph) {
//   using vertex_t = graph_type::vertex_t;
//   auto length = sizeof(vertex_t) * 8 + 1;

//   thrust::device_vector<vertex_t> histogram(length);

//   auto build_histogram = [graph] __device__ (vertex_t* counts, vertex_t i) {
//       auto degree = graph.get_neighbor_list_length(i);
//       while (num_neighbors >= (1 << log_length))
//         log_length++;

//       operation::atomic::add(&counts[log_length], (vertex_t)1);
//   };

//   auto begin = 0;
//   auto end = graph.get_number_of_vertices();
//   operator::for_all(thrust::device, histogram.data(), begin, end, build_histogram);
  
//   return histogram.data.get();
// }


} // namespace graph
} // namespace gunrock