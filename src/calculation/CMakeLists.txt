set (TARGET_NAME "calculate_aloha_things")
set (SOURCES "calculation.cpp")

add_executable (${TARGET_NAME} ${SOURCES})
target_link_libraries (${TARGET_NAME} Eigen3::Eigen)
