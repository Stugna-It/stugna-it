cmake_minimum_required(VERSION 3.5)

project(stugna-it LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "-O3")


include(FindPkgConfig)


set(src
   main.cpp
   utils.cpp
   httpserver.cpp
   proxy.cpp
   jobs.cpp
   job.cpp
   uaheaders.cpp
   stats.cpp
   multigun.cpp
)

add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CURL_STATICLIB OFF)


find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_executable(stugna-it ${src})
target_link_libraries(stugna-it PUBLIC ${CURL_LIBRARIES} Threads::Threads OpenSSL::SSL CURL::libcurl)


add_custom_command(TARGET stugna-it POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/index.html ${PROJECT_BINARY_DIR}/index.html
    COMMENT "copy WebUI index.html to target dir"

    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/config.json ${PROJECT_BINARY_DIR}/config.json
    COMMENT "copy config to target dir"
)

