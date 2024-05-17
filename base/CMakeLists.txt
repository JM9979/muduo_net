set(base_SRCS
        CountDownLatch.cpp
        Thread.cpp
)

add_library(muduo_base ${base_SRCS})


#install(TARGETS muduo_base DESTINATION lib)

#file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/muduo/base)

add_subdirectory(test)
