# Dynamic Displacement Mapping on the GPU

**Arquitetura para Mapas Dinâmicos de Deslocamento em GPU Aplicada a Jogos Digitais**  
*Originalmente desenvolvido em 2012 (Dissertação de Mestrado) e modernizado para C++17 e OpenGL Core em 2026.*

---

## 📖 Sobre o Projeto
Este repositório contém o código-fonte da arquitetura desenvolvida para gerenciar, processar e renderizar **mapas dinâmicos de deslocamento em tempo real** diretamente na GPU. A solução foi projetada de forma modular e desacoplada, utilizando **OpenCL** para processar a física da deformação do relevo e **OpenGL** com *Tessellation* e *Geometry Shaders* para renderizar a malha geométrica refinada em tempo real.

O grande mérito técnico da arquitetura é a **interoperabilidade direta em VRAM (Zero-Copy)** entre as APIs de computação (OpenCL) e renderização (OpenGL), eliminando completamente o gargalo de transferência de dados entre CPU e GPU através do barramento PCIe.

---

## 🛠️ Arquitetura e Kernels
A arquitetura é dividida em 5 cenas de validação de *kernels* especializados:

1. **Contact Scene (Contato)**: Rastreamento e gravação contínua de marcas/trilhas deixadas por entidades móveis (esferas físicas) sobre o terreno.
2. **Force Scene (Forças)**: Simulação paramétrica de impactos localizados, explosões e crateras causadas por impulsos vetoriais tridimensionais.
3. **Morphing Scene (Transição)**: Interpolação temporal fluida entre duas topologias de relevos distintas (ex: desmoronamento ou abertura de passagens).
4. **Custom Scene (Escultura)**: Permite pintura e deformação interativa do terreno em tempo real.
5. **Test Scene (Pincel Interativo)**: Interface interativa guiada por um pincel (círculo indicador ao redor do cursor) que permite esculpir relevos positivos e negativos dinamicamente.

---

## 📊 Comparativo Técnico: 2012 vs 2026
A tabela abaixo resume a profunda modernização feita na arquitetura original para viabilizar sua compilação e execução em hardware e sistemas modernos:

| Recurso / Módulo | Estado Original (2012) | Estado Atualizado (2026) |
| :--- | :--- | :--- |
| **Arquitetura de Compilação** | 32-bits (x86) com código *Assembly* inline | **64-bits (x64)** nativo com padrão **C++17** |
| **Gerenciador de Janela e Loop** | **OpenFrameworks 0071** + GLUT legados | **GLFW 3.5** (executável puro e leve) |
| **Carregamento de Texturas** | Classe proprietária `ofImage` (OpenFrameworks) | **`stb_image.h`** (single-header nativo e moderno) |
| **Cálculo de Tempo** | Função interna `ofGetElapsedTimeMillis()` | **`std::chrono`** da biblioteca padrão do C++ |
| **Pipeline Gráfico (OpenGL)** | OpenGL 4.0 (Immediate Mode / Função Fixa) | **OpenGL 4.2 Core Profile** |
| **Envio de Geometria** | `glBegin(GL_PATCHES)` / `glEnd()` legados | **VAO** (*Vertex Array Object*) e **VBO** (*VertexBuffer*) |
| **Cálculos Matemáticos** | Matrizes legadas do OpenGL (`glMatrixMode`, `gluPerspective`) | **GLM** (*OpenGL Mathematics*), otimizada para CPU/GPU |
| **Shader Language** | GLSL legado (com variáveis embutidas obsoletas) | **GLSL 420 Core** (atributos explícitos e MVP uniforme) |
| **OpenCL API** | Wrapper `cl.hpp` obsoleto (OpenCL 1.1) | **`opencl.hpp` moderno (OpenCL 2.0+)** |
| **Gerenciamento de Build** | Projeto estático do Visual Studio 2010 | **CMake** multiplataforma com dependências via **vcpkg** |

---

## 🚀 Como Compilar e Executar

### Pré-requisitos
* **Visual Studio 2022** (com C++ desktop habilitado).
* **CMake 3.20+** instalado.
* **vcpkg** configurado globalmente no sistema.
* **NVIDIA CUDA Toolkit** instalado (para drivers de OpenCL).

### Compilando via Terminal
1. No diretório raiz do projeto, configure os arquivos de build:
   ```powershell
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[Caminho/Para/vcpkg]/scripts/buildsystems/vcpkg.cmake
   ```
2. Realize a compilação do executável:
   ```powershell
   cmake --build build --config Release
   ```
3. O executável estará pronto na pasta `build/Release/DisplacementStage.exe`.

### Rodando pelo Visual Studio 2022
1. Abra o arquivo de solução **`build/DisplacementStage.sln`** no VS2022.
2. Clique com o botão direito no projeto **`DisplacementStage`** e selecione **"Set as Startup Project"**.
3. Defina a configuração de compilação como **`Release`**.
4. Pressione **F5** para iniciar a depuração interativa.

---

## 📂 Documentação Oficial (Docs)
Os arquivos científicos originais do mestrado estão catalogados e linkados na pasta `/docs`:
* 📄 **Dissertação de Mestrado (PDF)**: [docs/dissertacao_FCA.pdf](docs/dissertacao_FCA.pdf)
* 📄 **Artigo Científico SBGames (PDF)**: [docs/sbgames_FCA.pdf](docs/sbgames_FCA.pdf)
* 📝 **Transcrição Completa em Markdown**: [docs/dissertacao/arquitetura_mapas_dinamicos_deslocamento_gpu.md](docs/dissertacao/arquitetura_mapas_dinamicos_deslocamento_gpu.md)

---

## 📸 Demonstração dos Kernels

### 1. Contact Kernel (Deformação por Contato)
`![Contact Kernel](docs/figures/scene1.png)`

### 2. Force Kernel (Impacto de Forças)
`![Force Kernel](docs/figures/scene2.png)`

### 3. Morphing Kernel (Transições de Topologia)
`![Morphing Kernel](docs/figures/scene3.png)`

### 4. Custom Kernel (Pintura de Relevo)
`![Custom Kernel](docs/figures/scene4.png)`

### 5. Test Scene (Escultura Interativa)
`![Test Scene](docs/figures/scene5.png)`
