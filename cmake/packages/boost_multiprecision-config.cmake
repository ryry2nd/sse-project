add_library(boost_multiprecision INTERFACE)
add_library(Boost::multiprecision ALIAS boost_multiprecision)

target_include_directories(boost_multiprecision INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/boost_config/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/multiprecision/include"
)