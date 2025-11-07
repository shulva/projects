cmake_minimum_required(VERSION 4.0)
project(cudatest CUDA)

set(CMAKE_CUDA_STANDARD 20)

add_executable(cudatest main.cu)

set_target_properties(cudatest PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
