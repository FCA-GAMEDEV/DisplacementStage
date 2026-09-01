# DisplacementStage: Dynamic Displacement Mapping on GPU

> **Arquitetura extensível e de alto desempenho para geração e renderização de mapas dinâmicos de deslocamento na GPU aplicada a Jogos Digitais.**

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.2_Core-green.svg)](https://www.opengl.org/)
[![OpenCL](https://img.shields.io/badge/OpenCL-2.0%2B-orange.svg)](https://www.khronos.org/opencl/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-red.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows_x64-lightgrey.svg)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Release](https://img.shields.io/github/v/release/FCA-GAMEDEV/DisplacementStage?color=brightgreen&label=Release)](https://github.com/FCA-GAMEDEV/DisplacementStage/releases/latest)

---

## ⬇️ Executável Pronto para Uso (Windows x64)

> Baixe e execute diretamente — não é necessário compilar nem instalar Visual Studio, CMake ou vcpkg.

[![Baixar DisplacementStage v1.0.0](https://img.shields.io/badge/Download-DisplacementStage_v1.0.0_(.zip)-2ea44f?style=for-the-badge&logo=windows&logoColor=white)](https://github.com/FCA-GAMEDEV/DisplacementStage/releases/download/v1.0.0/DisplacementStage-v1.0.0-x64.zip)

### Passo a passo
1. Baixe e extraia o `.zip` mantendo a estrutura de pastas intacta.
2. *(Se necessário)* Instale o **[Visual C++ Redistributable 2022 x64](https://aka.ms/vs/17/release/vc_redist.x64.exe)** — já presente na maioria dos PCs com jogos modernos.
3. Execute `DisplacementStage.exe`.

> 💡 **Nota sobre o Windows SmartScreen:** Por ser um binário compilado de forma independente (sem certificado comercial pago), caso o Windows exiba o alerta azul ao abrir, clique em **"Mais informações"** e depois em **"Executar assim mesmo"**.

### Requisitos de Hardware
| Componente | Requisito Mínimo |
| :--- | :--- |
| **GPU** | Suporte a **OpenGL 4.2 Core** + **OpenCL 2.0+** *(Em notebooks com gráficos híbridos, certifique-se de executar com a GPU dedicada NVIDIA/AMD)* |
| **OS** | Windows 10 / 11 (64-bit) |

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

A arquitetura foi projetada para demonstrar a separação física/visual através de **4 cenas base** e **1 cena de prova de conceito autoral**, validando a facilidade de extensão do sistema:

### 📦 Cenas Base (Física Pré-definida)

#### 1. Contact Scene (Cena 1 - Contato)
Rastreamento e gravação contínua de marcas e trilhas deixadas por entidades móveis (esferas físicas) sobre o terreno.
![Contact Scene](docs/figures/scene1.png)

#### 2. Force Scene (Cena 2 - Forças e Impactos)
Simulação paramétrica de deformações causadas por vetores de força e impulsos pontuais tridimensionais (crateras e explosões).
![Force Scene](docs/figures/scene2.png)

#### 3. Morphing Scene (Cena 3 - Transição Topológica)
Interpolação temporal contínua e fluida entre duas matrizes de relevo distintas.
![Morphing Scene](docs/figures/scene3.png)

---

### 🧩 Extensibilidade e Criação Autoral

#### 4. Custom Scene (Cena 4 - Esqueleto Didático de Customização)
Um modelo de partida limpo e desacoplado (princípio *Open-Closed*). Ele serve como esqueleto didático para que novos desenvolvedores implementem suas próprias lógicas físicas no OpenCL (`displacementStage.cl`) sem precisar alterar o motor gráfico principal em C++/OpenGL.
![Custom Scene](docs/figures/scene4.png)

#### 5. Test Scene (Cena 5 - Prova Prática de Extensibilidade)
A prova de conceito e validação de que o esqueleto da **Cena 4** realmente funciona. Trata-se de uma ferramenta autoral completa de edição e escultura em tempo real orientada por um pincel circular interativo (deformações positivas e negativas nos canais RGB), demonstrando a implementação de uma lógica física customizada sobre a estrutura base.

**Nova Interface (2026):** Todos os controles de visualização 3D estão agora acessíveis via checkboxes persistentes no canto superior esquerdo, visíveis em todas as cenas.

![Test Scene — Nova UI 2026](docs/figures/newUIscene5.png)

> **💡 Iluminação Direcional:** A malha 3D é iluminada por uma luz direcional calculada analiticamente no *Geometry Shader*. As normais de cada triângulo são computadas por produto vetorial das arestas deformadas, revelando com nitidez a profundidade e as nuances do relevo do mapa de deslocamento — algo impossível de perceber com cor plana. A intensidade combina componente ambiente (`0.35`) e difusa de Lambert (`0.65`), com vetor de luz inclinado da frente para trás e da esquerda para a direita.

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
| **Shaders** | Strings inline no código C++ | Ficheiros externos em `bin/data/shaders/` com hot-reload (`F7`) |
| **Câmera 3D** | Acoplada à classe `Terrain` | Classe `Camera` dedicada e desacoplada (Arcball + Pan) |
| **Iluminação** | Sem iluminação (cor plana) | Luz direcional difusa (Lambert) calculada via Geometry Shader |
| **Esquema de Tessellation** | Fixo (`equal_spacing`) | Seleccionável em runtime: Even / Odd Spacing |
| **Interface de Controlo** | Sem HUD | 5 checkboxes persistentes em todas as cenas |

---

## 🎮 Guia de Controles (Teclado e Mouse)

### Navegação Geral
* **`1` a `5`:** Alterna entre as cenas de validação dos kernels.
* **`Espaço`:** Retorna ao Menu Principal (Cena 0).
* **`ESC`:** Fecha a aplicação imediatamente.

### Câmera 3D
* **`Botão do Meio do Mouse + Arrastar`:** Orbita a câmera livremente ao redor do terreno (Arcball). Nas cenas 1–4, arrastar com o clique esquerdo ou direito também orbita.
* **`Scroll do Mouse`:** Dá zoom (aproxima/afasta). Na **Cena 5 (Test Scene)**, segure **`Ctrl` + Scroll** para dar zoom.
* **`W` / `S` / `A` / `D`:** Movem o foco/alvo da câmera no plano do solo (frente, trás, esquerda, direita), de forma **contínua e relativa à orientação atual** — sem atraso de tecla.
* **`E` / `Q`:** Elevam ou abaixam a altura do alvo da câmera (eixo Y).

### Visualização e Depuração
* **`F7`:** **Hot-reload** de todos os shaders GLSL a partir dos ficheiros externos, sem reiniciar a aplicação.
* **`F9`:** Ativa/Desativa o mini-mapa 2D da textura de deslocamento (padrão Decorator).
* **`F11`:** Alterna o modo *Wireframe* da malha 3D.
* **`R`:** Liga/Desliga a rotação automática da malha 3D.
* **`P`:** Pausa/Retoma o cálculo de deformação física no OpenCL.
* **`M`:** Guarda captura da textura de deformação (`.png`) em `bin/data/dmap/`.

### Checkboxes de Interface (visíveis em todas as cenas)

| Checkbox | Cor | Função |
| :--- | :--- | :--- |
| **WIREFRAME** | Laranja | Liga/Desliga o modo wireframe da malha |
| **ROTATION** | Verde | Liga/Desliga a rotação automática |
| **CULL FACE** | Azul/Cyan | Ativa/Desativa o culling de faces traseiras (`GL_CULL_FACE`) |
| **EVEN SPACING** | Amarelo | Tessellation com partição par fracionária suave — **padrão ativo** |
| **ODD SPACING** | Roxo | Tessellation com partição ímpar fracionária suave |

> **Nota:** EVEN e ODD funcionam como radio buttons — um dos dois estará sempre activo. Clicar no que já está activo não faz nada; para mudar, clique no que está inactivo.

### Escultura e Pincel Interativo (Cena 5 - Test Scene)
* **`Scroll do Mouse` ou `+ / -`:** Ajusta o raio de ação do pincel de deformação.
* **`Clique Esquerdo + Arrastar`:** Aplica deformação positiva (elevação/escultura do relevo).
* **`Clique Direito + Arrastar`:** Aplica deformação negativa (depressão/cratera).
* **`8`, `9` ou `0` ou `TAB`:** Seleciona o canal de cor activo (**R → G → B → R**).

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
├── bin/data/              # Recursos de runtime (fontes, texturas, kernels)
│   ├── fonts/             # Fontes (.ttf)
│   ├── kernels/           # Simulação física em OpenCL (.cl)
│   ├── textures/          # Texturas e mapas de altura (.png)
│   ├── dmap/              # Capturas de textura gravadas em runtime
│   └── shaders/           # Shaders GLSL externos (hot-reload via F7)
│       ├── tess/          # displacement.vert/tesc/tese/geom/frag
│       ├── simple/        # quad.vert/frag (mini-mapa)
│       └── color/         # flat.vert/frag (UI e cursor)
├── docs/                  # Dissertação de Mestrado e artigos científicos
│   ├── dissertacao_FCA.pdf
│   └── sbgames_FCA.pdf
├── src/                   # Código-fonte em C++
│   ├── Camera.h / Camera.cpp      # Câmera 3D desacoplada (Arcball + Pan)
│   ├── Scene.h / Scene.cpp        # Classe base com câmera, UI e loop de input
│   ├── Interface.h / Interface.cpp # HUD com checkboxes e mini-mapa (Decorator)
│   ├── ShaderManager.h / .cpp     # Compilação, linking e hot-reload de shaders
│   ├── Terrain.h / Terrain.cpp    # Malha base, tessellation, culling e iluminação
│   └── ...                        # Cenas: Contact, Force, Morphing, Custom, Test
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
