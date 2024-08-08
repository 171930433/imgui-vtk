
find_package(GLEW REQUIRED)
find_package(VTK COMPONENTS 
  CommonCore
  CommonColor
  CommonDataModel
  FiltersCore
  InteractionStyle
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  QUIET
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "VTK not found!")
  return()
endif()

add_library(imgui_vtk_viewer SHARED VtkViewer.cpp)
target_link_libraries(imgui_vtk_viewer PUBLIC glfw_opengl3 GLEW ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS imgui_vtk_viewer
  MODULES ${VTK_LIBRARIES}
)

if(BUILD_EXAMPLE)
  add_executable(imgui_vtk main.cpp)
  target_link_libraries(imgui_vtk PRIVATE imgui_vtk_viewer glfw_opengl3 ${VTK_LIBRARIES} )

  # vtk_module_autoinit(
  #   TARGETS imgui_vtk
  #   MODULES ${VTK_LIBRARIES}
  # )

endif()