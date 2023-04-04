set(srcs "ak8963.c"
                   "calibrate.c"
                   "common.c"
                   "i2c-easy.c"
                   "mpu9250.c")
idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "." REQUIRES driver bsp)
