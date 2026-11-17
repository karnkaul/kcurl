add_library(${PROJECT_NAME})
add_library(${package_name}::${package_name} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  ${curl_link_target}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
)

set(version_module public/build_version.cppm)
configure_file(public/build_version.cppm.in ${version_module} @ONLY)

file(GLOB_RECURSE headers LIST_DIRECTORIES false "include/*.hpp")

target_sources(${PROJECT_NAME} PUBLIC FILE_SET CXX_MODULES
  BASE_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"

  FILES
  public/types.cppm
  public/curl.cppm
  public/easy.cppm
  public/http.cppm
  public/kcurl.cppm
  "${CMAKE_CURRENT_BINARY_DIR}/${version_module}"
)

target_sources(${PROJECT_NAME} PRIVATE 
  src/http.cpp
)

if(KCURL_CI_NO_LIBCURL)
  target_sources(${PROJECT_NAME} PRIVATE 
    src/null_curl.cpp
    src/null_easy.cpp
  )
else()
  target_sources(${PROJECT_NAME} PRIVATE 
    src/curl.cpp
    src/easy.cpp
  )
endif()
