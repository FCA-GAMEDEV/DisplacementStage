# DisplacementStage v1.0.0 — Windows x64

> Arquitetura extensível e de alto desempenho para geração e renderização de mapas dinâmicos de deslocamento na GPU aplicada a Jogos Digitais.
>
> Dissertação de Mestrado — Fábio C. Andrade · Universidade Federal Fluminense (UFF) · 2012  
> Modernizado para C++17 / OpenGL 4.2 Core / OpenCL 2.0+ em 2026.

---

## ⬇️ Download

**[DisplacementStage-v1.0.0-x64.zip](https://github.com/FCA-GAMEDEV/DisplacementStage/releases/download/v1.0.0/DisplacementStage-v1.0.0-x64.zip)**

Extraia e execute `DisplacementStage.exe`. Não é necessário instalar Visual Studio, CMake ou vcpkg.

---

## ⚙️ Requisitos de Hardware

| Requisito | Mínimo |
|:--|:--|
| **GPU** | OpenGL 4.2 Core Profile + OpenCL (NVIDIA, AMD ou Intel — driver padrão já inclui) |
| **OS** | Windows 10 / 11 (64-bit) |
| **RAM** | 4 GB recomendado |

## 📦 Dependência de Software

O executável requer o **Visual C++ Redistributable 2022 x64** (já presente na maioria dos PCs com jogos modernos).  
Se ao rodar aparecer erro de DLL ausente, instale:

➡️ **[vc_redist.x64.exe](https://aka.ms/vs/17/release/vc_redist.x64.exe)** (~25 MB, gratuito, da Microsoft)

---

## 🎮 Controles Rápidos

| Tecla | Ação |
|:--|:--|
| `1` a `5` | Alterna entre as 5 cenas de validação |
| `Espaço` | Volta ao Menu Principal |
| `ESC` | Fecha a aplicação |
| Botão do Meio + Arrastar | Orbita a câmera (Arcball) |
| Scroll | Zoom |
| `W` / `S` / `A` / `D` | Move o foco da câmera |
| `F7` | Hot-reload dos shaders GLSL |
| `F9` | Liga/desliga mini-mapa 2D |
| `F11` | Alterna modo Wireframe |
| `R` | Liga/desliga rotação automática |
| `P` | Pausa/retoma física no OpenCL |
| `M` | Grava captura da textura em `data/dmap/` |

**Cena 5 — Pincel de Escultura:**  
Scroll ou `+`/`-` ajusta o raio · Clique Esq = elevação · Clique Dir = cratera · `8`/`9`/`0`/`TAB` = canal R/G/B

---

## 🏛️ Arquitetura

Pipeline em 3 camadas totalmente na GPU (VRAM), sem tráfego pelo barramento PCIe:

1. **Displacement Generator (OpenCL)** — física de deformação do relevo
2. **Interoperability Bridge (`cl::Image2DGL`)** — zero-copy entre OpenCL e OpenGL
3. **Displacement Manager (OpenGL 4.2 Core)** — tessellation, geometry shader e iluminação Lambert

---

## 📚 Documentação Científica

- 📄 [Dissertação de Mestrado (PDF)](https://github.com/FCA-GAMEDEV/DisplacementStage/blob/main/docs/dissertacao_FCA.pdf)
- 📄 [Artigo SBGames (PDF)](https://github.com/FCA-GAMEDEV/DisplacementStage/blob/main/docs/sbgames_FCA.pdf)

---

## 🗂️ Conteúdo do Pacote

```
DisplacementStage-v1.0.0-x64/
├── DisplacementStage.exe   # Executável Release x64
├── glfw3.dll               # GLFW — janela e input
├── glew32.dll              # GLEW — extensões OpenGL
├── LEIA-ME.txt             # Instruções de execução
└── data/
    ├── fonts/              # Batang.ttf
    ├── kernels/            # displacementStage.cl (OpenCL)
    ├── shaders/            # GLSL (tess, simple, color)
    ├── textures/           # Mapas de altura e interface
    └── dmap/               # Capturas de textura gravadas em runtime
```

---

## Dependências de Runtime (para referência)

| DLL | Origem |
|:--|:--|
| `glfw3.dll` | Incluída no pacote (vcpkg) |
| `glew32.dll` | Incluída no pacote (vcpkg) |
| `OpenCL.dll` | Fornecida pelo driver da GPU (System32) — não distribuída |
| `MSVCP140.dll` / `VCRUNTIME140.dll` | Visual C++ Redistributable 2022 x64 |
| `OPENGL32.dll` / `KERNEL32.dll` | Windows — já presentes em qualquer instalação |
