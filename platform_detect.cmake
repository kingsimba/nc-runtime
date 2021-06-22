# This script define NC_PLATFORM variable
# ${NC_PLATFORM} will be pi64/linux64/x64
# ${NC_UNIX} will be 1/0

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        set(NC_PLATFORM "pi64")
    else()
        set(NC_PLATFORM "linux64")
    endif()
    set(NC_UNIX 1)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(NC_PLATFORM "x64")
    set(NC_UNIX 0)
elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set(NC_PLATFORM "pi64")
    set(NC_UNIX 1)
else()
    message(FATAL_ERROR "Unknown platform")
endif()
