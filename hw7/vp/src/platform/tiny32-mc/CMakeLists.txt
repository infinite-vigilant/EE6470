file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-tiny32-mc
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-tiny32-mc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tiny32-mc
        mc_main.cpp
        ${HEADERS})

target_link_libraries(tiny32-mc rv32 platform-basic-acc platform-common gdb-mc ${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)
# target_link_libraries(tiny32-mc rv32 platform-common gdb-mc
	# ${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS riscv-vp RUNTIME DESTINATION bin)
