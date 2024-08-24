set(LLVM_TOOLS
  dce 
  static_cc)

set(dce_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/DCEMain.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../lib/DeadCodeElimination.cpp"
)

set(static_cc_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/StaticCallCountMain.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/../lib/StaticCallCounter.cpp"
)


foreach( tool ${LLVM_TOOLS} )
  add_executable(${tool} ${${tool}_SOURCES})

  target_include_directories(
    ${tool}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")

  target_link_libraries(${tool}
      LLVMCore LLVMPasses LLVMIRReader LLVMSupport
  )

endforeach()