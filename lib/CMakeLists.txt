add_library(OpcodeCounter SHARED
  OpcodeCounter.cpp
  )

target_include_directories(OpcodeCounter PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)
target_link_libraries(OpcodeCounter
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
