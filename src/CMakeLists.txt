add_executable(${projectName} main.cpp)
set(CADMIUM_DIR $ENV{CADMIUM})
target_include_directories(${projectName} PUBLIC
    "."
    ".."
    "../model"
    ${CADMIUM_DIR}
    "${CADMIUM_DIR}/../json/include"
)
target_compile_options(${projectName} PUBLIC -std=gnu++17)
