CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT(Countdown)

ADD_EXECUTABLE(
    Countdown
    source/main.cpp
)

IF(NOT WIN32)
    TARGET_LINK_LIBRARIES(
        Countdown
        pthread
    ) 

    IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
        TARGET_LINK_LIBRARIES(
	    Countdown
	    gcov asan
	)
    ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)
ENDIF(NOT WIN32)

TARGET_COMPILE_FEATURES(
    Countdown
    PRIVATE
    cxx_relaxed_constexpr
)

SET_PROPERTY(
    TARGET Countdown 
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "-Wall -Werror"
)

IF(NOT WIN32)
    IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
        SET_PROPERTY(
            TARGET Countdown 
            APPEND_STRING PROPERTY COMPILE_FLAGS
            " -fsanitize=address --coverage"
        )
    ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)
ENDIF()

