#include "DisplacementStage.h"
#include <Windows.h>   // wglGetCurrentContext / wglGetCurrentDC

DisplacementStage::DisplacementStage(const std::string& fileName)
{
    try {
        // Obter plataformas disponíveis
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.empty()) {
            std::cerr << "DisplacementStage: nenhuma plataforma OpenCL encontrada!\n";
            return;
        }

        // Propriedades de contexto para interoperabilidade OpenCL/OpenGL
        // wglGetCurrentContext() retorna o contexto GL ativo (Windows)
        cl_context_properties cps[] = {
            CL_GL_CONTEXT_KHR,   (cl_context_properties)wglGetCurrentContext(),
            CL_WGL_HDC_KHR,      (cl_context_properties)wglGetCurrentDC(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
            0
        };

        // Criar contexto compartilhado com OpenGL
        this->context = new cl::Context(CL_DEVICE_TYPE_GPU, cps);

        // Listar dispositivos do contexto
        std::vector<cl::Device> devices = this->context->getInfo<CL_CONTEXT_DEVICES>();
        if (devices.empty()) {
            std::cerr << "DisplacementStage: nenhum dispositivo GPU encontrado!\n";
            return;
        }

        std::cout << "OpenCL device: "
                  << devices[0].getInfo<CL_DEVICE_NAME>() << "\n";

        // Criar fila de comandos com profiling (para medição de tempo)
        queue = new cl::CommandQueue(*this->context, devices[0],
                                     CL_QUEUE_PROFILING_ENABLE);

        // Ler e compilar o arquivo de kernel .cl
        std::ifstream inputFile(fileName);
        if (!inputFile.is_open()) {
            std::cerr << "DisplacementStage: não foi possível abrir '" << fileName << "'\n";
            return;
        }
        std::string sourceStr((std::istreambuf_iterator<char>(inputFile)),
                              std::istreambuf_iterator<char>());

        cl::Program::Sources sourceCode;
        sourceCode.push_back({ sourceStr.c_str(), sourceStr.length() });
        this->program = new cl::Program(*this->context, sourceCode);
        program->build(devices);

        this->list_size = 0;
        this->kernel    = nullptr;
        this->startNullBuffers();

    } catch (cl::Error& e) {
        std::cerr << "DisplacementStage::DisplacementStage() CL Error: "
                  << e.what() << " (" << e.err() << ")\n";
        // Tentar obter log de build se for erro de compilação
        if (e.err() == CL_BUILD_PROGRAM_FAILURE && program) {
            std::vector<cl::Device> devs = context->getInfo<CL_CONTEXT_DEVICES>();
            if (!devs.empty())
                std::cerr << program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(devs[0]) << "\n";
        }
    }
}

DisplacementStage::~DisplacementStage(void)
{
    releaseContactBuffer();
    releaseForceBuffer();
    releaseMorphingBuffer();
    releaseCustomBuffer();

    if (kernel)  { delete kernel;  kernel  = nullptr; }
    if (program) { delete program; program = nullptr; }
    if (queue)   { delete queue;   queue   = nullptr; }
    if (context) { delete context; context = nullptr; }
}

void DisplacementStage::startNullBuffers(void)
{
    bufferPosition = bufferDepth    = bufferRadius    =
    bufferPositionF = bufferForce   = bufferTime      =
    bufferCustom1  = bufferCustom2  = bufferCustom3   = bufferCustom4 = nullptr;
}

// ── Seleção de kernel ──────────────────────────────────────────

void DisplacementStage::setKernel(const std::string& kernelName)
{
    try {
        if (kernel) { delete kernel; kernel = nullptr; }
        kernel = new cl::Kernel(*program, kernelName.c_str());
    } catch (cl::Error& e) {
        std::cerr << "setKernel: " << e.what() << " (" << e.err() << ")\n";
    }
}

void DisplacementStage::setKernel(KernelType tag)
{
    activeKernelType = tag;
    switch (tag) {
        case KRNL_CONTACT:  setKernel("contactKernel");  break;
        case KRNL_FORCE:    setKernel("forceKernel");    break;
        case KRNL_MORPHING: setKernel("morphingKernel"); break;
        case KRNL_CUSTOM:   setKernel("customKernel");   break;
    }
}

// ── Criação de buffers ─────────────────────────────────────────

void DisplacementStage::createMemoryBufferToContactKernel(void)
{
    try {
        bufferPosition = new cl::Buffer(*context, CL_MEM_READ_ONLY, 2 * sizeof(float));
        bufferDepth    = new cl::Buffer(*context, CL_MEM_READ_ONLY,     sizeof(float));
        bufferRadius   = new cl::Buffer(*context, CL_MEM_READ_ONLY,     sizeof(float));
    } catch (cl::Error& e) {
        std::cerr << "createMemoryBufferToContactKernel: " << e.what() << " (" << e.err() << ")\n";
    }
}

