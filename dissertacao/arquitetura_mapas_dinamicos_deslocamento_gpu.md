# Uma Proposta de Arquitetura Extensível para Mapas Dinâmicos de Deslocamento na GPU para Jogos Digitais

**Autor:** Fábio Corato de Andrade  
**Orientadora:** Profa. Dra. Aura Conci  
**Coorientador:** Prof. Dr. Esteban Walter Gonzalez Clua  
**Instituição:** Universidade Federal Fluminense (UFF) — Instituto de Computação  
**Programa:** Pós-Graduação em Computação (Área: Computação Visual | Linha: Jogos e Entretenimento Digital)  
**Ano:** 2012  

---

## Resumo

*Tessellation* e *Displacement Mapping* são métodos consolidados em Computação Gráfica, porém historicamente restritos em aplicações de tempo real devido ao seu elevado custo de processamento geométrico e de memória. Com a introdução de novos estágios programáveis no pipeline gráfico das GPUs modernas, essas técnicas tornaram-se viáveis para a renderização interativa em jogos digitais. 

Este trabalho propõe uma **arquitetura extensível baseada em GPU** para gerenciar, atualizar e aplicar mapas dinâmicos de deslocamento em tempo real. A arquitetura desacopla a manipulação algorítmica do relevo dos *shaders* convencionais de renderização, transferindo essa responsabilidade para *kernels* paralelos em OpenCL integrados com OpenGL através de mecanismos de interoperabilidade de memória de textura (*Texture Buffer Objects* / *Image Objects*). A solução apresenta três *kernels* especializados nativos — deformação por contato, deformação por forças externas e deformação por transição topológica (*morphing*) —, além de um módulo customizável (*custom kernel*) que viabiliza a expansão modular por parte dos desenvolvedores. Os experimentos práticos validam a arquitetura alcançando taxas de renderização acima de 120 FPS mesmo sob fatores máximos de subdivisão (*tessellation factor* 64), comprovando a viabilidade, modularidade e independência de hardware da proposta.

**Palavras-chave:** Arquitetura de Software, Mapeamento de Deslocamento, Unidades de Processamento Gráfico (GPU), Jogos Digitais, Tessellation, Morphing, OpenCL, OpenGL, Padrões de Projeto.

---

## Abstract

*Tessellation* and *Displacement Mapping* are well-known techniques in Computer Graphics, historically bounded to offline rendering due to their prohibitive computational cost. With the advent of programmable hardware tessellation stages in modern Graphics Processing Units (GPUs), dynamic surface displacement has become feasible for real-time interactive applications such as digital games.

This work proposes an **extensible GPU-based software architecture** dedicated to generating, controlling, and applying dynamic displacement maps in real time. By decoupling surface deformation algorithms from conventional rendering shaders and delegating them to dedicated OpenCL kernels running concurrently on the GPU, the architecture maximizes code reusability, modularity, and maintainability. Zero-copy memory interoperability between OpenCL and OpenGL ensures high-bandwidth data flow without CPU bottlenecks. The framework provides three specialized built-in kernels — contact deformation, force-driven deformation, and linear mesh morphing — alongside an extensible custom kernel interface for arbitrary procedural deformations. Empirical evaluations demonstrate interactive rendering performance exceeding 120 FPS under maximum tessellation factors (up to 64) on entry-level hardware, validating the architecture's efficiency and extensibilidade.

**Keywords:** Software Architecture, Displacement Mapping, Graphics Processing Units (GPU), Digital Games, Tessellation, Morphing, OpenCL, OpenGL, Design Patterns.

---

## Lista de Siglas e Abreviações

| Sigla | Significado |
|---|---|
| **API** | *Application Programming Interface* (Interface de Programação de Aplicações) |
| **CPU** | *Central Processing Unit* (Unidade Central de Processamento) |
| **DDM** | *Deformation Displacement Maps* (Mapas de Deslocamento de Deformação) |
| **FPS** | *Frames Per Second* (Quadros por Segundo) |
| **GLSL** | *OpenGL Shading Language* |
| **GPGPU / GPU Computing** | *General-Purpose Computing on Graphics Processing Units* |
| **GPU** | *Graphics Processing Unit* (Unidade de Processamento Gráfico) |
| **IDE** | *Integrated Development Environment* (Ambiente de Desenvolvimento Integrado) |
| **LOD** | *Level of Detail* (Nível de Detalhe) |
| **MCP** | *Model Context Protocol* |
| **OpenCL** | *Open Computing Language* |
| **OpenGL** | *Open Graphics Library* |
| **PN-Triangles** | *Point-Normal Triangles* (N-Patches) |
| **POM** | *Parallax Occlusion Mapping* |
| **TexBO** | *Texture Buffer Object* |
| **UML** | *Unified Modeling Language* |

---

## Sumário

