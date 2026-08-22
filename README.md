# DisplacementStage: Dynamic Displacement Mapping on GPU

> **Arquitetura extensível e de alto desempenho para geração e renderização de mapas dinâmicos de deslocamento na GPU aplicada a Jogos Digitais.**

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.2_Core-green.svg)](https://www.opengl.org/)
[![OpenCL](https://img.shields.io/badge/OpenCL-2.0%2B-orange.svg)](https://www.khronos.org/opencl/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-red.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows_x64-lightgrey.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

---

## 📌 Sobre o Projeto

Este repositório contém o código-fonte da arquitetura desenvolvida para gerenciar, processar e renderizar **mapas dinâmicos de deslocamento em tempo real diretamente na GPU**. A solução foi projetada de forma modular e desacoplada, utilizando **OpenCL** para processar a física da deformação do relevo e **OpenGL com Hardware Tessellation e Geometry Shaders** para renderizar a malha geométrica refinada em tempo real.

Originalmente concebido e implementado entre **2010 e 2012** como projeto de Dissertação de Mestrado em Computação Gráfica na Universidade Federal Fluminense (UFF), o projeto foi integralmente modernizado em **2026** para os padrões **C++17**, **OpenGL 4.2 Core Profile** e **OpenCL 2.0+**, eliminando dependências legadas e adotando um pipeline de compilação modular via CMake.

---

## 🏛️ Arquitetura em 3 Camadas e Separação de Responsabilidades

O sistema foi estruturado sobre o princípio de **Alta Coesão e Baixo Acoplamento**, dividindo o pipeline em três módulos bem delimitados:

1. **Displacement Generator (OpenCL):** Responsável estritamente pela física e regras matemáticas de deformação (cálculo de forças, raios de impacto, dissipação e deformação contínua).
2. **Interoperability Manager (Zero-Copy Bridge):** Gerencia a sincronização de contexto e compartilhamento direto de texturas em VRAM (`cl::Image2DGL`), eliminando o tráfego de dados pelo barramento PCIe.
3. **Displacement Manager (OpenGL):** Responsável exclusivamente pela renderização geométrica, subdivisão dinâmica de malha por hardware (*Hardware Tessellation*) e iluminação gráfica.

```
       +-------------------------------------------------------+
       |                       GPU VRAM                        |
       |                                                       |
       |  [Camada 1: Displacement Generator - OpenCL]          |
       |   • Contato / Forças / Morphing / Pincel              |
       |           │                                           |
       |           ▼ (Zero-Copy / Shared Texture)              |
       |  [Camada 2: Interoperability Bridge - cl::Image2DGL]  |
       |           │                                           |
       |           ▼                                           |
       |  [Camada 3: Displacement Manager - OpenGL 4.2 Core]   |
       |   • Vertex & Tessellation Control Shader              |
       |   • Tessellation Evaluation (Deslocamento de Malha)   |
       |   • Geometry & Fragment Shader (Normais/Iluminação)   |
       +-------------------------------------------------------+
                                   │
                                   ▼
                              [Display]
```

### 💡 Prova Histórica de Manutenibilidade (14 anos depois)
A robustez desta separação em 3 camadas foi colocada à prova durante o processo de modernização em 2026 (14 anos após a concepção original). A camada inteira de visualização e janela gráfica pôde ser substituída (removendo o framework OpenFrameworks e o Immediate Mode legado em favor de OpenGL 4.2 Core, GLFW e GLM) **sem a necessidade de alterar uma única linha de lógica dos algoritmos matemáticos ou dos kernels OpenCL (`displacementStage.cl`) originais**.

---

## 🧪 Cenas e Kernels de Validação

A arquitetura desacopla os algoritmos de deformação através de 5 cenas/kernels especializados:

### 1. Contact Scene (Cena 1 - Contato)
Rastreamento e gravação contínua de marcas e trilhas deixadas por entidades móveis (esferas físicas) sobre o terreno.
![Contact Scene](docs/figures/scene1.png)

### 2. Force Scene (Cena 2 - Forças e Impactos)
Simulação paramétrica de deformações causadas por vetores de força e impulsos pontuais tridimensionais (crateras e explosões).
![Force Scene](docs/figures/scene2.png)

### 3. Morphing Scene (Cena 3 - Transição Topológica)
Interpolação temporal contínua e fluida entre duas matrizes de relevo distintas.
![Morphing Scene](docs/figures/scene3.png)

### 4. Custom Scene (Cena 4 - Esqueleto Didático de Customização)
Ponto de partida desacoplado (princípio *Open-Closed*) para desenvolvedores implementarem novos kernels customizados sem alterar o motor gráfico principal.
![Custom Scene](docs/figures/scene4.png)

### 5. Test Scene (Cena 5 - Escultura Interativa)
Ferramenta de edição e escultura em tempo real guiada por pincel circular, suportando deformações positivas (elevação) e negativas (rebaixamento/cratera) multicanal (RGB).
![Test Scene](docs/figures/scene5.png)

---

## ⚙️ Comparativo de Modernização: 2012 vs 2026

| Recurso / Módulo | Estado Original (2012) | Estado Modernizado (2026) |
| :--- | :--- | :--- |
| **Arquitetura de Compilação** | 32-bits (x86) com Assembly inline | 64-bits (x64) nativo em C++17 |
| **Gerenciador de Janela e Loop** | OpenFrameworks 0071 + GLUT legados | GLFW 3.5 puro e desacoplado |
| **Pipeline Gráfico (OpenGL)** | OpenGL 4.0 (Immediate Mode / Funções Fixas) | OpenGL 4.2 Core Profile nativo |
| **Envio de Geometria** | `glBegin(GL_PATCHES)` / `glEnd()` legados | VAO (Vertex Array) e VBO (VertexBuffer) |
| **Cálculos Matemáticos** | Matrizes legadas (`glMatrixMode`, `gluPerspective`) | GLM (OpenGL Mathematics) otimizada |
| **Shader Language** | GLSL legado com variáveis embutidas | GLSL 420 Core com uniform blocks e layouts explícitos |
| **OpenCL API** | Wrapper `cl.hpp` obsoleto (OpenCL 1.1) | Wrapper `opencl.hpp` moderno (OpenCL 2.0+) |
| **Carregamento de Texturas** | Classe proprietária `ofImage` (OpenFrameworks) | `stb_image.h` nativo em C/C++ |
| **Cálculo de Tempo** | Função interna `ofGetElapsedTimeMillis()` | `std::chrono` da biblioteca padrão C++ |
| **Gerenciamento de Build** | Solução estática do Visual Studio 2010 | CMake 3.20+ integrado ao vcpkg |

---

## 🎮 Guia de Controles (Teclado e Mouse)

### Navegação Geral
* **`1` a `5`:** Alterna entre as cenas de validação dos kernels.
* **`Espaço`:** Retorna ao Menu Principal (Cena 0).
* **`ESC`:** Fecha a aplicação imediatamente.

### Câmera e Visualização
* **`Setas (↑ ↓ ← →)`:** Movimentam a câmera pelo espaço 3D (frente, trás e laterais).
* **`Page Up` / `Page Down`:** Eleva ou abaixa a altura da câmera (Eixo Y).
* **`W`:** Alterna a exibição em modo *Wireframe* (linhas de grade).
* **`R`:** Liga/Desliga a rotação automática da malha 3D.
* **`P`:** Pausa/Retoma o cálculo de deformação física no OpenCL.
* **`S`:** Salva captura em alta definição (.png) da textura de deformação atual na pasta `bin/data/dmap/`.

### Escultura e Pincel Interativo (Cena 5 - Test Scene)
* **`D`:** Liga/Desliga a exibição do pincel circular interativo e do mini-mapa 2D.
* **`Scroll do Mouse` ou `+ / -`:** Ajusta o raio de ação do pincel de deformação.
* **`Clique Esquerdo + Arrastar`:** Aplica deformação positiva (elevação/escultura do relevo).
* **`Clique Direito + Arrastar`:** Aplica deformação negativa (depressão/escultura de cratera).
* **`8`, `9` ou `0`:** Modifica a deformação física do canal selecionado (Vermelho, Verde ou Azul).

### Controle de Tesselação Dinâmica (GPU)
* **`F1` / `F2`:** Diminui / Aumenta o fator de Tessellation Interno.
* **`F3` / `F4`:** Diminui / Aumenta o fator de Tessellation Externo.
* **`F5` / `F6`:** Ajusta ambos os fatores simultaneamente.

---

## 🛠️ Como Compilar e Executar

### Pré-requisitos
* **Visual Studio 2022** (com suporte a Desktop C++).
* **CMake 3.20+** instalado.
* **vcpkg** instalado e configurado globalmente (`vcpkg integrate install`).
* **Driver NVIDIA com suporte a OpenCL** (ou NVIDIA CUDA Toolkit).

### Opção 1: Compilação via Terminal (CMake)

```bash
# 1. Configurar o diretório de build apontando para o toolchain do vcpkg
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[Caminho/Para/vcpkg]/scripts/buildsystems/vcpkg.cmake

# 2. Compilar o executável otimizado em Release
cmake --build build --config Release

# 3. Executar o binário gerado
./build/Release/DisplacementStage.exe
```

### Opção 2: Pelo Visual Studio 2022 (IDE)
1. Abra a solução gerada em `build/DisplacementStage.sln` no Visual Studio 2022.
2. No *Solution Explorer*, clique com o botão direito no projeto **`DisplacementStage`** e selecione **"Set as Startup Project"**.
3. Altere o modo de build no menu superior para **`Release`** (`x64`).
4. Pressione **`F5`** (ou clique em *Local Windows Debugger*). O diretório de trabalho é configurado automaticamente pelo CMake para encontrar a pasta `data/`.

---

## 📂 Estrutura do Repositório

```
DisplacementStage/
├── bin/data/              # Shaders (GLSL), texturas e mapas de altura
├── docs/                  # Dissertação de Mestrado, artigos e documentação técnica
│   ├── dissertacao_FCA.pdf
│   ├── sbgames_FCA.pdf
│   └── arquitetura_mapas_dinamicos_deslocamento_gpu.md
├── src/                   # Código-fonte em C++ e kernels OpenCL (.cl)
│   ├── DisplacementStage.cpp
│   ├── displacementStage.cl
│   └── ...
├── vendor/                # Dependências de cabeçalho único (stb_image, etc.)
├── CMakeLists.txt         # Configuração de build multiplataforma
└── README.md              # Documentação principal
```

---

## 📚 Documentação Científica Oficial

Os arquivos científicos originais do mestrado estão catalogados e linkados na pasta `/docs`:
* 📄 **Dissertação de Mestrado (PDF):** [docs/dissertacao_FCA.pdf](docs/dissertacao_FCA.pdf)
* 📄 **Artigo Científico SBGames (PDF):** [docs/sbgames_FCA.pdf](docs/sbgames_FCA.pdf)

---

## 📚 Publicações e Citação

Caso este projeto ou os conceitos desta arquitetura sejam utilizados em pesquisas acadêmicas ou projetos técnicos, utilize a citação abaixo:

```bibtex
@mastersthesis{andrade2012displacement,
  title     = {Arquitetura extensível para mapas dinâmicos de deslocamento na GPU em jogos digitais},
  author    = {Andrade, Fábio C.},
  year      = {2012},
  school    = {Universidade Federal Fluminense (UFF)},
  address   = {Niterói, RJ, Brasil}
}
```

---

## 📄 Licença

Este projeto é distribuído sob a licença [MIT](LICENSE).
