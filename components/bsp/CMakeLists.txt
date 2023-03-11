set(srcs "bsp.c")
idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "." REQUIRES driver fatfs)
