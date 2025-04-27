if(NOT TARGET SDL3::Headers)
add_library(SDL3::Headers INTERFACE IMPORTED)
set_target_properties(SDL3::Headers PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "C:/Users/Hexagon/.gradle/caches/8.12/transforms/911fc0ec81bf1bedb9f080a8bd8c9ca9/transformed/SDL3-3.2.10/prefab/modules/Headers/include"
    INTERFACE_LINK_LIBRARIES "SDL3::SDL3-Headers"
)
endif()

if(NOT TARGET SDL3::SDL3)
add_library(SDL3::SDL3 INTERFACE IMPORTED)
set_target_properties(SDL3::SDL3 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "C:/Users/Hexagon/.gradle/caches/8.12/transforms/911fc0ec81bf1bedb9f080a8bd8c9ca9/transformed/SDL3-3.2.10/prefab/modules/SDL3/include"
    INTERFACE_LINK_LIBRARIES "SDL3::SDL3-shared"
)
endif()

if(NOT TARGET SDL3::SDL3-Headers)
add_library(SDL3::SDL3-Headers INTERFACE IMPORTED)
set_target_properties(SDL3::SDL3-Headers PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "C:/Users/Hexagon/.gradle/caches/8.12/transforms/911fc0ec81bf1bedb9f080a8bd8c9ca9/transformed/SDL3-3.2.10/prefab/modules/SDL3-Headers/include"
    INTERFACE_LINK_LIBRARIES ""
)
endif()

if(NOT TARGET SDL3::SDL3-shared)
add_library(SDL3::SDL3-shared SHARED IMPORTED)
set_target_properties(SDL3::SDL3-shared PROPERTIES
    IMPORTED_LOCATION "C:/Users/Hexagon/.gradle/caches/8.12/transforms/911fc0ec81bf1bedb9f080a8bd8c9ca9/transformed/SDL3-3.2.10/prefab/modules/SDL3-shared/libs/android.arm64-v8a/libSDL3.so"
    INTERFACE_INCLUDE_DIRECTORIES "C:/Users/Hexagon/.gradle/caches/8.12/transforms/911fc0ec81bf1bedb9f080a8bd8c9ca9/transformed/SDL3-3.2.10/prefab/modules/SDL3-shared/include"
    INTERFACE_LINK_LIBRARIES "SDL3::Headers"
)
endif()

if(NOT TARGET SDL3::SDL3_test)
add_library(SDL3::SDL3_test STATIC IMPORTED)
set_target_properties(SDL3::SDL3_test PROPERTIES
    IMPORTED_LOCATION "C:/Users/Hexagon/.gradle/caches/8.12/transforms/911fc0ec81bf1bedb9f080a8bd8c9ca9/transformed/SDL3-3.2.10/prefab/modules/SDL3_test/libs/android.arm64-v8a/libSDL3_test.a"
    INTERFACE_INCLUDE_DIRECTORIES "C:/Users/Hexagon/.gradle/caches/8.12/transforms/911fc0ec81bf1bedb9f080a8bd8c9ca9/transformed/SDL3-3.2.10/prefab/modules/SDL3_test/include"
    INTERFACE_LINK_LIBRARIES "SDL3::Headers"
)
endif()

