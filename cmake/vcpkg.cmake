# vcpkg manifest mode takes a long time to run even if nothing has changed due
# to hashing compiler executables... use timestamp to disable manifest if it
# hasn't changed
# https://github.com/microsoft/vcpkg/issues/16920#issuecomment-852436706


if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/ports)
    set(VCPKG_OVERLAY_PORTS ${CMAKE_CURRENT_LIST_DIR}/ports)

    file(GLOB ports_files "${CMAKE_CURRENT_LIST_DIR}/ports/**/*")
    foreach(f IN LISTS ports_files)
        file(TIMESTAMP ${f} hash)
        list(APPEND vcpkg_hashes ${hash})
    endforeach()
endif()

file(TIMESTAMP "${CMAKE_CURRENT_LIST_DIR}/../vcpkg.json" hash)
list(APPEND vcpkg_hashes ${hash})
string(SHA512 vcpkg_hash "${vcpkg_hashes}")

if(VCPKG_HASH_PREV STREQUAL vcpkg_hash)
    set(VCPKG_MANIFEST_INSTALL OFF CACHE STRING "" FORCE)
else()
    set(VCPKG_MANIFEST_INSTALL ON CACHE STRING "" FORCE)
endif()

set(VCPKG_HASH_PREV "${vcpkg_hash}" CACHE STRING "" FORCE)

macro(vcpkg_add_feature FEATURE)
    list(APPEND VCPKG_MANIFEST_FEATURES "${FEATURE}")
endmacro()

macro(vcpkg_feature NAME DOC VALUE FEATURE)
    option(${NAME} ${DOC} ${VALUE})

    if(${${NAME}})
        vcpkg_add_feature(${FEATURE})
    endif()
endmacro()
