set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_EXCLUDE_PATTERNS default_font.h)

doxygen_add_docs(docs
    "${PROJECT_SOURCE_DIR}/README.md"
    "${PROJECT_SOURCE_DIR}/src/singe-core"
    "${PROJECT_SOURCE_DIR}/src/singe-graphics"
    "${PROJECT_SOURCE_DIR}/src/singe-support"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