void DisplacementStage::createMemoryBufferToForcetKernel(void)
{
    try {
        bufferPositionF = new cl::Buffer(*context, CL_MEM_READ_ONLY, 2 * sizeof(float));
        bufferForce     = new cl::Buffer(*context, CL_MEM_READ_ONLY, 4 * sizeof(float));
    } catch (cl::Error& e) {
        std::cerr << "createMemoryBufferToForcetKernel: " << e.what() << " (" << e.err() << ")\n";
    }
}

void DisplacementStage::createMemoryBufferToMorphingtKernel(void)
{
    try {
        bufferTime = new cl::Buffer(*context, CL_MEM_READ_ONLY, sizeof(float));
    } catch (cl::Error& e) {
        std::cerr << "createMemoryBufferToMorphingtKernel: " << e.what() << " (" << e.err() << ")\n";
    }
}

void DisplacementStage::createMemoryBufferToCustomKernel(void)
{
    try {
        bufferCustom1 = new cl::Buffer(*context, CL_MEM_READ_ONLY, 4 * sizeof(float));
        bufferCustom2 = new cl::Buffer(*context, CL_MEM_READ_ONLY, 4 * sizeof(float));
        bufferCustom3 = new cl::Buffer(*context, CL_MEM_READ_ONLY, 4 * sizeof(float));
        bufferCustom4 = new cl::Buffer(*context, CL_MEM_READ_ONLY, 4 * sizeof(float));
    } catch (cl::Error& e) {
        std::cerr << "createMemoryBufferToCustomKernel: " << e.what() << " (" << e.err() << ")\n";
    }
}

// ── Configuração de argumentos ─────────────────────────────────

template<class T>
void DisplacementStage::setArg(cl_uint index, T value)
{
    kernel->setArg<T>(index, value);
}

void DisplacementStage::setContactArg(void)
{
    setArg(0, getDisplacementMap());
    setArg(1, getDisplacementMap());
    setArg(2, getBufferPosition());
    setArg(3, getBufferDepth());
    setArg(4, getBufferRadius());
}

void DisplacementStage::setForceArg(void)
{
    setArg(0, getDisplacementMap());
    setArg(1, getDisplacementMap());
    setArg(2, getBufferPositionF());
    setArg(3, getBufferForce());
}

void DisplacementStage::setMorphingArg(void)
{
    setArg(0, getSourceMorphing());
    setArg(1, getDestinyMorphing());
    setArg(2, getDisplacementMap());
    setArg(3, getBufferTime());
}

void DisplacementStage::setCustomArg(void)
{
    setArg(0, getDisplacementMap());
    setArg(1, getDisplacementMap());
    setArg(2, getBufferCustom1());
    setArg(3, getBufferCustom2());
    setArg(4, getBufferCustom3());
    setArg(5, getBufferCustom4());
}

// ── Cópia CPU → GPU ────────────────────────────────────────────

void DisplacementStage::copyContactToMemory(float* position, float* depth, float* radius)
{
    try {
        float tempPos[2] = { position[0], 64.f - position[1] };
        queue->enqueueWriteBuffer(getBufferPosition(), CL_TRUE, 0, 2*sizeof(float), tempPos);
        queue->enqueueWriteBuffer(getBufferDepth(),    CL_TRUE, 0,   sizeof(float), depth);
        queue->enqueueWriteBuffer(getBufferRadius(),   CL_TRUE, 0,   sizeof(float), radius);
    } catch (cl::Error& e) {
        std::cerr << "copyContactToMemory: " << e.what() << " (" << e.err() << ")\n";
    }
}

void DisplacementStage::copyForceToMemory(float* position, float* force)
{
    try {
        float tempPos[2] = { position[0], 64.f - position[1] };
        queue->enqueueWriteBuffer(getBufferPositionF(), CL_TRUE, 0, 2*sizeof(float), tempPos);
        queue->enqueueWriteBuffer(getBufferForce(),     CL_TRUE, 0, 4*sizeof(float), force);
    } catch (cl::Error& e) {
        std::cerr << "copyForceToMemory: " << e.what() << " (" << e.err() << ")\n";
    }
}

void DisplacementStage::copyMorphingToMemory(float* time)
{
    try {
        queue->enqueueWriteBuffer(getBufferTime(), CL_TRUE, 0, sizeof(float), time);
    } catch (cl::Error& e) {
        std::cerr << "copyMorphingToMemory: " << e.what() << " (" << e.err() << ")\n";
    }
}

