#include <fstream>
#include <vector>
#include <iostream>

#include "utils.hpp"
#include "cudamemory.hpp"
 
template class cudamemory<int>;
template class cudamemory<double>;

template <typename T> cudamemory<T>::cudamemory(){
    cudaptr = nullptr;
}

template <typename T> cudamemory<T>::cudamemory(std::int64_t m) : m_(m){
    CUDA_CHECK(cudaMalloc((void**) &cudaptr, m_ * sizeof(T)));
}

template <typename T> cudamemory<T>::cudamemory(std::vector<T> &vec){
    m_ = vec.size();
    CUDA_CHECK(cudaMalloc((void**) &cudaptr, m_ * sizeof(T)));
}

template <typename T> cudamemory<T>::~cudamemory(){
    if(cudaptr != nullptr)
    {
        CUDA_CHECK(cudaFree(cudaptr));
    }
}

template <typename T> T*  cudamemory<T>::data(){
    return cudaptr;
}

template <typename T> void cudamemory<T>::resize(std::int64_t m) {
    m_ = m;
    CUDA_CHECK(cudaMalloc((void**) &cudaptr, m_ * sizeof(T)));
}

template <typename T> void cudamemory<T>::memset(){
    CUDA_CHECK(cudaMemset ( cudaptr, 0, m_ * sizeof(double) ));
}

template <typename T> void cudamemory<T>::memset(T value){
    std::vector<T> tmp(m_, value);
    CUDA_CHECK(cudaMemcpy ( cudaptr, tmp.data(), m_ * sizeof(T), cudaMemcpyHostToDevice));
}

template void cudamemory<int>::memset(int );
template void cudamemory<double>::memset(double);


template <typename T> void cudamemory<T>::copytohost(std::vector<T> &host){
    CUDA_CHECK(cudaMemcpy ( host.data(), cudaptr, m_ * sizeof(T), cudaMemcpyDeviceToHost));
}


template <typename T> void cudamemory<T>::copytodevice(std::vector<T> &host){
    CUDA_CHECK(cudaMemcpy ( cudaptr, host.data(), m_ * sizeof(T), cudaMemcpyHostToDevice));
}

template <typename T> void cudamemory<T>::savematrix(const char* filename)
{
    std::vector<T> Buff_tmp(m_);
    CUDA_CHECK(cudaMemcpy ( Buff_tmp.data(), cudaptr, m_ * sizeof(T), cudaMemcpyDeviceToHost));

    std::ofstream file(filename, std::ios::out | std::ios::binary);

    if (file.is_open())
    {   
        for(int i=0; i< m_; ++i){
            file.write( reinterpret_cast<char*>( &Buff_tmp[i] ), sizeof(double) );
        }

    }   
    else 
        std::cout << "File not opened!!!" << std::endl;
        file.close();
}