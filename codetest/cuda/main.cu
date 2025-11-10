#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <stdexcept>
#include <cmath>
#include <cuda_runtime.h>

#define CEIL_DIV(a,b) ((a+b-1)/b)

// A helper macro to check for CUDA API errors
#define CHECK_CUDA_ERROR(val) check((val), #val, __FILE__, __LINE__)
__host__ void check(cudaError_t err, const char* const func, const char* const file, const int line) {
    if (err != cudaSuccess) {
        std::cerr << "CUDA Error at: " << file << ":" << line << std::endl;
        std::cerr << cudaGetErrorString(err) << " " << func << std::endl;
        #ifdef _WIN32
        system("pause");
        #endif
        std::exit(EXIT_FAILURE);
    }
}

const int BLOCKSIZE = 64;
// ---------------------------------------------------------------------------
// Your SGEMM Kernel (Device Code)
// ---------------------------------------------------------------------------
__global__ void sgemm_naive(int M, int N, int K, float alpha, const float *A,
                            const float *B, float beta, float *C) {
    // the output block that we want to compute in this threadblock
    const int cRow = blockIdx.x;
    const int cCol = blockIdx.y;

    // allocate buffer for current block in fast shared mem
    // shared mem is shared between all threads in a block
    __shared__ float As[BLOCKSIZE * BLOCKSIZE];
    __shared__ float Bs[BLOCKSIZE * BLOCKSIZE];

    // the inner row & col that we're accessing in this thread
    const int threadCol = threadIdx.x % BLOCKSIZE;
    const int threadRow = threadIdx.x / BLOCKSIZE;

    // advance pointers to the starting positions
    A += cRow * BLOCKSIZE * K;                    // row=cRow, col=0
    B += cCol * BLOCKSIZE;                        // row=0, col=cCol
    C += cRow * BLOCKSIZE * N + cCol * BLOCKSIZE; // row=cRow, col=cCol

    float tmp = 0.0;
    for (int bkIdx = 0; bkIdx < K; bkIdx += BLOCKSIZE) {
        // Have each thread load one of the elements in A & B
        // Make the threadCol (=threadIdx.x) the consecutive index
        // to allow global memory access coalescing
        As[threadRow * BLOCKSIZE + threadCol] = A[threadRow * K + threadCol];
        Bs[threadRow * BLOCKSIZE + threadCol] = B[threadRow * N + threadCol];

        // block threads in this block until cache is fully populated
        __syncthreads();
        A += BLOCKSIZE;
        B += BLOCKSIZE * N;

        // execute the dotproduct on the currently cached block
        for (int dotIdx = 0; dotIdx < BLOCKSIZE; ++dotIdx) {
            tmp += As[threadRow * BLOCKSIZE + dotIdx] *
                   Bs[dotIdx * BLOCKSIZE + threadCol];
        }
        // need to sync again at the end, to avoid faster threads
        // fetching the next block into the cache before slower threads are done
        __syncthreads();
    }
    C[threadRow * N + threadCol] =
        alpha * tmp + beta * C[threadRow * N + threadCol];

}

// ---------------------------------------------------------------------------
// Host-side helper functions
// ---------------------------------------------------------------------------

__host__ void gemm_naive_cpu(int M, int N, int K, float alpha, const float* A, const float* B, float beta, float* C) {
    for (int m = 0; m < M; ++m) {
        for (int n = 0; n < N; ++n) {
            float tmp = 0.0f;
            for (int k = 0; k < K; ++k) {
                tmp += A[m * K + k] * B[k * N + n];
            }
            C[m * N + n] = alpha * tmp + beta * C[m * N + n];
        }
    }
}

__host__ bool verify_result(int M, int N, const float* C_kernel, const float* C_ref) {
    const float tolerance = 1e-3f;
    for (int i = 0; i < M * N; ++i) {
        if (std::abs(C_kernel[i] - C_ref[i]) > tolerance) {
            std::cerr << "Verification FAILED at index " << i << "! "
                      << "Kernel result: " << C_kernel[i]
                      << ", Reference result: " << C_ref[i] << std::endl;
            return false;
        }
    }
    std::cout << "Verification PASSED!" << std::endl;
    return true;
}

__host__ void fill_random(std::vector<float>& vec) {
    std::mt19937 gen(1337);
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    for (auto& val : vec) {
        val = dis(gen);
    }
}

// ---------------------------------------------------------------------------
// Main function (Host Code)
// ---------------------------------------------------------------------------
__host__ int main() {

    int M, N, K;
    M=512;
    N=512;
    K=512;

    const float alpha = 1.0f;
    const float beta = 0.0f;

    std::cout << "Running sgemm_naive with M=" << M << ", N=" << N << ", K=" << K << std::endl;

    std::vector<float> h_A(M * K);
    std::vector<float> h_B(K * N);
    std::vector<float> h_C(M * N, 0.0f);

    fill_random(h_A);
    fill_random(h_B);

    float *d_A, *d_B, *d_C;
    CHECK_CUDA_ERROR(cudaMalloc(&d_A, M * K * sizeof(float)));
    CHECK_CUDA_ERROR(cudaMalloc(&d_B, K * N * sizeof(float)));
    CHECK_CUDA_ERROR(cudaMalloc(&d_C, M * N * sizeof(float)));

    CHECK_CUDA_ERROR(cudaMemcpy(d_A, h_A.data(), M * K * sizeof(float), cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_B, h_B.data(), K * N * sizeof(float), cudaMemcpyHostToDevice));
    if (beta != 0.0f) {
        CHECK_CUDA_ERROR(cudaMemcpy(d_C, h_C.data(), M * N * sizeof(float), cudaMemcpyHostToDevice));
    }

    dim3 gridDim(CEIL_DIV(M, 32), CEIL_DIV(N, 32), 1);
    // 32 * 32 = 1024 thread per block
    dim3 blockDim(32, 32, 1);

    std::cout << "Running your CUDA kernel..." << std::endl;
    std::cout << "Grid size: (" <<gridDim.x << ", " <<gridDim.y << "), Block size: (" <<blockDim.x << ", " <<blockDim.y << ")" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    sgemm_naive<<<gridDim,blockDim>>>(M, N, K, alpha, d_A, d_B, beta, d_C);
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    auto end = std::chrono::high_resolution_clock::now();

    CHECK_CUDA_ERROR(cudaMemcpy(h_C.data(), d_C, M * N * sizeof(float), cudaMemcpyDeviceToHost));

    CHECK_CUDA_ERROR(cudaFree(d_A));
    CHECK_CUDA_ERROR(cudaFree(d_B));
    CHECK_CUDA_ERROR(cudaFree(d_C));

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Your kernel took: " << elapsed.count() << " ms" << std::endl;
    // The number of floating point operations is roughly 2 * M * N * K for the multiply-adds
    double gflops = (2.0 * M * N * K) / (elapsed.count() / 1000.0) / 1e9;
    std::cout << "Performance: " << gflops << " GFLOPS" << std::endl;

#ifdef VERIFY
    std::cout << "\nRunning reference CPU implementation for verification..." << std::endl;
    std::vector<float> h_C_ref(M * N, 0.0f);
    gemm_naive_cpu(M, N, K, alpha, h_A.data(), h_B.data(), beta, h_C_ref.data());
    verify_result(M, N, h_C.data(), h_C_ref.data());
#endif

    return 0;
}