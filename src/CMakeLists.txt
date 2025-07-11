# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

# SPDX-License-Identifier: MPL-2.0

cmake_minimum_required(VERSION 3.21)

add_library(cereal 
	include/cereal/cereal.hpp
	cereal.cpp
)
target_include_directories(cereal PUBLIC include)
target_compile_features(cereal PUBLIC cxx_std_23)

add_executable(asdf asdf.cpp)
target_link_libraries(asdf PUBLIC cereal)

