#include_directories(${PROJECT_SOURCE_DIR}/src/include)
#link_directories(${PROJECT_SOURCE_DIR}/src/lib)
#find_package(nlohmann_json 3.6.0 REQUIRED)

add_executable(ocrimg ocrimg.cpp)

target_link_libraries(
        ocrimg
        tencentcloud-sdk-cpp-ocr
        tencentcloud-sdk-cpp-core
        curl
        gflags
        pthread
        )


install(FILES conf/tencent_key.json DESTINATION /etc/ )