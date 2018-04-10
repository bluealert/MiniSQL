set(dirs init query storage common)
foreach(dir ${dirs})
    file(GLOB_RECURSE files ${dir}/*.h ${dir}/*.cpp)
    set(minisql_files ${minisql_files} ${files})
endforeach()

add_library(minisql ${minisql_files})