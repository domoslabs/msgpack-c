# Source files
SET (dmsgpack-c_SOURCES
    src/objectc.c
    src/unpack.c
    src/version.c
    src/vrefbuffer.c
    src/zone.c
)

# Header files
SET (dmsgpack-c_common_HEADERS
    include/dmsgpack.h
    include/dmsgpack/fbuffer.h
    include/dmsgpack/gcc_atomic.h
    include/dmsgpack/object.h
    include/dmsgpack/pack.h
    include/dmsgpack/pack_define.h
    include/dmsgpack/sbuffer.h
    include/dmsgpack/timestamp.h
    include/dmsgpack/unpack.h
    include/dmsgpack/unpack_define.h
    include/dmsgpack/unpack_template.h
    include/dmsgpack/util.h
    include/dmsgpack/version.h
    include/dmsgpack/version_master.h
    include/dmsgpack/vrefbuffer.h
    include/dmsgpack/zbuffer.h
    include/dmsgpack/zone.h
)

# Header files will configured
SET (dmsgpack-c_configured_HEADERS
    include/dmsgpack/pack_template.h
    include/dmsgpack/sysdep.h
)

# All header files
LIST (APPEND dmsgpack-c_HEADERS
    ${dmsgpack-c_common_HEADERS}
    ${dmsgpack-c_configured_HEADERS}
)
