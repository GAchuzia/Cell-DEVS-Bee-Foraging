add_executable(${projectName} main.cpp)
target_include_directories(${projectName} PRIVATE $ENV{CADMIUM} "$ENV{CADMIUM}/../json/include" "../model")
target_compile_options(${projectName} PUBLIC -std=gnu++17)
