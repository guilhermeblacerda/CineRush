# CineRush


Projeto desenvolvido em C para a disciplina de Algoritmos e Estruturas de Dados (AED).

O CineRush é um jogo em que o jogador organiza uma sessão de cinema, selecionando filmes de um catálogo com o objetivo de obter a maior pontuação possível, respeitando o limite de 300 minutos.

## Estruturas e algoritmos

- **Lista simplesmente encadeada:** armazena os filmes selecionados e permite operações de inserção, remoção e consulta.
- **Insertion Sort:** ordena o catálogo por duração, nota ou popularidade.
- **Raylib:** biblioteca utilizada para a interface gráfica e interação com o jogador.

## Como executar

É necessário ter um compilador C (GCC) e a biblioteca raylib configurados.

Na pasta do projeto, compile os arquivos `main.c`, `game.c`, `filmes.c` e `lista.c`, vinculando a biblioteca raylib e suas dependências.

Execute o programa gerado para iniciar o jogo.

## Controles

- **Enter:** iniciar ou finalizar a partida e retornar ao menu.
- **1 a 8:** selecionar filmes.
- **Backspace:** remover o último filme selecionado.
- **D / N / P:** ordenar por duração, nota ou popularidade.
- **Esc:** sair do jogo.


## Desafios

Cada partida apresenta um desafio sorteado entre seis objetivos: Noite dos Críticos, Sessão de Sucesso, Tempo é Ouro, Festival Diversificado, Maratona Cinematográfica e Obras-Primas.

A pontuação final é calculada de acordo com o objetivo sorteado. No menu inicial, pressione **H** para consultar os desafios.