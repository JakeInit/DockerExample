if(WIN32)
	set (CMAKE_GENERATOR "MinGW Makefiles" CACHE INTERNAL "" FORCE)
  message(STATUS "generator is set to ${CMAKE_GENERATOR}")
endif(WIN32)