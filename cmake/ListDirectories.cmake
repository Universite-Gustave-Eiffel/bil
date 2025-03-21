
# Function to get the list of directories found below a given directory
function(list_directories dir return_var)
  #message("Entry in list_directories!")
  #message("return_var: ${return_var}")
  message("dir: ${dir}")
  if(IS_ABSOLUTE ${dir})
    set(fulldir ${dir})
    #message("fulldir-1: ${fulldir}")
  elseif("${dir}" STREQUAL ".")
    set(fulldir ${CMAKE_CURRENT_SOURCE_DIR})
    #message("fulldir-2: ${fulldir}")
  else()
    set(fulldir ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
    message("fulldir-3: ${fulldir}")
  endif()
  unset(list)
  #file(GLOB list LIST_DIRECTORIES true RELATIVE ${fulldir} ${fulldir}/*)
  file(GLOB list LIST_DIRECTORIES true ${fulldir}/*)
  #message("list: ${list}")
  unset(dirlist)
  foreach(it  ${list})
    if(IS_DIRECTORY ${it})
      list(APPEND dirlist ${it})
    endif()
  endforeach()
  #message("dirlist: ${dirlist}")
  set(${return_var} ${dirlist} PARENT_SCOPE)
endfunction()
