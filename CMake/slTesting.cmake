# Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License").
# You may not use this file except in compliance with the License.
# A copy of the License is located at
#
#  http://aws.amazon.com/apache2.0
#
# or in the "license" file accompanying this file. This file is distributed
# on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
# express or implied. See the License for the specific language governing
# permissions and limitations under the License.

include(AwsCFlags)
include(AwsSanitizers)

# Registers a test case by name (the first argument to the AWS_TEST_CASE macro in aws_test_harness.h)
macro(sl_add_test_case name)
    list(APPEND TEST_CASES "ctest_${name}")
endmacro()

# Generate a test driver executable with the given name
function(sl_generate_test_driver driver_exe_name namespace)
    create_test_sourcelist(test_srclist ${namespace}_test_runner.c ${TEST_CASES})
    # Write clang tidy file that disables all but one check to avoid false positives
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.clang-tidy" "Checks: '-*,misc-static-assert'")

    add_executable(${driver_exe_name} ${CMAKE_CURRENT_BINARY_DIR}/${namespace}_test_runner.c ${TESTS})
    aws_set_common_properties(${driver_exe_name} NO_WEXTRA NO_PEDANTIC)
    aws_add_sanitizers(${driver_exe_name} ${${TARGET_PREFIX_STR}_SANITIZERS})

    target_link_libraries(${driver_exe_name} PRIVATE ${TARGET_PREFIX_STR})

    #set_target_properties(${driver_exe_name} PROPERTIES LINKER_LANGUAGE C C_STANDARD 99)
    target_compile_definitions(${driver_exe_name} PRIVATE -DAWS_UNSTABLE_TESTING_API)
    target_include_directories(${driver_exe_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

    foreach(name IN LISTS TEST_CASES)
        add_test(${name} ${driver_exe_name} "${name}")
    endforeach()

    # Clear test cases in case another driver needsto be generated
    unset(TEST_CASES PARENT_SCOPE)
endfunction()

# Generate a test driver executable for a single case
function(sl_test_single c_file_name test_case_name target_link_libs target_compile_defs)
	set(TEST_CASE_NAME "ctest_${test_case_name}")
	set(TEST_SRC_C ${c_file_name}.c)
	set(TEST_OUTPUT_C ${CMAKE_CURRENT_BINARY_DIR}/${c_file_name}_runner_single.c)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tests/runner_single.c.in ${TEST_OUTPUT_C})

	set(TEST_SINGLE_TARGET ${c_file_name}_runner_single)

	add_executable(${TEST_SINGLE_TARGET} ${TEST_OUTPUT_C})
	target_include_directories(${TEST_SINGLE_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)
	target_link_libraries(${TEST_SINGLE_TARGET} ${target_link_libs})
	target_compile_definitions(${TEST_SINGLE_TARGET} PRIVATE ${target_compile_defs} -DAWS_UNSTABLE_TESTING_API)
endfunction()