void DisplacementStage::copyCustomToMemory(float* a1, float* a2, float* a3, float* a4)
{
    try {
        float tempA1[4] = { a1[0], 64.f - a1[1], a1[2], a1[3] };
        queue->enqueueWriteBuffer(getBufferCustom1(), CL_TRUE, 0, 4*sizeof(float), tempA1);
        queue->enqueueWriteBuffer(getBufferCustom2(), CL_TRUE, 0, 4*sizeof(float), a2);
        queue->enqueueWriteBuffer(getBufferCustom3(), CL_TRUE, 0, 4*sizeof(float), a3);
        queue->enqueueWriteBuffer(getBufferCustom4(), CL_TRUE, 0, 4*sizeof(float), a4);
    } catch (cl::Error& e) {
        std::cerr << "copyCustomToMemory: " << e.what() << " (" << e.err() << ")\n";
    }
}

// ── Execução do kernel ─────────────────────────────────────────

void DisplacementStage::runKernel(void)
{
    try {
        cl::NDRange global(64, 64);
        cl::NDRange local(1, 1);
        cl::Event ev;

        // Adquirir texturas GL para uso pelo OpenCL
        std::vector<cl::Memory> memObjs;
        memObjs.push_back(displacementMap);
        if (activeKernelType == KRNL_MORPHING) {
            memObjs.push_back(sourceMorphing);
            memObjs.push_back(destinyMorphing);
        }

        cl_int result = queue->enqueueAcquireGLObjects(&memObjs, nullptr, &ev);
        ev.wait();
        if (result != CL_SUCCESS) std::cerr << "AcquireGLObjects: " << result << "\n";

        result = queue->enqueueNDRangeKernel(*kernel, cl::NullRange, global, local, nullptr, &ev);
        if (result != CL_SUCCESS) std::cerr << "NDRangeKernel: " << result << "\n";

        result = queue->enqueueReleaseGLObjects(&memObjs, nullptr, &ev);
        ev.wait();
        if (result != CL_SUCCESS) std::cerr << "ReleaseGLObjects: " << result << "\n";

    } catch (cl::Error& e) {
        std::cerr << "runKernel: " << e.what() << " (" << e.err() << ")\n";
    }
}

// ── Setters / Getters ──────────────────────────────────────────

void DisplacementStage::setListSize(int value) { list_size = value; }

cl::Image2DGL DisplacementStage::getDisplacementMap(void) { return displacementMap; }
cl::Image2DGL DisplacementStage::getSourceMorphing(void)  { return sourceMorphing;  }
cl::Image2DGL DisplacementStage::getDestinyMorphing(void) { return destinyMorphing; }

cl::Buffer DisplacementStage::getBufferPosition(void)  { return *bufferPosition; }
cl::Buffer DisplacementStage::getBufferDepth(void)     { return *bufferDepth;    }
cl::Buffer DisplacementStage::getBufferRadius(void)    { return *bufferRadius;   }
cl::Buffer DisplacementStage::getBufferPositionF(void) { return *bufferPositionF;}
cl::Buffer DisplacementStage::getBufferForce(void)     { return *bufferForce;    }
cl::Buffer DisplacementStage::getBufferTime(void)      { return *bufferTime;     }
cl::Buffer DisplacementStage::getBufferCustom1(void)   { return *bufferCustom1;  }
cl::Buffer DisplacementStage::getBufferCustom2(void)   { return *bufferCustom2;  }
cl::Buffer DisplacementStage::getBufferCustom3(void)   { return *bufferCustom3;  }
cl::Buffer DisplacementStage::getBufferCustom4(void)   { return *bufferCustom4;  }

// ── Release de buffers ─────────────────────────────────────────

void DisplacementStage::releaseContactBuffer(void)
{
    if (bufferPosition) { delete bufferPosition; bufferPosition = nullptr; }
    if (bufferDepth)    { delete bufferDepth;    bufferDepth    = nullptr; }
    if (bufferRadius)   { delete bufferRadius;   bufferRadius   = nullptr; }
}

void DisplacementStage::releaseForceBuffer(void)
{
    if (bufferPositionF) { delete bufferPositionF; bufferPositionF = nullptr; }
    if (bufferForce)     { delete bufferForce;     bufferForce     = nullptr; }
}

void DisplacementStage::releaseMorphingBuffer(void)
{
    if (bufferTime) { delete bufferTime; bufferTime = nullptr; }
}

void DisplacementStage::releaseCustomBuffer(void)
{
    if (bufferCustom1) { delete bufferCustom1; bufferCustom1 = nullptr; }
    if (bufferCustom2) { delete bufferCustom2; bufferCustom2 = nullptr; }
    if (bufferCustom3) { delete bufferCustom3; bufferCustom3 = nullptr; }
    if (bufferCustom4) { delete bufferCustom4; bufferCustom4 = nullptr; }
}
