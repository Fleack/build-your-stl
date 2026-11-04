option(BYS_REFERENCE_TESTS "Build and register exercise tests against reference implementations" OFF)

include("${CMAKE_CURRENT_LIST_DIR}/AddBysExercise.cmake")

add_bys_exercise(warmup/add "")
add_bys_exercise(algorithms/find algorithm)
add_bys_exercise(algorithms/count algorithm)
add_bys_exercise(algorithms/equal algorithm)
add_bys_exercise(algorithms/lower_bound algorithm)
add_bys_exercise(algorithms/upper_bound algorithm)
add_bys_exercise(containers/array array get swap to_array)
add_bys_exercise(containers/vector vector swap erase erase_if)
add_bys_exercise(views/span span dynamic_extent as_bytes as_writable_bytes)
