/*
 * GraphBLAS Template Library, Version 2.0
 *
 * Copyright 2018 Carnegie Mellon University, Battelle Memorial Institute, and
 * Authors. All Rights Reserved.
 *
 * THIS MATERIAL WAS PREPARED AS AN ACCOUNT OF WORK SPONSORED BY AN AGENCY OF
 * THE UNITED STATES GOVERNMENT.  NEITHER THE UNITED STATES GOVERNMENT NOR THE
 * UNITED STATES DEPARTMENT OF ENERGY, NOR THE UNITED STATES DEPARTMENT OF
 * DEFENSE, NOR CARNEGIE MELLON UNIVERSITY, NOR BATTELLE, NOR ANY OF THEIR
 * EMPLOYEES, NOR ANY JURISDICTION OR ORGANIZATION THAT HAS COOPERATED IN THE
 * DEVELOPMENT OF THESE MATERIALS, MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LEGAL LIABILITY OR RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS,
 * OR USEFULNESS OR ANY INFORMATION, APPARATUS, PRODUCT, SOFTWARE, OR PROCESS
 * DISCLOSED, OR REPRESENTS THAT ITS USE WOULD NOT INFRINGE PRIVATELY OWNED
 * RIGHTS..
 *
 * Released under a BSD (SEI)-style license, please see license.txt or contact
 * permission@sei.cmu.edu for full terms.
 *
 * This release is an update of:
 *
 * 1. GraphBLAS Template Library (GBTL)
 * (https://github.com/cmu-sei/gbtl/blob/master/LICENSE) Copyright 2015 Carnegie
 * Mellon University and The Trustees of Indiana. DM17-0037, DM-0002659
 *
 * DM18-0559
 */

#ifndef GBTL_SSSP_HPP
#define GBTL_SSSP_HPP

#include <functional>
#include <limits>
#include <tuple>

#include <graphblas/graphblas.hpp>

namespace algorithms
{
    /**
     * @brief Compute the lenghts of the single source shortest path(s) of
     *        one specified starting vertex in the given graph.
     *
     * The algorithm used in the implementation is the Floyd-Warshall
     * algorithm.  It is a dynamic programming solution (similar to the
     * Bellman-Ford algorithm).  The algorithm (like Bellman-Ford) is
     * based off of relaxation.  At each iteration in the algorithm, the
     * approximate lengths of the paths are always overestimates.  The
     * edges are relaxed at each iteration, until the lenghts are no
     * longer overestimates, at which point the algorithm terminates and
     * the path lengths are returned.
     *
     * The start can be specified multiple ways.  It can be in one
     * of the following three formats:
     *
     * <ol>
     * <li>The vertex itself (to specify only one vertex).</li>
     * <li>As a \f$N\f$ by \f$1\f$ column vector, where if the \f$i^{th}\f$
     * index is \f$1\f$, include that vertex in the set of vertices to
     * compute single source shortest paths for.</li>
     * <li>As a \f$N\f$ by \f$N\f$ matrix (say, \f$M\f$), where if
     * \f$M_{ii} = 1\f$, include vertex \f$i\f$ in the set of vertices to
     * compute single source shortest paths for.</li>
     * </ol>
     *
     * @param[in]  graph     The adjacency matrix of the graph for which SSSP
     *                       will be computed.  The implied zero will be
     *                       infinity.
     * @param[in,out] path   On input, the root vertex of the SSSP is set to zero.
     *                       On output, contains the shortest path for this
     *                       specified starting vertex, specified in terms of
     *                       length from the source vertex.
     */
    template<typename MatrixT,
             typename PathVectorT>
    void sssp(MatrixT const     &graph,
              PathVectorT       &path)
    {
        using T = typename MatrixT::ScalarType;

        if ((graph.nrows() != path.size()) ||
            (graph.ncols() != path.size()))
        {
            throw GraphBLAS::DimensionException();
        }

        /// @todo why num_rows iterations? (REALLY should be bounded by diameter)
        /// or detection of no change in paths matrix
        for (GraphBLAS::IndexType k = 0; k < graph.nrows(); ++k)
        {
            GraphBLAS::vxm(path,
                           GraphBLAS::NoMask(), GraphBLAS::Min<T>(),
                           GraphBLAS::MinPlusSemiring<T>(),
                           path, graph);

            //std::cout << "Iteration " << k << std::endl;
            //GraphBLAS::print_vector(std::cout, path, "Path");
            //std::cout << std::endl;
        }
        // paths holds return value
    }

    //****************************************************************************

