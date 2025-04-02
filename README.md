# Paint em OpenGL - Computação Gráfica 

Este é um projeto desenvolvido para a disciplina de **Computação Gráfica** da **Universidade Federal do Cariri (UFCA)**. O objetivo é criar uma aplicação em **OpenGL** e **C** que funcione como um editor gráfico simples, semelhante ao *Paint*.

## 🎨 Funcionalidades

### 🎭 1. Criação de Objetos Primários
- **Ponto**
- **Segmento de reta**
- **Polígono**

O usuário pode criar quantos objetos desejar.

### 🔍 2. Seleção e Exclusão de Objetos
- **Selecionar objetos individualmente** (ponto, segmento de reta e polígono)
- **Excluir objetos selecionados**

### 🔄 3. Transformações Geométricas
- **Translação (arrastar e soltar)**
- **Rotação em relação ao centro do objeto**
  - No caso do ponto, é em relação à origem do eixo de coordenadas
- **Escala em relação ao centro do objeto** (exceto para ponto)
- **Reflexão**
- **Cisalhamento**

### 🖱️ 4. Entrada de Evento
As ações podem ser executadas por meio de:
- **Clique do mouse**
- **Botão de rolagem do mouse**
- **Teclado**

### 💾 5. Salvamento e Carregamento de Objetos 
- **Salvar os objetos geométricos criados em um arquivo**
- **Carregar os objetos geométricos de um arquivo**

### 🎥 6. Ponto Extra: Animações Criativas
Implementação de uma funcionalidade extra para animar os objetos criados.

## 📁 Estrutura do Projeto

```plaintext
/
├── include/               # Arquivos de cabeçalho
│   ├── draw.h             # Header para funções de desenho
│   ├── transform.h        # Header para transformações geométricas
│   ├── arqs.h             # Header para manipulação de arquivos
|   ├── data.h             # arquivos dos structs e cabeçalhs das funfções principais
|   ├── calculate.h        # Header para funções de calculos geométricos
├── README.md              # Este arquivo
└── main.c                 # Documentação do projeto
```

## ⚙️ Como Compilar e Executar

```
Em Manutenção
```

## 🛠️ Tecnologias Utilizadas
- **Linguagem**: C
- **Bibliotecas**: OpenGL, GLUT
- **Compilador**: GCC

## 👥 Integrantes da Equipe
- **José Denis**
- **Raphael Sousa Rabelo Rates**
- **Gabriela Queiroga**

---
Projeto desenvolvido para a disciplina de **Computação Gráfica** - Universidade Federal do Cariri (UFCA).