1. [Introdução](#1-introdução)
   - 1.1 Contextualização e Motivação
   - 1.2 Objetivos e Escopo
   - 1.3 Computação Paralela na GPU e Abstração Arquitetural
   - 1.4 Principais Contribuições
2. [Trabalhos Relacionados](#2-trabalhos-relacionados)
3. [Tessellation em Hardware](#3-tessellation-em-hardware)
   - 3.1 Fundamentos Teóricos e Pipeline Gráfico Moderno
   - 3.2 Suavização Geométrica e PN-Triangles
   - 3.3 Detalhamento Adaptativo e Nível de Detalhe Dinâmico (LOD)
   - 3.4 Escalabilidade e Eficiência de Produção
4. [Displacement Mapping](#4-displacement-mapping)
   - 4.1 Comparação entre Técnicas de Texturização, Bump, POM e Displacement
   - 4.2 Deslocamento de Vértices e Dependência da Tessellation
5. [A Arquitetura Extensível](#5-a-arquitetura-extensível)
   - 5.1 O Modelo em Três Camadas
   - 5.2 Camada Displacement Generator
   - 5.3 Camada Interoperability Manager
   - 5.4 Camada Displacement Manager
   - 5.5 Módulo de Contato (*Contact Kernel*)
   - 5.6 Módulo de Força (*Force Kernel*)
   - 5.7 Módulo de Morphing (*Morphing Kernel*)
   - 5.8 Módulo Personalizado (*Custom Kernel*)
6. [Implementação e Validação Experimental](#6-implementação-e-validação-experimental)
   - 6.1 Fluxo de Execução da Aplicação
   - 6.2 Estrutura Orientada a Objetos e Padrões de Projeto (GoF)
   - 6.3 Descrição Detalhada dos Componentes de Software
   - 6.4 Ambiente de Testes e Especificações de Hardware
   - 6.5 Resultados de Desempenho e Benchmarks Comparativos
7. [Conclusão e Trabalhos Futuros](#7-conclusão-e-trabalhos-futuros)
8. [Referências Bibliográficas](#8-referências-bibliográficas)

---

# 1. Introdução

## 1.1 Contextualização e Motivação

Na indústria de software contemporânea, a aplicação rigorosa de padrões de projeto (*Design Patterns*) e modelos arquiteturais consolidados é uma prática essencial para garantir a manutenibilidade, extensibilidade e modularidade de sistemas complexos sob cronogramas rigorosos. No desenvolvimento de jogos digitais, essa necessidade torna-se ainda mais evidente devido à heterogeneidade dos subsistemas envolvidos — como renderização gráfica, física, inteligência artificial, áudio e rede.

Historicamente, a representação de detalhes ricos em superfícies tridimensionais (como terrenos acidentados, pegadas na lama, crateras de explosões ou deformações estruturais) dependia de abordagens baseadas em CPU ou de técnicas puramente visuais no nível do fragmento (*pixel/fragment shader*), como *Bump Mapping* e *Normal Mapping*. Embora essas técnicas simulem irregularidades por meio do cálculo de iluminação por pixel, elas falham ao não alterar a silhueta real dos objetos, gerando artefatos óbvios em ângulos rasantes e impedindo a oclusão geométrica e o lançamento de sombras precisas.

O advento da *Tessellation* acelerada por hardware nas APIs gráficas modernas (OpenGL 4.x e DirectX 11) inaugurou a possibilidade de gerar milhões de novos polígonos dinamicamente na GPU. Contudo, a simples subdivisão poligonal não acrescenta detalhes topológicos se os novos vértices não forem deslocados espacialmente. É a combinação de *Tessellation* com *Displacement Mapping* que transforma primitivas simples em geometrias complexas e hiper-realistas.

![Figura 1.1: Utilização de tessellation e displacement mapping em conjunto](./figures/fig_1_1_tessellation_displacement.png)  
*Figura 1.1: Evolução geométrica: modelo base simplificado (esquerda), malha suavizada via tessellation (centro) e geometria final esculpida por displacement mapping (direita).*

No entanto, a abordagem tradicional de implementar a lógica de deformação diretamente dentro dos *shaders* de renderização (GLSL/HLSL) impõe sérias restrições arquiteturais:
1. **Acoplamento excessivo:** A lógica matemática que calcula a deformação (ex.: raio de impacto, atenuação de força, transição de estado) fica intrinsecamente amarrada ao código de iluminação e desenho.
2. **Dificuldade de manutenção:** Qualquer alteração na física da deformação exige reescrever ou duplicar programas de *shader*.
3. **Falta de reutilização:** Os dados do relevo deformado não ficam facilmente acessíveis para outros subsistemas (ex.: IA de agentes para *pathfinding* ou detecção de colisão física).

## 1.2 Objetivos e Escopo

O objetivo central deste trabalho é **projetar, implementar e validar uma arquitetura de software extensível** que retire dos *shaders* convencionais de renderização a responsabilidade de calcular e gerenciar as alterações nos mapas de deslocamento, delegando essa tarefa a *kernels* modulares de processamento paralelo em GPU (OpenCL) sincronizados com o pipeline gráfico (OpenGL).

Os objetivos específicos compreendem:
* Estruturar um modelo arquitetural em **três camadas funcionais desacopladas**: *Displacement Generator*, *Interoperability Manager* e *Displacement Manager*.
* Desenvolver *kernels* nativos especializados para três tipos fundamentais de deformação:
  1. **Deformação por Contato:** Rastreamento e impressão contínua de trilhas/marcas geradas por entidades dinâmicas.
  2. **Deformação por Força:** Geração paramétrica de impactos localizados, crateras e fissuras causadas por impulsos vetoriais e explosões.
  3. **Deformação por Transição (Morphing):** Interpolação temporal entre topologias distintas para desmoronamentos, erosão e abertura de passagens.
* Fornecer um **Módulo Personalizável (*Custom Kernel*)** com protocolo aberto de passagem de parâmetros para suportar extensões arbitrárias de desenvolvedores.
* Eliminar o tráfego de memória redundante entre CPU e GPU utilizando interoperabilidade de texturas compartilhadas diretamente em VRAM.
* Mensurar o desempenho computacional e a escalabilidade da arquitetura em cenários de alta subdivisão poligonal (*tessellation factors* de 1 a 64).

## 1.3 Computação Paralela na GPU e Abstração Arquitetural

As GPUs modernas são processadores massivamente paralelos compostos por centenas a milhares de núcleos otimizados para operações SIMD/SIMT (*Single Instruction, Multiple Data / Threads*). No contexto de mapas de deslocamento bidimensionais, cada texel da textura pode ser processado de forma independente e simultânea.

Ao transferir o algoritmo de deformação para *kernels* OpenCL:
* A complexidade computacional da CPU torna-se independente do raio de ação da deformação e do tamanho da malha.
* Os dados calculados permanecem na memória global da GPU, sendo imediatamente consumidos pelo estágio de *Tessellation* e *Geometry Shader* do OpenGL sem cópias via barramento PCIe (*Zero-Copy Interoperability*).

## 1.4 Principais Contribuições

1. **Separação em Camadas Limpas:** Isolamento total entre geração de dados topológicos (OpenCL), transporte em VRAM (Interoperabilidade) e renderização gráfica (OpenGL/GLSL).
2. **Modularização dos Tipos de Deformação:** Unificação conceitual de três padrões de deformação física e estética em *kernels* reutilizáveis.
3. **Extensibilidade Nativa:** Facilidade de introduzir novos comportamentos proceduralmente via *Custom Kernel* sem necessidade de recompilar ou refatorar o motor gráfico.
4. **Armazenamento de Histórico e Uso Híbrido:** Possibilidade de persistir mapas deformados em disco para restaurar o estado de mundos virtuais e utilizar texturas de deslocamento como mapas espaciais para navegação de IA.

---

# 2. Trabalhos Relacionados

A evolução das técnicas de modificação de superfície e aceleração por hardware constitui a base teórica sobre a qual esta arquitetura foi construída:

* **Cook (1984):** Introduziu o conceito seminal de *Displacement Mapping* no contexto de *Shade Trees*, definindo-o como uma técnica de modelagem geométrica real em oposição ao mero mapeamento de texturas ou sombreamento de superfícies.
* **Pharr e Hanrahan (1996):** Apresentaram esquemas de *caching* geométrico para acelerar o traçado de raios (*ray tracing*) sobre superfícies com mapas de deslocamento.
* **Wang et al. (2003):** Desenvolveram o *View-Dependent Displacement Mapping*, viabilizando a renderização de silhuetas e sombras dependentes do ponto de vista do observador sem reconstruir a geometria base em CPU.
* **Schein et al. (2005):** Propuseram os *Deformation Displacement Maps* (DDM), utilizando hardware programável para deformar em tempo real superfícies poligonais e paramétricas.
* **Takahashi e Miyata (2005):** Descreveram abordagens de deformação interativa na GPU utilizando *Vertex Textures*, acompanhadas de detecção de colisão calculada diretamente no hardware gráfico.
* **Tortelli e Walter (2007):** Investigaram a viabilidade de transpor o *Displacement Mapping* de ambientes offline para jogos digitais interativos aproveitando os estágios programáveis de vértices.
* **Tatarchuk et al. (2009):** Mapearam os benefícios da tessellation em hardware para sistemas interativos, destacando compressão de malha, redução drástica de largura de banda no barramento e escalabilidade geométrica.
* **Loop e Schaefer (2008):** Introduziram esquemas de aproximação de superfícies de subdivisão de Catmull-Clark utilizando *patches* bicúbicos, padrão amplamente adotado nos estágios de *Hull/Domain Shaders*.
* **Nunes (2011):** Analisou o pipeline de *tessellation* do OpenGL 4 e DirectX 11, comparando implementações clássicas (PN-Triangles, Phong Tessellation) com novos algoritmos para terrenos e cilindros em GPU, comprovando o ganho expressivo de FPS frente a abordagens em CPU.
* **Batista (2011):** Desenvolveu sistemas de simulação facial na GPU combinando *Bump Mapping* e *Morphing*. Este trabalho estende as limitações identificadas por Batista, substituindo o *Bump Mapping* pelo deslocamento geométrico real via *Tessellation* para evitar quebras visuais em auto-oclusões e silhuetas extremas.

---

# 3. Tessellation em Hardware

## 3.1 Fundamentos Teóricos e Pipeline Gráfico Moderno

A técnica de *Tessellation* consiste na partição contínua de um espaço bidimensional ou tridimensional em regiões poligonais convexas (geralmente triângulos), sem lacunas nem sobreposições. No contexto de rendering em tempo real, ela representa a capacidade da GPU de receber uma primitiva grosseira (*patch* com poucos pontos de controle) e subdividi-la proceduralmente em centenas de novos triângulos diretamente no pipeline gráfico.

Com as APIs DirectX 11 e OpenGL 4.x, foram introduzidos três novos estágios dedicados exclusivamente à tessellation, posicionados entre o *Vertex Shader* e o *Geometry Shader*:

![Figura 3.2 e 3.3: Pipelines Gráficos do DirectX 11 e OpenGL 4](./figures/fig_3_2_pipeline_direct3d11.png)  
![Figura 3.3: Pipeline gráfico do OpenGL 4](./figures/fig_3_3_pipeline_opengl4.png)  
*Figuras 3.2 e 3.3: Comparativo estrutural dos pipelines gráficos modernos do DirectX 11 (esquerda) e OpenGL 4 (direita), destacando os três estágios de tessellation em verde.*

| Função no Pipeline | Estágio DirectX 11 | Estágio OpenGL 4 | Natureza |
|---|---|---|---|
| **Cálculo de Fatores e Pontos de Controle** | *Hull Shader* | *Tessellation Control Shader* (TCS) | Totalmente Programável |
| **Geração de Topologia / Subdivisão** | *Tessellator* | *Tessellation Primitive Generator* (TPG) | Função Fixa Configurável |
| **Avaliação e Posicionamento de Vértices** | *Domain Shader* | *Tessellation Evaluation Shader* (TES) | Totalmente Programável |

1. **Tessellation Control Shader (Hull Shader):** Recebe os pontos de controle da primitiva de entrada. Calcula os fatores de tessellation internos e de borda (*Tessellation Factors*, variando tipicamente de 1 a 64), determinando a densidade da subdivisão. Também pode transformar as bases dos pontos de controle.
2. **Tessellation Primitive Generator (Tessellator):** Executa em silício a divisão puramente matemática do domínio paramétrico (triangular ou quadrilátero) com base nos fatores recebidos, gerando coordenadas baricêntricas $(u, v, w)$ para cada novo vértice.
3. **Tessellation Evaluation Shader (Domain Shader):** Recebe as coordenadas paramétricas geradas pelo tessellator e interpola as propriedades dos vértices (posição tridimensional, normais, coordenadas de textura).

![Figura 3.1: Exemplo de tessellation aplicado](./figures/fig_3_1_tessellation_head.png)  
*Figura 3.1: Subdivisão progressiva de uma malha base facial em múltiplos níveis de resolução geométrica.*

![Figuras 3.4 e 3.5: Tessellation em jogos comerciais](./figures/fig_3_4_alien_vs_predator.png)  
![Figura 3.5: Tessellation em Metro 2033](./figures/fig_3_5_metro_2033.png)  
*Figuras 3.4 e 3.5: Aplicações de tessellation em produções comerciais: Aliens vs Predator (DX11 ativado com silhuetas curvas) e Metro 2033 (objetos arredondados e sem arestas facetadas).*

## 3.2 Suavização Geométrica e PN-Triangles

A combinação de tessellation com algoritmos como **PN-Triangles (Point-Normal Triangles / N-Patches)** permite converter malhas poliédricas de baixa resolução em superfícies curvas contínuas ($C^0$ ou $C^1$) sem exigir trabalho manual adicional de modelagem por parte dos artistas.

![Figura 3.6: Suavização usando PN-Triangles](./figures/fig_3_6_suavizacao_pn_triangles.png)  
*Figura 3.6: Modelo poligonal grosseiro (esquerda) transformado em malha suave e arredondada (direita) através do algoritmo PN-Triangles.*

## 3.3 Detalhamento Adaptativo e Nível de Detalhe Dinâmico (LOD)

A tessellation dinâmica em hardware soluciona o clássico problema de *popping* visual na transição de LOD (*Level of Detail*). Em vez de armazenar múltiplas versões discretas de uma malha em memória RAM/VRAM, o *Tessellation Control Shader* ajusta dinamicamente os fatores de subdivisão de cada polígono como uma função contínua da distância da câmera, orientação e área projetada na tela:

$$	ext{TessFactor} = 	ext{clamp}\left( rac{d_{	ext{max}} - d_{	ext{cam}}}{d_{	ext{max}} - d_{	ext{min}}} 	imes F_{	ext{max}}, 1, 64 ight)$$

![Figura 3.7: Level of details dinâmico](./figures/fig_3_7_level_of_details.png)  
*Figura 3.7: Malha de terreno com subdivisão adaptativa: alta densidade de triângulos nas proximidades da câmera e densidade reduzida nas regiões distantes.*

## 3.4 Escalabilidade e Eficiência de Produção

A capacidade de manter um único ativo tridimensional base leve e escalá-lo proceduralmente de acordo com a capacidade do hardware alvo simplifica pipelines de produção, reduz tempos de carregamento e minimiza o consumo de largura de banda de memória.

![Figura 3.8: Escalabilidade geométrica](./figures/fig_3_8_escalabilidade.png)  
*Figura 3.8: Modelo de personagem escalado de baixa resolução até qualidade cinematográfica.*

---

# 4. Displacement Mapping

## 4.1 Comparação entre Técnicas de Texturização, Bump, POM e Displacement

| Característica | Texture Mapping | Bump / Normal Mapping | Parallax Occlusion Mapping (POM) | Displacement Mapping |
|---|---|---|---|---|
| **Nível de Atuação** | Fragmento (Cor) | Fragmento (Normais) | Fragmento (Offset de UV por Raymarching) | **Vértice Real (Geometria 3D)** |
| **Altera Silhueta Externa?** | Não | Não | Não | **Sim** |
| **Auto-Oclusão Real?** | Não | Não | Parcial (interna ao polígono) | **Sim (Geométrica)** |
| **Sombras Próprias Precisas?** | Não | Apenas Iluminação Local | Parcial | **Sim (Compatível com Shadow Maps)** |
| **Custo Computacional** | Muito Baixo | Baixo | Médio/Alto na GPU (Loops de Pixel) | **Equilibrado via Tessellation** |

![Figura 4.1: Comparação entre Bump, POM e Displacement Mapping](./figures/fig_4_1_bump_pom_displacement.png)  
*Figura 4.1: Comparativo visual direto: Bump Mapping (topo - plano sem relevo de borda), Parallax Occlusion Mapping (meio - ilusão de profundidade interna mas bordas retas) e Displacement Mapping (base - silhueta real esculpida com contornos tridimensionais).*

## 4.2 Deslocamento de Vértices e Dependência da Tessellation

No *Displacement Mapping*, a posição espacial do vértice $\mathbf{P}$ é transladada ao longo do vetor normal unitário da superfície $\mathbf{N}$, ponderada pelo valor escalar de intensidade $h(u, v)$ amostrado do mapa de deslocamento:

$$\mathbf{P}_{	ext{deslocado}} = \mathbf{P}_{	ext{base}} + \mathbf{N} \cdot \left( h(u, v) 	imes 	ext{Escala} ight)$$

Para que esse deslocamento produza curvas suaves e relevos bem definidos, a densidade da malha precisa ser proporcional à frequência espacial do mapa. A *Tessellation* fornece exatamente a infraestrutura de vértices sob demanda necessária para que o *Displacement Mapping* expresse todo o seu potencial geométrico em tempo real.

---

# 5. A Arquitetura Extensível

## 5.1 O Modelo em Três Camadas

A arquitetura proposta foi concebida sob um modelo em camadas desacopladas, garantindo que responsabilidades distintas (cálculo numérico, tráfego de dados e renderização) operem de maneira independente:

![Figura 5.1: Arquitetura Extensível: Modelo em Camadas](./figures/fig_5_1_arquitetura_modelo_camadas.png)  
*Figura 5.1: Diagrama conceitual das camadas da arquitetura: Displacement Generator (OpenCL Kernels), Interoperability Manager (Memória de Textura / TexBO) e Displacement Manager (Pipeline Gráfico OpenGL/GLSL).*

```
+-----------------------------------------------------------------------------+
|                        CAMADA 1: DISPLACEMENT GENERATOR                     |
|                                                                             |
|  +-----------------------------------------------------------------------+  |
|  |                          Displacement Module                          |  |
|  |  +----------------+  +--------------+  +----------------+  +-------+  |  |
|  |  | Contact Kernel |  | Force Kernel |  | Morphing Kernel|  |Custom |  |  |
|  |  |    (OpenCL)    |  |   (OpenCL)   |  |    (OpenCL)    |  |Kernel |  |  |
|  |  +----------------+  +--------------+  +----------------+  +-------+  |  |
|  +-----------------------------------------------------------------------+  |
+--------------------------------------|--------------------------------------+
                                       | (Zero-Copy OpenCL/GL Interop)
+--------------------------------------v--------------------------------------+
|                       CAMADA 2: INTEROPERABILITY MANAGER                     |
|                                                                             |
|  +-----------------------------------------------------------------------+  |
|  |                              Texel Data                               |  |
|  |  +---------------------------------+  +----------------------------+  |  |
|  |  |  Texture Buffer Object (TexBO)  |  |       Texture Image        |  |  |
|  |  +---------------------------------+  +----------------------------+  |  |
|  +-----------------------------------------------------------------------+  |
+--------------------------------------|--------------------------------------+
                                       | (Amostragem Direta em Shader)
+--------------------------------------v--------------------------------------+
|                         CAMADA 3: DISPLACEMENT MANAGER                      |
|                                                                             |
|  +-----------------------------------------------------------------------+  |
|  |                        Pipeline Gráfico (GLSL)                        |  |
|  |  [Vertex Shader] -> [Tessellation Control] -> [Tessellator] ->        |  |
|  |  [Tessellation Evaluation] -> [Geometry Shader (Displacement)] ->     |  |
|  |  [Rasterization] -> [Fragment Shader] -> [Frame Buffer]                |  |
|  +-----------------------------------------------------------------------+  |
+-----------------------------------------------------------------------------+
```

## 5.2 Camada Displacement Generator
Contém o módulo `Displacement Module`, implementado em OpenCL. Executa o processamento paralelo em GPU que altera dinamicamente os valores de cada texel dos mapas de deslocamento com base em eventos da aplicação.

## 5.3 Camada Interoperability Manager
Responsável por gerenciar os recursos de textura alocados na GPU. Utilizando as extensões `cl_khr_gl_sharing`, o OpenCL adquire os identificadores de textura do OpenGL (`clCreateFromGLTexture2D`) e executa escritas diretas sem necessidade de transferir dados para a memória RAM do sistema operacional através do barramento PCIe.

## 5.4 Camada Displacement Manager
Representa o pipeline gráfico tradicional estendido. Recebe a malha base do terreno, executa a subdivisão adaptativa no *Tessellator* e, no *Geometry Shader* (ou *Evaluation Shader*), lê a textura atualizada para deslocar os vértices resultantes antes da rasterização e sombreamento final.

---

## 5.5 Módulo de Contato (*Contact Kernel*)

O módulo de contato modela a interação contínua entre entidades móveis (jogadores, veículos, tropas, animais) e o solo. À medida que a entidade se desloca, ela imprime sua trajetória no mapa de deslocamento.

![Figura 5.2: Módulo de Contato](./figures/fig_5_2_modulo_contato.png)  
*Figura 5.2: Resultado visual do Contact Kernel: geometria 3D deformada com trilha de passagem (esquerda) e mapa de deslocamento correspondente em vista superior (direita).*

### Algoritmo do Contact Kernel (OpenCL C):
```c
__kernel void contact(
    __read_only image2d_t in,
    __write_only image2d_t out,
    float2 pos,
    float depth,
    float radius)
{
    const sampler_t s = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
    int y = get_global_id(1);
    int2 coords = (int2)(x, y);

    float4 image = read_imagef(in, s, coords);
    float dist = distance(pos, (float2)((float)x, (float)y));

    if (dist < radius) {
        image.z = depth;
    }

    write_imagef(out, coords, image);
}
```

### Funcionalidade Alternativa: Mapas de Localização Espacial e Uso Híbrido
Além de deformar a topologia física, o mapa resultante pode operar como um **mapa de rastreamento e navegação para Inteligência Artificial**. Agentes perseguidores podem amostrar a textura em GPU para seguir o rastro deixado por outras entidades sem a necessidade de executar algoritmos caros de *pathfinding* ($A^*$) na CPU a cada quadro.

![Figura 5.3: Mapa de localização espacial](./figures/fig_5_3_mapa_localizacao_espacial.png)  
*Figura 5.3: Mapa de deslocamento armazenando históricos de trilhas de navegação para consulta de agentes de IA.*

---

## 5.6 Módulo de Força (*Force Kernel*)

O módulo de força representa deformações plásticas causadas por impactos externos pontuais ou distribuídos, como explosões de granadas, colisões de projéteis ou forças tectônicas subterrâneas.

A força é modelada como um vetor de 4 componentes: $\mathbf{F} = (F_x, F_y, F_z, K_{	ext{escala}})$, onde as três primeiras componentes definem a orientação e intensidade vetorial, e a quarta atua como multiplicador de escala da área afetada.

![Figura 5.4: Módulo de Força](./figures/fig_5_4_modulo_forca.png)  
*Figura 5.4: Deformações geradas pelo Force Kernel: relevo 3D apresentando crateras e elevações (esquerda) e mapa com as nove aplicações de força registradas (direita).*

### Algoritmo do Force Kernel (OpenCL C):
```c
__kernel void force(
    __read_only image2d_t in,
    __write_only image2d_t out,
    float2 pos,
    float4 force_vec)
{
    const sampler_t s = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
    int y = get_global_id(1);
    int2 coords = (int2)(x, y);

    float4 image = read_imagef(in, s, coords);
    float area = length(force_vec.xyz) * force_vec.w; // force_vec.w contém a constante de escala
    float dist = distance(pos, (float2)((float)x, (float)y));
    float dissipation = 1.0f - (dist / area);

    if (dissipation > 0.0f) {
        image += force_vec * dissipation;
    }

    write_imagef(out, coords, image);
}
```

---

## 5.7 Módulo de Morphing (*Morphing Kernel*)

O módulo de morphing executa a transição temporal e contínua entre dois estados topográficos distintos ($I_1$ e $I_2$) através de interpolação linear parametrizada no tempo $t \in [0, 1]$:

$$I_{	ext{out}}(x, y) = I_1(x, y) \cdot (1 - t) + I_2(x, y) \cdot t$$

Essa técnica viabiliza eventos narrativos dinâmicos em jogos, tais como abertura de crateras, surgimento de desfiladeiros, erosão de montanhas ou simulação de desmoronamentos de terreno.

![Figura 5.5 e 5.6: Módulo de Morphing](./figures/fig_5_5_modulo_morphing_inicial.png)  
![Figura 5.6: Módulo de Morphing Estado Final](./figures/fig_5_6_modulo_morphing_final.png)  
*Figuras 5.5 e 5.6: Transição por morphing: estado inicial da cratera fechada (Figura 5.5) evoluindo continuamente até o estado final com passagem aberta (Figura 5.6).*

### Algoritmo do Morphing Kernel (OpenCL C):
```c
__kernel void morphing(
    __read_only image2d_t in1,
    __read_only image2d_t in2,
    __write_only image2d_t out,
    float time_factor)
{
    const sampler_t s = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
    int y = get_global_id(1);
    int2 coords = (int2)(x, y);

    float4 image1 = read_imagef(in1, s, coords);
    float4 image2 = read_imagef(in2, s, coords);
    float4 image_out = image1 * (1.0f - time_factor) + image2 * time_factor;

    write_imagef(out, coords, image_out);
}
```

---

## 5.8 Módulo Personalizado (*Custom Kernel*)

O módulo customizável garante a extensibilidade da arquitetura. Ele fornece um *template* pré-configurado capaz de receber até 16 parâmetros genéricos empacotados em quatro vetores `float4` (`arg1`, `arg2`, `arg3`, `arg4`), permitindo aos desenvolvedores criar novos tipos de deformação procedural sem alterar a infraestrutura da engine.

```c
__kernel void custom(
    __read_only image2d_t in,
    __write_only image2d_t out,
    float4 arg1,
    float4 arg2,
    float4 arg3,
    float4 arg4)
{
    const sampler_t s = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
    int y = get_global_id(1);
    int2 coords = (int2)(x, y);

    float4 image = read_imagef(in, s, coords);

    // =========================================================
    // INSERIR LÓGICA DE DEFORMAÇÃO PERSONALIZADA AQUI
    // =========================================================

    write_imagef(out, coords, image);
}
```

![Figura 5.7: Interface da classe DisplacementStage](./figures/fig_5_7_interface_displacement_stage.png)  
*Figura 5.7: Definição da interface C++ da classe DisplacementStage com métodos dedicados para alocação de buffers e passagem de argumentos para o Custom Kernel.*

---

# 6. Implementação e Validação Experimental

## 6.1 Fluxo de Execução da Aplicação

O ciclo de vida da aplicação divide-se em três etapas bem delimitadas: **Configuração**, **Atualização** e **Desenho**:

![Figura 6.1: Fluxograma das principais atividades da arquitetura](./figures/fig_6_1_fluxograma_atividades.png)  
*Figura 6.1: Fluxograma estrutural das etapas de Configuração (azul), Atualização em GPU (amarelo) e Desenho no pipeline gráfico (verde).*

1. **Etapa de Configuração (Executada uma única vez):**
   * Inicialização do contexto OpenCL e detecção do dispositivo GPU.
   * Criação e envio da malha geométrica base para a GPU.
   * Carregamento da textura inicial e registro de identificador no OpenGL.
   * Registro do buffer de textura no OpenCL via extensão de interoperabilidade.
2. **Etapa de Atualização (Condicional / Por Frame):**
   * Caso haja eventos de interação, disparo do *kernel* OpenCL correspondente para modificar os dados de texels diretamente na VRAM.
3. **Etapa de Desenho (Executada a cada quadro de renderização):**
   * Envio da malha base para os shaders GLSL.
   * Subdivisão poligonal adaptativa no estágio do *Tessellator*.
   * Aplicação do *Displacement Mapping* no *Geometry Shader* através da amostragem do mapa de deslocamento atualizado.
   * Rasterização, sombreamento de fragmentos e exibição no *Frame Buffer*.

## 6.2 Estrutura Orientada a Objetos e Padrões de Projeto (GoF)

A implementação adotou princípios de orientação a objetos e padrões de projeto consolidados (GoF):

* **Mediator (`SceneManager`):** Centraliza a coordenação e alternância entre as diferentes cenas de teste sem gerar dependências cíclicas entre elas.
* **Singleton (`SceneManager`, `TimeManager`):** Garante instância única global para controle de ciclo de vida e cálculo do $\Delta t$ (*deltaTime*) desacoplado da taxa de quadros.
* **Decorator (`Interface`):** Permite acoplar dinamicamente componentes de depuração visual e miniaturas do mapa de deslocamento sobre qualquer cena sem alterar sua hierarquia de herança.

![Figura 6.2: Diagrama de Classes da Implementação](./figures/fig_6_2_diagrama_classes_uml.png)  
*Figura 6.2: Diagrama de classes UML ilustrando o relacionamento entre os módulos da arquitetura e as cenas de teste.*

![Figura 6.3: Padrão Decorator aplicado a ForceScene](./figures/fig_6_3_padrao_decorator_forcescene.png)  
*Figura 6.3: Janela de execução da ForceScene utilizando o padrão Decorator para renderizar o mini-painel de inspeção do mapa de deslocamento no canto superior esquerdo.*

## 6.3 Descrição Detalhada dos Componentes de Software

* `Game`: Classe principal herdada do framework Openframeworks, responsável pelo laço principal (*game loop*), eventos de teclado/mouse e janelas.
* `DisplacementStage`: Componente central da camada *Displacement Generator*. Compila os códigos-fonte OpenCL em tempo de execução, aloca os buffers de memória e despacha a execução dos *kernels*.
* `Interoperability`: Encapsula a sincronização entre buffers do OpenGL e referências do OpenCL (`cl_mem`), garantindo exclusão mútua e transferências *zero-copy*.
* `ShaderManager`: Carrega, compila e gerencia os programas GLSL (*Vertex*, *Tessellation Control*, *Tessellation Evaluation*, *Geometry* e *Fragment Shaders*).
* `Terrain`: Representa a malha 3D base (inicialmente um plano de 2 triângulos) submetida ao processo de subdivisão e deformação.
* `ContactScene`, `ForceScene`, `MorphingScene`, `CustomScene`, `TestScene`: Classes especializadas que herdam da classe abstrata `Scene` para demonstrar e testar cada caso de uso da arquitetura.

### Caso de Teste do Custom Kernel (`TestScene` - Ferramenta de Escultura em Tempo Real)
Na cena `TestScene`, o *Custom Kernel* foi adaptado como uma ferramenta interativa de escultura 3D em tempo real (*test kernel*), onde o usuário interage via mouse e teclado alterando o raio, intensidade e posição do relevo:

![Figura 6.4: Malha base e após Tessellation](./figures/fig_6_4_resultado_malha_tessellation.png)  
![Figura 6.5: Malha após Displacement Mapping](./figures/fig_6_5_resultado_displacement_mapping.png)  
![Figura 6.6: Mapa de deslocamento gerado pelo test kernel](./figures/fig_6_6_resultado_mapa_deslocamento.png)  
*Figuras 6.4, 6.5 e 6.6: Pipeline de escultura interativa: plano original de 2 triângulos transformado em malha densa tesselada (Figura 6.4), geometria final deformada em perspectiva distante e aproximada (Figura 6.5) e textura resultante gerada pelo test kernel (Figura 6.6).*

---

## 6.4 Ambiente de Testes e Especificações de Hardware

Todos os testes de desempenho foram executados em um ambiente de hardware modesto de entrada, intencionalmente configurado para avaliar a eficiência da arquitetura sob restrições computacionais severas:

* **Processador (CPU):** Intel® Core™ 2 Duo E6300 @ 1.86 GHz
* **Memória RAM:** 4 GB DDR2
* **Placa de Vídeo (GPU):** ATI Radeon HD 5450 (80 *Stream Cores*, Clock 650 MHz, 512 MB VRAM DDR2 400 MHz)
* **Sistema Operacional:** Microsoft Windows 7 Professional 64-bit
* **APIs / Ferramentas:** OpenGL 4.0, OpenCL 1.1, Visual Studio 2010, Openframeworks 0071

Para cada medição registrada, foram realizadas 10 execuções independentes de 1.000 amostras cada, descartando-se os valores extremos (limítrofes) para cálculo das médias estatísticas.

---

## 6.5 Resultados de Desempenho e Benchmarks Comparativos

### Tabela 6.1: Tempo Gasto nas Etapas de Configuração
| Etapa de Configuração | Camada Responsável | Tempo Médio (segundos) |
|---|---|---|
| **Criar Contexto OpenCL** | *Displacement Generator* | 1,069039 s |
| **Carregar Textura e Registrar no OpenGL** | *Displacement Manager* | 0,005773 s |
| **Registrar no OpenCL (Interoperabilidade)** | *Interoperability Manager* | **0,000009 s** |

*Observação: A etapa de registro de interoperabilidade consome apenas 9 microssegundos, comprovando o custo desprezível do mapeamento compartilhado em VRAM.*

---

### Tabela 6.2: Tempo de Execução dos Kernels — GPU (OpenCL) vs CPU (Milissegundos)
| Função / Kernel | GPU (Kernel OpenCL) | CPU (Raio Mínimo) | CPU (Raio Máximo) |
|---|---|---|---|
| **Contact** | **0,333 ms** | 0,843 ms | 1,636 ms |
| **Force** | **0,367 ms** | 0,858 ms | 1,823 ms |
| **Morphing** | **0,310 ms** | 0,934 ms | *N/A* |
| **Custom** | **0,279 ms** | 0,841 ms | *N/A* |
| **Test** | **0,321 ms** | 0,852 ms | 1,647 ms |

*(Dimensão da textura de deslocamento: $64 	imes 64$ texels)*

![Figura 6.7: Gráfico Comparativo CPU x GPU](./figures/fig_6_7_grafico_comparativo_cpu_gpu.png)  
*Figura 6.7: Comparação do tempo de processamento por função: a execução na GPU mantém tempo constante (~0,3 ms) e independente do raio de ação, enquanto na CPU o custo dobra com o aumento da área.*

---

### Benchmarks de Taxa de Quadros (FPS) por Fator de Tessellation (1 a 64)

#### Tabela 6.3: Cena de Contato (*Contact Scene*) — FPS
| Modo de Visualização | Uso do Kernel | TF = 1 | TF = 8 | TF = 16 | TF = 24 | TF = 32 | TF = 40 | TF = 48 | TF = 56 | TF = 64 |
|---|---|---|---|---|---|---|---|---|---|---|
| **Wireframe** | **Não** | 327,90 | 293,80 | 262,10 | 242,77 | 227,41 | 212,38 | 200,01 | 191,65 | 178,92 |
| **Wireframe** | **Sim** | 201,02 | 185,56 | 176,00 | 165,22 | 154,57 | 149,50 | 143,60 | 137,75 | 131,19 |
| **Fill (Texturizado)** | **Não** | 242,19 | 232,62 | 227,79 | 221,08 | 212,27 | 203,96 | 195,22 | 187,13 | 178,08 |
| **Fill (Texturizado)** | **Sim** | 164,17 | 158,69 | 155,17 | 150,38 | 148,71 | 144,99 | 140,18 | 135,32 | **130,96** |

![Figura 6.8: Gráfico de desempenho da cena de contato](./figures/fig_6_8_grafico_desempenho_contato.png)  
*Figura 6.8: Curvas de desempenho na ContactScene variando de 327 FPS (TF 1) até 130 FPS (TF 64 com kernel ativo e preenchimento).*

---

#### Tabela 6.4: Cena de Força (*Force Scene*) — FPS
| Modo de Visualização | Uso do Kernel | TF = 1 | TF = 8 | TF = 16 | TF = 24 | TF = 32 | TF = 40 | TF = 48 | TF = 56 | TF = 64 |
|---|---|---|---|---|---|---|---|---|---|---|
| **Wireframe** | **Não** | 330,24 | 294,50 | 263,21 | 243,98 | 230,16 | 213,94 | 201,03 | 191,36 | 180,80 |
| **Wireframe** | **Sim** | 191,21 | 180,17 | 165,61 | 156,69 | 151,98 | 143,21 | 138,01 | 132,36 | 129,55 |
| **Fill (Texturizado)** | **Não** | 243,63 | 235,19 | 231,82 | 222,76 | 216,09 | 205,45 | 197,10 | 188,70 | 180,37 |
| **Fill (Texturizado)** | **Sim** | 158,82 | 156,29 | 152,87 | 149,96 | 146,55 | 142,73 | 137,78 | 131,65 | **129,09** |

![Figura 6.9: Gráfico de desempenho da cena de força](./figures/fig_6_9_grafico_desempenho_forca.png)  
*Figura 6.9: Desempenho na ForceScene mantendo-se consistentemente acima de 129 FPS.*

---

#### Tabela 6.5: Cena de Morphing (*Morphing Scene*) — FPS
| Modo de Visualização | Uso do Kernel | TF = 1 | TF = 8 | TF = 16 | TF = 24 | TF = 32 | TF = 40 | TF = 48 | TF = 56 | TF = 64 |
|---|---|---|---|---|---|---|---|---|---|---|
| **Wireframe** | **Não** | 330,53 | 291,20 | 254,40 | 238,35 | 216,35 | 201,79 | 189,94 | 178,66 | 168,09 |
| **Wireframe** | **Sim** | 216,45 | 199,79 | 183,64 | 171,01 | 161,61 | 154,85 | 146,77 | 140,34 | 133,38 |
| **Fill (Texturizado)** | **Não** | 264,80 | 241,87 | 226,30 | 216,70 | 206,34 | 199,32 | 193,40 | 182,36 | 172,68 |
| **Fill (Texturizado)** | **Sim** | 186,05 | 170,41 | 165,97 | 160,14 | 156,72 | 150,79 | 145,89 | 141,80 | **136,59** |

![Figura 6.10: Gráfico de desempenho da cena de morphing](./figures/fig_6_10_grafico_desempenho_morphing.png)  
*Figura 6.10: Curvas de desempenho na MorphingScene sob ciclo contínuo de interpolação.*

---

#### Tabela 6.6: Cena Personalizável (*Custom Scene*) — FPS
| Modo de Visualização | Uso do Kernel | TF = 1 | TF = 8 | TF = 16 | TF = 24 | TF = 32 | TF = 40 | TF = 48 | TF = 56 | TF = 64 |
|---|---|---|---|---|---|---|---|---|---|---|
| **Wireframe** | **Não** | 326,74 | 294,85 | 266,86 | 244,86 | 229,05 | 214,56 | 200,87 | 190,40 | 180,32 |
| **Wireframe** | **Sim** | 195,71 | 184,31 | 169,01 | 163,48 | 153,83 | 149,60 | 142,95 | 136,87 | 131,30 |
| **Fill (Texturizado)** | **Não** | 263,57 | 238,78 | 231,04 | 222,18 | 213,33 | 205,19 | 195,95 | 188,30 | 178,73 |
| **Fill (Texturizado)** | **Sim** | 171,29 | 160,36 | 154,18 | 152,58 | 144,94 | 142,76 | 140,51 | 135,62 | **130,64** |

![Figura 6.11: Gráfico de desempenho da cena personalizável](./figures/fig_6_11_grafico_desempenho_custom.png)  
*Figura 6.11: Curvas de desempenho na CustomScene.*

---

#### Tabela 6.7: Cena de Teste / Escultura (*Test Scene*) — FPS
| Modo de Visualização | Uso do Kernel | TF = 1 | TF = 8 | TF = 16 | TF = 24 | TF = 32 | TF = 40 | TF = 48 | TF = 56 | TF = 64 |
|---|---|---|---|---|---|---|---|---|---|---|
| **Wireframe** | **Não** | 311,18 | 278,87 | 250,26 | 232,79 | 215,23 | 202,26 | 191,01 | 180,37 | 169,41 |
| **Wireframe** | **Sim** | 181,23 | 172,94 | 161,34 | 153,91 | 141,81 | 140,04 | 133,62 | 129,19 | 123,15 |
| **Fill (Texturizado)** | **Não** | 242,14 | 222,51 | 216,06 | 207,59 | 199,59 | 192,33 | 184,39 | 175,49 | 167,43 |
| **Fill (Texturizado)** | **Sim** | 157,17 | 148,33 | 145,42 | 142,07 | 139,14 | 131,06 | 127,96 | 121,61 | **120,35** |

![Figura 6.12: Gráfico de desempenho da cena de teste](./figures/fig_6_12_grafico_desempenho_test.png)  
*Figura 6.12: Desempenho durante a escultura interativa em tempo real (TestScene), sustentando 120 FPS mesmo no nível máximo de subdivisão.*

---

# 7. Conclusão e Trabalhos Futuros

## 7.1 Síntese dos Resultados Alcançados

O trabalho comprovou a viabilidade técnica e a eficiência arquitetural da manipulação dinâmica de mapas de deslocamento na GPU para jogos digitais:
* **Taxas de Quadros Elevadas:** Todas as cenas mantiveram taxas de renderização superiores a **120 FPS** sob fator máximo de tessellation ($	ext{TF} = 64$), mesmo operando em uma GPU de entrada com apenas 80 núcleos de fluxo.
* **Gargalo Zero de Barramento:** O mecanismo de interoperabilidade OpenCL/OpenGL reduziu o tempo de sincronização para a faixa de microssegundos ($0,000009	ext{ s}$), eliminando cópias desnecessárias via barramento PCIe.
* **Isolamento de Responsabilidades:** A separação em camadas permitiu desacoplar completamente os algoritmos de deformação dos programas de renderização gráfica, simplificando a escrita de *shaders* e facilitando a depuração.
* **Portabilidade Real:** A adoção de padrões abertos da indústria (C++, OpenGL e OpenCL) garantiu suporte multiplataforma e independência de fornecedor de placa de vídeo.

## 7.2 Trabalhos Futuros

1. **Fusão Multicamadas de Shading:** Integração do pipeline proposto com técnicas complementares de *Bump Mapping* e *Parallax Occlusion Mapping* para micro-detalhamento sub-triângulo.
2. **Animação Procedural de Tecidos e Superfícies Orgânicas:** Aplicação do *Morphing Kernel* para simulação de tecidos, capas, bandeiras, bem como deformação progressiva de rugas de pele e envelhecimento de avatares.
3. **Simulação Geológica e Erosão:** Expansão dos *kernels* para modelar dinâmica de fluidos e erosão hídrica/eólica sobre relevos montanhosos em tempo real.
4. **Acoplamento com Motores de Física (ex.: Jolt Physics / PhysX):** Leitura direta dos mapas de deslocamento em VRAM para geração procedural de *heightfield colliders* sincronizados com a malha visual.
5. **Shaders de Modificação Dinâmica de Albedo / Texturas:** Acoplar a deformação física a alterações de cor da superfície (ex.: escurecimento e queima de terreno no epicentro de explosões geradas pelo *Force Kernel*).

---

# 8. Referências Bibliográficas

* [ALEXANDER et al. 1977] ALEXANDER, C.; ISHIKAWA, S.; SILVERSTEIN, M.; IACOBSON, M.; FIKSDAHL-KING, I.; ANGEL, S. *A Pattern Language*. Oxford University Press, New York, 1977.
* [ANDRADE and CLUA 2011] ANDRADE, F. C.; CLUA, E. W. G. Using real time hardware tessellation for morphing of geometry in GPU. In: *II Workshop Argentino sobre Videojuegos (WAVI 2011)*, Buenos Aires. Actas del WAVI 2011. Bahía Blanca: Editorial de la Universidad Nacional del Sur, v. 2, p. 49-63, 2011.
* [ANDRADE et al. 2012] ANDRADE, F. C.; SHAFAATDOOST, M.; CONCI, A.; CLUA, E. W. G. Displacement Stage: Arquitetura extensível para mapas dinâmicos de deslocamento na GPU. In: *XI Brazilian Symposium on Computer Games and Digital Entertainment (SBGames 2012)*, Brasília, p. 37-40, 2012.
* [BASS et al. 2003] BASS, L.; CLEMENTS, P.; KAZMAN, R. *Software Architecture in Practice*. 2. ed. Boston: Addison-Wesley, 2003.
* [BATISTA 2011] BATISTA, M. L. S. *Simulação de emoções em faces humanas utilizando os algoritmos de bump mapping e morphing implementados na GPU*. Dissertação (Mestrado em Computação), Universidade Federal Fluminense, Niterói, 2011.
* [BLINN 1978] BLINN, J. F. Simulation of wrinkled surfaces. In: *ACM SIGGRAPH 78*, p. 286-292, 1978.
* [BUSCHMANN et al. 1996] BUSCHMANN, F. et al. *Pattern-Oriented Software Architecture: A System of Patterns*. Chichester: John Wiley & Sons, 1996.
* [CATMULL 1974] CATMULL, E. *A subdivision algorithm for computer display of curved surfaces*. Ph.D. Thesis, Department of Computer Science, University of Utah, Salt Lake City, 1974.
* [CONCI et al. 2008] CONCI, A.; AZEVEDO, E.; LETA, F. *Computação Gráfica: Teoria e Prática*. v. 2. Rio de Janeiro: Elsevier, 2008.
* [COOK 1984] COOK, R. L. Shade trees. In: *ACM SIGGRAPH 84*, p. 223-231, 1984.
* [GAMMA et al. 1995] GAMMA, E.; HELM, R.; JOHNSON, R.; VLISSIDES, J. *Design Patterns: Elements of Reusable Object-Oriented Software*. Reading: Addison-Wesley, 1995.
* [KIRK and HWU 2011] KIRK, D. B.; HWU, W. W. *Programando para Processadores Paralelos: Uma Abordagem Prática à Programação de GPU*. Rio de Janeiro: Elsevier, 2011.
* [LOOP and SCHAEFER 2008] LOOP, C.; SCHAEFER, S. Approximating Catmull-Clark subdivision surfaces with bicubic patches. *ACM Transactions on Graphics (TOG)*, v. 27, n. 1, p. 1-11, 2008.
* [NI et al. 2009] NI, T.; CASTAÑO, I.; PETERS, J.; MITCHELL, J.; SCHNEIDER, P.; VERMA, V. Efficient substitutes for subdivision surfaces. In: *ACM SIGGRAPH 2009 Courses*, n. 13, 2009.
* [NOVAK 2011] NOVAK, J. *Desenvolvimento de Games*. São Paulo: Cengage Learning, 2011.
* [NUNES 2011] NUNES, G. B. *Explorando aplicações que usam geração de vértices em GPU*. Dissertação (Mestrado em Informática), Pontifícia Universidade Católica do Rio de Janeiro (PUC-Rio), Rio de Janeiro, 2011.
* [PHARR and HANRAHAN 1996] PHARR, M.; HANRAHAN, P. Geometry caching for ray-tracing displacement maps. In: *7th Eurographics Workshop on Rendering*, p. 31-40, 1996.
* [PRESSMAN 2002] PRESSMAN, R. S. *Engenharia de Software*. 5. ed. Rio de Janeiro: McGraw-Hill, 2002.
* [RABIN 2012] RABIN, S. *Introdução ao Desenvolvimento de Games*. v. 2. São Paulo: Cengage Learning, 2012.
* [ROLLINGS and MORRIS 2000] ROLLINGS, A.; MORRIS, D. *Game Architecture and Design*. Scottsdale: The Coriolis Group, 2000.
* [SCHEIN et al. 2005] SCHEIN, S.; KARPEN, E.; ELBER, G. Real-time geometric deformation displacement maps using programmable hardware. *The Visual Computer*, v. 21, n. 8-10, p. 791-800, 2005.
* [SCHOENBERG 2001] SCHOENBERG, F. P. *Tessellations*. Department of Statistics, University of California, Los Angeles, 2001.
* [SNOOK 2003] SNOOK, G. *Real-Time 3D Terrain Engines Using C++ and DirectX 9*. Massachusetts: Charles River Media, 2003.
* [TAKAHASHI and MIYATA 2005] TAKAHASHI, M.; MIYATA, K. GPU based interactive displacement mapping. In: *International Workshop on Advanced Image Technology*, p. 105-108, 2005.
* [TATARCHUK et al. 2009] TATARCHUK, N.; BARCZAK, J.; BILODEAU, B. *Programming for Real-Time Tessellation on GPU*. AMD/ATI Whitepaper, 2009.
* [TATARINOV 2008] TATARINOV, A. Instanced tessellation in DirectX 10. In: *Game Developers Conference (GDC 2008)*, 2008.
* [TORTELLI and WALTER 2007] TORTELLI, D. M.; WALTER, M. Implementação da técnica de displacement mapping em hardware gráfico. In: *VI Simpósio Brasileiro de Jogos para Computador e Entretenimento Digital (SBGames 2007)*, p. 1-4, 2007.
* [WANG et al. 2003] WANG, L.; WANG, X.; TONG, X.; LIN, S.; HU, S.; GUO, B.; SHUM, H. View-dependent displacement mapping. *ACM Transactions on Graphics (TOG)*, v. 22, n. 3, p. 334-339, 2003.
* [WOLBERG 1998] WOLBERG, G. Image morphing: a survey. *The Visual Computer*, v. 14, n. 8-9, p. 360-372, 1998.
* [ZAMITH et al. 2009] ZAMITH, M.; CLUA, E. W. G.; MONTENEGRO, A.; PASSOS, E.; LEAL, R.; CONCI, A. Real time feature-based parallel morphing in GPU applied to texture-based animation. In: *16th International Workshop on Systems, Signals and Image Processing (IWSSIP 2009)*, Chalkida. IEEE Region 8, p. 145-150, 2009.