    /**
     * @brief Compute the lenghts of the single source shortest path(s) of
     *        the specified starting vertex (vertices) in the given graph.
     *
     * The algorithm used in the implementation is the Floyd-Warshall
     * algorithm.  It is a dynamic programming solution (similar to the
     * Bellman-Ford algorithm).  The algorithm (like Bellman-Ford) is
     * based off of relaxation.  At each iteration in the algorithm, the
     * approximate lengths of the paths are always overestimates.  The
     * edges are relaxed at each iteration, until the lenghts are no
     * longer overestimates, at which point the algorithm terminates and
     * the path lengths are returned.
     *
     * The start can be specified multiple ways.  It can be in one
     * of the following three formats:
     *
     * <ol>
     * <li>The vertex itself (to specify only one vertex).</li>
     * <li>As a \f$N\f$ by \f$1\f$ column vector, where if the \f$i^{th}\f$
     * index is \f$1\f$, include that vertex in the set of vertices to
     * compute single source shortest paths for.</li>
     * <li>As a \f$N\f$ by \f$N\f$ matrix (say, \f$M\f$), where if
     * \f$M_{ii} = 1\f$, include vertex \f$i\f$ in the set of vertices to
     * compute single source shortest paths for.</li>
     * </ol>
     *
     * @param[in]  graph     The adjacency matrix of the graph for which SSSP
     *                       will be computed.  The implied zero will be
     *                       infinity.
     * @param[in,out] paths  Each row contains the lengths of single source
     *                       shortest path(s) for each the specified starting
     *                       vertices. These roots are specified, in this matrix
     *                       on input, by setting the source vertex to zero in the
     *                       corresponding row.
     */
    template<typename MatrixT,
             typename PathMatrixT>
    void batch_sssp(MatrixT const     &graph,
                    PathMatrixT       &paths)  // paths are initialized to start
    {
        using T = typename MatrixT::ScalarType;

        if ((graph.nrows() != paths.ncols()) ||
            (graph.ncols() != paths.ncols()))
        {
            throw GraphBLAS::DimensionException();
        }

        /// @todo why num_rows iterations?  Should be the diameter or terminate
        /// when there are no changes?
        for (GraphBLAS::IndexType k = 0; k < graph.nrows(); ++k)
        {
            GraphBLAS::mxm(paths,
                           GraphBLAS::NoMask(),
                           GraphBLAS::Min<T>(),
                           GraphBLAS::MinPlusSemiring<T>(),
                           paths, graph);

            //std::cout << "Iteration " << k << std::endl;
            //paths.print_info(std::cout);
            //std::cout << std::endl;
        }
        // paths holds return value
    }

    //************************************************************************
    template<typename MatrixT,
             typename DistanceVectorT>
    void filtered_sssp(MatrixT const     &graph,
                       DistanceVectorT   &distance)
    {
        using T = typename DistanceVectorT::ScalarType;

        if ((graph.nrows() != distance.size()) ||
            (graph.ncols() != distance.size()))
        {
            throw GraphBLAS::DimensionException();
        }

        GraphBLAS::Vector<T> new_distance(distance);
        GraphBLAS::Vector<bool> new_distance_flags(distance.size());

        /// @todo why num_rows iterations? (REALLY should be bounded by diameter)
        /// or detection of no change in distances matrix
        while (true)
        {
            std::cout << "============= Filtered BF Iteration ================="
                      << std::endl;
            GraphBLAS::vxm(new_distance,
                           GraphBLAS::NoMask(), GraphBLAS::NoAccumulate(),
                           GraphBLAS::MinPlusSemiring<T>(),
                           new_distance, graph);
            GraphBLAS::print_vector(std::cout, new_distance,
                                    "new distance");

            // new_distance_flags<new_distance> = new_distance .< distance
            GraphBLAS::eWiseAdd(new_distance_flags,
                                new_distance,
                                GraphBLAS::NoAccumulate(),
                                GraphBLAS::LessThan<T>(),
                                new_distance, distance, true);
            GraphBLAS::print_vector(std::cout, new_distance_flags,
                                    "new distance flags");

            // clear out any non-contributing paths
            GraphBLAS::apply(new_distance,
                             new_distance_flags,
                             GraphBLAS::NoAccumulate(),
                             GraphBLAS::Identity<T>(), new_distance, true);
            GraphBLAS::print_vector(std::cout, new_distance,
                                    "new distance (cleared)");

            if (new_distance.nvals() == 0)
                break;

            GraphBLAS::eWiseAdd(distance,
                                GraphBLAS::NoMask(),
                                GraphBLAS::NoAccumulate(),
                                GraphBLAS::Min<T>(),
                                new_distance, distance);
            GraphBLAS::print_vector(std::cout, distance, "Distance");
            std::cout << std::endl;
        }
        // paths holds return value
    }

