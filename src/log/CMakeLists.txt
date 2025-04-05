# Create the log library as an interface target.
add_library(log INTERFACE)

# Specify the include directory so that any consumer of log can include logger.h.
target_include_directories(log INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Propagate the necessary compile definition for Boost.Log dynamic linking.
target_compile_definitions(log INTERFACE BOOST_LOG_DYN_LINK)

# Propagate Boost libraries and pthread as interface dependencies.
target_link_libraries(log INTERFACE ${Boost_LIBRARIES} pthread)