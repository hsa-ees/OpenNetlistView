cmake_minimum_required(VERSION 3.15)

# set the defines LIBVPSC_DEBUG and LIBVPSC_LOGGING
# add_definitions(-DLIBVPSC_DEBUG)

# libvpsc
set(LIBVPSC_LIB vpsc)

set(LIBVPSC_SRC
    libvpsc/block.cpp
    libvpsc/blocks.cpp
    libvpsc/cbuffer.cpp
    libvpsc/constraint.cpp
    libvpsc/rectangle.cpp
    libvpsc/solve_VPSC.cpp
    libvpsc/variable.cpp)

project(${LIBVPSC_LIB}
    LANGUAGES CXX)

add_library(${LIBVPSC_LIB} ${LIBVPSC_SRC})

# libcola
set(LIBCOLA_LIB cola)

set(LIBCOLA_SRC
    libcola/box.cpp
    libcola/cc_clustercontainmentconstraints.cpp
    libcola/cc_nonoverlapconstraints.cpp
    libcola/cluster.cpp
    libcola/colafd.cpp
    libcola/compound_constraints.cpp
    libcola/conjugate_gradient.cpp
    libcola/connected_components.cpp
    libcola/convex_hull.cpp
    libcola/gradient_projection.cpp
    libcola/output_svg.cpp
    libcola/pseudorandom.cpp
    libcola/shapepair.cpp
    libcola/straightener.cpp)

project(${LIBCOLA_LIB}
    LANGUAGES CXX)

add_library(${LIBCOLA_LIB} ${LIBCOLA_SRC})
target_link_libraries(${LIBCOLA_LIB} PRIVATE ${LIBVPSC_LIB})

# libavoid
set(LIBAVOID_LIB avoid)

set(LIBAVOID_SRC
    libavoid/actioninfo.cpp
    libavoid/connectionpin.cpp
    libavoid/connector.cpp
    libavoid/connend.cpp
    libavoid/geometry.cpp
    libavoid/geomtypes.cpp
    libavoid/graph.cpp
    libavoid/hyperedge.cpp
    libavoid/hyperedgeimprover.cpp
    libavoid/hyperedgetree.cpp
    libavoid/junction.cpp
    libavoid/makepath.cpp
    libavoid/mtst.cpp
    libavoid/obstacle.cpp
    libavoid/orthogonal.cpp
    libavoid/router.cpp
    libavoid/scanline.cpp
    libavoid/shape.cpp
    libavoid/timer.cpp
    libavoid/vertices.cpp
    libavoid/viscluster.cpp
    libavoid/visibility.cpp
    libavoid/vpsc.cpp)

project(${LIBAVOID_LIB}
    LANGUAGES CXX)

add_library(${LIBAVOID_LIB} ${LIBAVOID_SRC})

# libtopology
set(LIBTOPOLOGY_LIB topology)

set(LIBTOPOLOGY_SRC
    libtopology/cola_topology_addon.cpp
    libtopology/cola_topology_addon.h
    libtopology/compute_forces.cpp
    libtopology/orthogonal_topology.cpp
    libtopology/orthogonal_topology.h
    libtopology/resize.cpp
    libtopology/topology_constraints_constructor.cpp
    libtopology/topology_constraints.cpp
    libtopology/topology_constraints.h
    libtopology/topology_graph.cpp
    libtopology/topology_graph.h
    libtopology/topology_log.h
    libtopology/util.h
)

project(${LIBTOPOLOGY_LIB}
    LANGUAGES CXX)

add_library(${LIBTOPOLOGY_LIB} ${LIBTOPOLOGY_SRC})

target_link_libraries(${LIBTOPOLOGY_LIB} PRIVATE ${LIBCOLA_LIB} ${LIBAVOID_LIB} ${LIBVPSC_LIB})
