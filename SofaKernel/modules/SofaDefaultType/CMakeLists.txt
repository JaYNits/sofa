cmake_minimum_required(VERSION 3.12)
project(SofaDefaultType)

set(SRC_ROOT "src/sofa/defaulttype")

set(HEADER_FILES
    ${SRC_ROOT}/BaseMatrix.h
    ${SRC_ROOT}/BaseVector.h
    ${SRC_ROOT}/BoundingBox.h
    ${SRC_ROOT}/DataTypeInfo.h
    ${SRC_ROOT}/Frame.h
    ${SRC_ROOT}/MapMapSparseMatrix.h
    ${SRC_ROOT}/MapMapSparseMatrixEigenUtils.h
    ${SRC_ROOT}/Mat.h
    ${SRC_ROOT}/MatSym.h
    ${SRC_ROOT}/Mat_solve_Cholesky.h
    ${SRC_ROOT}/Mat_solve_LU.h
    ${SRC_ROOT}/Mat_solve_SVD.h
    ${SRC_ROOT}/Quat.h
    ${SRC_ROOT}/Quat.inl
    ${SRC_ROOT}/RGBAColor.h
    ${SRC_ROOT}/RigidTypes.h
    ${SRC_ROOT}/RigidVec6Types.h
    ${SRC_ROOT}/SolidTypes.h
    ${SRC_ROOT}/SolidTypes.inl
    ${SRC_ROOT}/TemplatesAliases.h
    ${SRC_ROOT}/TopologyTypes.h
    ${SRC_ROOT}/Vec.h
    ${SRC_ROOT}/Ray.h
    ${SRC_ROOT}/Vec3Types.h
    ${SRC_ROOT}/VecTypes.h
    ${SRC_ROOT}/defaulttype.h
    ${SRC_ROOT}/config.h.in
    ${SRC_ROOT}/init.h
    ${SRC_ROOT}/Color.h
)

set(SOURCE_FILES
    ${SRC_ROOT}/BaseMatrix.cpp
    ${SRC_ROOT}/BoundingBox.cpp
    ${SRC_ROOT}/Frame.cpp
    ${SRC_ROOT}/SolidTypes.cpp
    ${SRC_ROOT}/TemplatesAliases.cpp
    ${SRC_ROOT}/init.cpp
)

find_package(Eigen3 REQUIRED)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SofaHelper)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
target_compile_definitions(${PROJECT_NAME} PRIVATE "-DSOFA_BUILD_DEFAULTTYPE")

sofa_install_targets(SofaFramework ${PROJECT_NAME} "SofaFramework" "src")

if(SOFA_BUILD_TESTS)
    add_subdirectory(SofaDefaultType_test)
endif()
