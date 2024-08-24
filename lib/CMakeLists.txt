set(LLVM_PLUGINS
    OpcodeCounter
    InstrumentFuncCall
    StaticCallCounter
    DynamicCallCounter
    ArithmeticObfuscators
    DeadCodeElimination
    )

set(OpcodeCounter_SOURCES
  OpcodeCounter.cpp)
set(InstrumentFuncCall_SOURCES
  InstrumentFuncCall.cpp)
set(StaticCallCounter_SOURCES
  StaticCallCounter.cpp)
set(DynamicCallCounter_SOURCES
  DynamicCallCounter.cpp)
set(ArithmeticObfuscators_SOURCES
  ArithmeticObfuscators.cpp)
set(DeadCodeElimination_SOURCES
  DeadCodeElimination.cpp)

foreach( plugin ${LLVM_PLUGINS} )
  add_library(
    ${plugin}
    SHARED
    ${${plugin}_SOURCES}
    )

  target_include_directories(
    ${plugin}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
  )

  target_link_libraries(
    ${plugin}
    "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
    )
endforeach()

