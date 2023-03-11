set(srcs "storage.c")
idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "." REQUIRES bsp)
