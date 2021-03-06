cmake_minimum_required(VERSION 3.12)
project(SofaGeneral)

# Send warnings when deprecated CMake project is activated
option(CMAKE_WARN_DEPRECATED "Send warning when deprecated projects are found" ON)

# Subdirectories
set(SOFAGENERAL_TARGETS
    SofaGeneralAnimationLoop
    SofaGeneralDeformable
    SofaGeneralExplicitOdeSolver
    SofaGeneralImplicitOdeSolver
    SofaGeneralLinearSolver
    SofaGeneralLoader
    SofaGeneralMeshCollision
    SofaGeneralObjectInteraction
    SofaGeneralRigid
    SofaGeneralSimpleFem
    SofaGeneralTopology
    SofaGeneralVisual
    SofaBoundaryCondition
    SofaConstraint
    SofaGeneralEngine
    SofaGraphComponent
    SofaTopologyMapping
    SofaUserInteraction
)

foreach(TARGET ${SOFAGENERAL_TARGETS})
    add_subdirectory(../modules/${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})
endforeach()

set(SOFAGENERAL_SRC src/${PROJECT_NAME})

set(HEADER_FILES
    ${SOFAGENERAL_SRC}/config.h.in
    ${SOFAGENERAL_SRC}/initSofaGeneral.h
    )

set(SOURCE_FILES
    ${SOFAGENERAL_SRC}/initSofaGeneral.cpp
    )

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SOFAGENERAL_TARGETS})

sofa_generate_package(
    NAME ${PROJECT_NAME}
    VERSION ${Sofa_VERSION}
    TARGETS ${PROJECT_NAME}
    INCLUDE_SOURCE_DIR "src"
    INCLUDE_INSTALL_DIR "${PROJECT_NAME}"
    )
