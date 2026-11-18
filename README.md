## Como executar

Para executar o CineRush no Windows, é necessário ter
GCC, Raylib e Make instalados e configurados.

Abra o terminal na pasta principal do projeto e execute:

    make

Após a compilação, execute:

    ./CineRush.exe

## Sistema de atendimento

Após montar a programação, o jogador inicia o expediente do cinema, no qual deve atender clientes que chegam em grupos e entram em uma fila.

Cada cliente solicita um filme da programação, uma quantidade de ingressos e, opcionalmente, pipoca. O jogador precisa preparar e entregar o pedido corretamente antes que a paciência do cliente acabe.

A fila utiliza a estrutura FIFO (First In, First Out), atendendo os clientes na ordem de chegada, com capacidade máxima de quatro pessoas. Quando a fila está cheia, novos clientes são recusados, gerando penalidades.

## Controles do atendimento

- **1 a 8:** selecionar o filme solicitado.
- **Setas esquerda/direita:** ajustar a quantidade de ingressos.
- **P:** adicionar ou remover pipoca do pedido.
- **Espaço:** confirmar e entregar o pedido.

## Dificuldade progressiva

O jogo possui três níveis de dificuldade, que modificam a frequência de chegada dos clientes, a quantidade de pessoas por grupo, a paciência e a complexidade dos pedidos.

| Característica | Nível 1 | Nível 2 | Nível 3 |
|---|---|---|---|
| Clientes por grupo | 1–2 | 1–3 | 2–3 |
| Intervalo de chegada | 8 s | 7 s | 6 s |
| Paciência | 15 s | 12 s | 10 s |
| Ingressos por pedido | 1–2 | 1–3 | 1–4 |

## Pontuação e resultados

- **Atendimento correto:** +20 pontos.
- **Pedido incorreto:** penalidade conforme o nível.
- **Cliente que perde a paciência:** penalidade conforme o nível.
- **Fila cheia:** penalidade por cliente recusado.

O expediente dura 45 segundos. Ao final, a pontuação da programação é combinada com os resultados dos atendimentos.

Para concluir o nível, o jogador precisa cumprir as metas de filmes e pontuação e realizar pelo menos cinco atendimentos corretos.

## Ranking

O jogo possui um ranking que registra as pontuações dos jogadores, permitindo comparar os resultados obtidos nas partidas.

## Testes

A pasta `testes` contém os arquivos `testefila.c` e `testeatendimento.c`, utilizados para verificar o funcionamento das estruturas de dados e das mecânicas de atendimento.