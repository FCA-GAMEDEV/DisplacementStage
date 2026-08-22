#pragma once

// OpenCL moderno — wrapper C++ Khronos (opencl.hpp substitui o antigo cl.hpp)
// O header fica em vendor/CL/opencl.hpp
#define CL_HPP_ENABLE_EXCEPTIONS          // equivalente ao antigo __CL_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 120  // OpenCL 1.2 (suportado pelo driver NVIDIA)
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS // Necessário para cl::Image2DGL
#include <CL/cl_gl.h>
#include <CL/opencl.hpp>

#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class DisplacementStage
{
public:
    friend class Interoperability;

    enum KernelType {
        KRNL_CONTACT,
        KRNL_FORCE,
        KRNL_MORPHING,
        KRNL_CUSTOM
    };

    DisplacementStage(const std::string& fileName);
    ~DisplacementStage(void);

    // Escolher kernel
    void setKernel(const std::string& kernelName);
    void setKernel(KernelType tag);

    // Preparar buffers de memória
    void createMemoryBufferToContactKernel(void);
    void createMemoryBufferToForcetKernel(void);
    void createMemoryBufferToMorphingtKernel(void);
    void createMemoryBufferToCustomKernel(void);

    // Configurar argumentos
    void setContactArg(void);
    void setForceArg(void);
    void setMorphingArg(void);
    void setCustomArg(void);

    // Copiar valores para GPU
    void copyContactToMemory (float* position, float* depth, float* radius);
    void copyForceToMemory   (float* position, float* force);
    void copyMorphingToMemory(float* time);
    void copyCustomToMemory  (float*, float*, float*, float*);

    // Executar o kernel
    void runKernel(void);

    void setListSize(int value);

    cl::Image2DGL getDisplacementMap(void);
    cl::Image2DGL getSourceMorphing(void);
    cl::Image2DGL getDestinyMorphing(void);

    // Liberar buffers
    void releaseContactBuffer(void);
    void releaseForceBuffer(void);
    void releaseMorphingBuffer(void);
    void releaseCustomBuffer(void);

private:
    template<class T> void setArg(cl_uint index, T value);

    void startNullBuffers(void);

    // Getters de buffer internos
    cl::Buffer getBufferPosition(void);
    cl::Buffer getBufferDepth(void);
    cl::Buffer getBufferRadius(void);
    cl::Buffer getBufferPositionF(void);
    cl::Buffer getBufferForce(void);
    cl::Buffer getBufferTime(void);
    cl::Buffer getBufferCustom1(void);
    cl::Buffer getBufferCustom2(void);
    cl::Buffer getBufferCustom3(void);
    cl::Buffer getBufferCustom4(void);

    // Objetos OpenCL (ponteiros para facilitar null-check e delete)
    cl::Context*      context  = nullptr;
    cl::CommandQueue* queue    = nullptr;
    cl::Kernel*       kernel   = nullptr;
    cl::Program*      program  = nullptr;

    int list_size = 0;

    // Buffers por kernel
    cl::Buffer* bufferPosition  = nullptr;
    cl::Buffer* bufferDepth     = nullptr;
    cl::Buffer* bufferRadius    = nullptr;
    cl::Buffer* bufferPositionF = nullptr;
    cl::Buffer* bufferForce     = nullptr;
    cl::Buffer* bufferTime      = nullptr;
    cl::Buffer* bufferCustom1   = nullptr;
    cl::Buffer* bufferCustom2   = nullptr;
    cl::Buffer* bufferCustom3   = nullptr;
    cl::Buffer* bufferCustom4   = nullptr;

    // Texturas compartilhadas OpenCL/OpenGL
    cl::Image2DGL displacementMap;
    cl::Image2DGL sourceMorphing;
    cl::Image2DGL destinyMorphing;

    // Guarda o tipo de kernel ativo para runKernel saber quais texturas adquirir
    KernelType activeKernelType = KRNL_CONTACT;
};