    //************************************************************************
    //************************************************************************

    //************************************************************************
    template <typename ScalarT>
    struct SelectInRange
    {
        typedef bool result_type;

        ScalarT const m_low, m_high;

        SelectInRange(ScalarT low, ScalarT high) : m_low(low), m_high(high) {}

        inline bool operator()(ScalarT input)
        {
            if ((m_low <= input) && (input < m_high))
            {
                return true;
            }

            return false;
        }
    };

    //************************************************************************
    template<typename MatrixT>
    void sssp_delta_step(MatrixT const                                   &graph,
                         typename MatrixT::ScalarType                     delta,
                         GraphBLAS::IndexType                             src,
                         GraphBLAS::Vector<typename MatrixT::ScalarType> &paths)
    {
        std::cerr << "delta = " << delta << std::endl;
        using T = typename MatrixT::ScalarType;
        GraphBLAS::print_matrix(std::cerr, graph, "A");

        GraphBLAS::IndexType n(graph.nrows());  /// @todo assert nrows = ncols
        paths.clear();                          /// @todo assert size = nrows

        GraphBLAS::Vector<T>    t(n);
        GraphBLAS::Vector<T>    tmasked(n);
        GraphBLAS::Vector<T>    tReq(n);

        GraphBLAS::Vector<bool> tBi(n);
        GraphBLAS::Vector<bool> tnew(n);
        GraphBLAS::Vector<bool> tcomp(n);
        GraphBLAS::Vector<bool> tless(n);
        GraphBLAS::Vector<bool> s(n);

        // t = infinity, t[src] = 0
        t.setElement(src, 0);

        // AL = A .* (A <= delta)
        MatrixT AL(n, n);
        GraphBLAS::BinaryOp_Bind2nd<T, GraphBLAS::LessEqual<T>>
            leq_delta((T)delta);
        GraphBLAS::apply(AL, GraphBLAS::NoMask(), GraphBLAS::NoAccumulate(),
                         leq_delta, graph);
        GraphBLAS::apply(AL, AL, GraphBLAS::NoAccumulate(),
                         GraphBLAS::Identity<T>(), graph, true);
        GraphBLAS::print_matrix(std::cerr, AL, "AL = A(<=delta)");

        // AH = A .* (A > delta)
        MatrixT AH(n, n);
        //GraphBLAS::apply(AH, GraphBLAS::complement(AL), GraphBLAS::NoAccumulate(),
        //                 GraphBLAS::Identity<T>(), A);
        GraphBLAS::BinaryOp_Bind2nd<T, GraphBLAS::GreaterThan<T>>
            gt_delta(delta);
        GraphBLAS::apply(AH, GraphBLAS::NoMask(), GraphBLAS::NoAccumulate(),
                         gt_delta, graph);
        GraphBLAS::apply(AH, AH, GraphBLAS::NoAccumulate(),
                         GraphBLAS::Identity<T>(), graph, true);
        GraphBLAS::print_matrix(std::cerr, AH, "AH = A(>delta)");

        // i = 0
        GraphBLAS::IndexType i(0);

        // t >= i*delta
        GraphBLAS::BinaryOp_Bind2nd<T, GraphBLAS::GreaterEqual<T>>
            geq_idelta((T)i*delta);
        GraphBLAS::apply(tcomp, GraphBLAS::NoMask(), GraphBLAS::NoAccumulate(),
                         geq_idelta, t);
        GraphBLAS::apply(tcomp, tcomp, GraphBLAS::NoAccumulate(),
                         GraphBLAS::Identity<bool>(), tcomp);

        GraphBLAS::print_vector(std::cerr, tcomp, "tcomp = t(>=i*delta)");

        // while (t >= i*delta) not empty
        while (tcomp.nvals() > 0)
        {
            std::cerr << "************************************************\n";
            std::cerr << "****** Outer loop: i = " << i << std::endl;
            // s = 0 (clear)
            s.clear();

            // tBi = t .* (i*delta <= t < (i+1)*delta)
            SelectInRange<T> in_range((T)i*delta, ((T)(i + 1))*delta);
            GraphBLAS::apply(tBi, GraphBLAS::NoMask(), GraphBLAS::NoAccumulate(),
                             in_range, t);
            GraphBLAS::apply(tBi, tBi, GraphBLAS::NoAccumulate(),
                             GraphBLAS::Identity<T>(), tBi, true);
            GraphBLAS::print_vector(std::cerr, tBi,
                                    "tBi<tless> = tReq([i*d, (i+1)*d))");

            // tm<tBi> = t
            GraphBLAS::apply(tmasked, tBi, GraphBLAS::NoAccumulate(),
                             GraphBLAS::Identity<T>(), t, true);
            GraphBLAS::print_vector(std::cerr, tmasked, "tm = t<tBi>");

            while (tmasked.nvals() > 0)
            {
                std::cerr << "******************* inner *********************\n";
                // tReq = AL' (min.+) (t .* tBi)
                GraphBLAS::vxm(tReq, GraphBLAS::NoMask(), GraphBLAS::NoAccumulate(),
                               GraphBLAS::MinPlusSemiring<T>(), tmasked, AL);
                GraphBLAS::print_vector(std::cerr, tReq, "tReq = tm*AL");

                // s = s + tBi
                GraphBLAS::eWiseAdd(s,
                                    GraphBLAS::NoMask(),
                                    GraphBLAS::NoAccumulate(),
                                    GraphBLAS::LogicalOr<bool>(),
                                    s, tBi);
                GraphBLAS::print_vector(std::cerr, s, "s += tBi");

                // Don't tBi = 0
                // tBi.clear();

                // tless<tReq> = tReq .< t
                GraphBLAS::eWiseAdd(tless,
                                    tReq,
                                    GraphBLAS::NoAccumulate(),
                                    GraphBLAS::LessThan<T>(),
                                    tReq, t, true);
                GraphBLAS::print_vector(std::cerr, tless, "tless<tReq> = tReq .< t");

                // tBi<tless> = i*delta <= tReq < (i+1)*delta
                GraphBLAS::apply(tBi,
                                 tless,
                                 GraphBLAS::NoAccumulate(),
                                 in_range, tReq, true);
                //GraphBLAS::apply(tnew, tnew, GraphBLAS::NoAccumulate(),
                //                 GraphBLAS::Identity<bool>(), tnew, true);
                GraphBLAS::print_vector(std::cerr, tBi,
                                        "tBi<tless> = tReq([i*d, (i+1)*d))");

                // t = min(t, tReq)
                GraphBLAS::eWiseAdd(t,
                                    GraphBLAS::NoMask(),
                                    GraphBLAS::NoAccumulate(),
                                    GraphBLAS::Min<T>(),
                                    t, tReq);
                GraphBLAS::print_vector(std::cerr, t, "t = min(t, tReq)");

                // tm<tBi> = t
                GraphBLAS::apply(tmasked, tBi, GraphBLAS::NoAccumulate(),
                                 GraphBLAS::Identity<T>(), t, true);
                GraphBLAS::print_vector(std::cerr, tmasked, "tm = t<tBi>");
            }
            std::cerr << "******************** end inner loop *****************\n";

            // (t .* s)
            GraphBLAS::apply(tmasked, s, GraphBLAS::NoAccumulate(),
                             GraphBLAS::Identity<T>(), t, true);
            GraphBLAS::print_vector(std::cerr, tmasked, "tm = t<s>");

            // tReq = AH'(t .* s)
            GraphBLAS::vxm(tReq,
                           GraphBLAS::NoMask(),
                           GraphBLAS::NoAccumulate(),
                           GraphBLAS::MinPlusSemiring<T>(), tmasked, AH);
            GraphBLAS::print_vector(std::cerr, tReq, "tReq = tcomp min.+ AH");

            // t = min(t, tReq)
            GraphBLAS::eWiseAdd(t,
                                GraphBLAS::NoMask(),
                                GraphBLAS::NoAccumulate(),
                                GraphBLAS::Min<T>(), t, tReq);
            GraphBLAS::print_vector(std::cerr, t, "t = min(t,tReq)");

            ++i;

            // t >= i*delta
            GraphBLAS::BinaryOp_Bind2nd<T, GraphBLAS::GreaterEqual<T>>
                geq_idelta((T)i*delta);

            GraphBLAS::apply(tcomp,
                             GraphBLAS::NoMask(),
                             GraphBLAS::NoAccumulate(),
                             geq_idelta, t);
            GraphBLAS::apply(tcomp, tcomp, GraphBLAS::NoAccumulate(),
                             GraphBLAS::Identity<bool>(), tcomp, true);
            GraphBLAS::print_vector(std::cerr, tcomp, "tcomp = t(>=i*delta)");
        }

        // result = t
        GraphBLAS::apply(paths, GraphBLAS::NoMask(), GraphBLAS::NoAccumulate(),
                         GraphBLAS::Identity<T>(), t);
    }
} // algorithms

#endif // GBTL_SSSP_HPP
