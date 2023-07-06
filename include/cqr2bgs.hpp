#include <iostream>
#include <vector>
#include <string>
#include <memory>

#ifdef GPU
    #include <cublas_v2.h>
    #include <cuda_runtime.h>
    #include <cusolverDn.h>
#endif

#include "internals/distributedmatrix.hpp"
#include "internals/validate.hpp"
#include "internals/timing.hpp"

#ifdef GPU
    #include "internals/cudamemory.hpp"
    #include "internals/utils.hpp"
#endif

#pragma once

namespace cqr
{

    class qr2bgs{
    public:
        qr2bgs(std::int64_t m, std::int64_t n, std::int64_t panel_size);
        qr2bgs(std::int64_t m, std::int64_t n, std::size_t panel_num);
        ~qr2bgs();

        void InputMatrix(std::vector<double> &A);
        void InputMatrix(double *A);
        void InputMatrix(std::string filename);

        void Start();
        void Validate_output();

     private:
#ifdef GPU
        void cqr2bgs(cudamemory<double> &A, cudamemory<double> &R);
        void cqrbgs(cudamemory<double> &A, cudamemory<double> &R);
#else
        void cqr2bgs(std::vector<double> &A, std::vector<double> &R);
        void cqrbgs(std::vector<double> &A, std::vector<double> &R);
#endif
        void gramMatrix(double *A, double *R, double *tmp);
        void cholesky(double *B);
        void calculateQ(double *A, double *R);
        void updateMatrix(int n, int ldw, double *A, double *R);
        float get_time();

        std::int64_t n_, m_, localm_, block_size_;
        std::int64_t input_panel_size_, panel_size_;
        std::int64_t size = 1;
        std::string filename_;

        std::vector<double> A_;
        std::vector<double> Alocal_;
        std::vector<double> R_;

#ifdef GPU
        cudamemory<double> cudaAlocal_;
        cudamemory<double> cudaR_, cudaR1_, cudaR2_;
        cudamemory<double> cudaI_;
        cudamemory<double> cudatmp_, cudaWtmp_;
#endif       
        std::unique_ptr<DistributedMatrix> distmatrix;
        std::unique_ptr<Validate> validate;
#ifdef GPU
        std::unique_ptr<TimingGpu> timing;
#endif

        double orthogonality_, residuals_;

        float time_of_execution_ = 0;

        MPI_Comm mpi_comm_;
        MPI_Comm shmcomm_;
        int world_size_;
        int world_rank_;
        int shmsize_;
        int shmrank_;

#ifdef GPU
        cublasHandle_t cublashandle_;
        cusolverDnHandle_t cusolverhandle_;
#endif
    };

}