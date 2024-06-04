#copyright (c) 2024 Codezeelers , ALL Rights reserved
#Author: Ibrahim/Hafiz
cmake_minimum_required(VERSION 3.22)

project(INCManager)

set(PUBLIC_INCLUDE_DIR  ${CMAKE_CURRENT_LIST_DIR}/public)
set(PRIVATE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(${PROJECT_NAME} "")

add_subdirectories(PhyComm)

target_link_libraries(${PROJECT_NAME} PUBLIC
#Add libs to link
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    ${PUBLIC_INCLUDE_DIR}
PRIVATE
    ${PRIVATE_INCLUDE_DIR}
)
install(TARGETS ${PROJECT_NAME} DESTINATION ./Bins)

