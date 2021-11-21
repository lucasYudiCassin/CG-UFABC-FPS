# FPS Aim Trainer

Esse é um modelo de um jogo, criado para a disciplina de computação gráfica da UFABC - 2021.Q3

Baseado no [AIMLAB](https://aimlab.gg/) 

**[Clique aqui para visualizar!](https://lucasyudicassin.github.io/fpsAimTrainerV1/index.html)**

Desenvolvido pelos alunos:
 - Fernando Kenji - RA: 11201722146
 - Lucas Yudi - RA: 11201812210

# Dependência

Foi utilizada a biblioteca [ABCg](https://github.com/hbatagelo/abcg)

Desenvolvida por Harlen Batagelo


# O modelo
Este é um modelo em 3d de um jogo de prática de tiro.

O quarto e os alvos foram desenvolvidos utilizando o [blender](https://www.blender.org/), e a arma utilizada do [open 3d model](https://open3dmodel.com/pt/3d-models/scifi-handgun_39931.html)

A disposição já está no formato que será utilizada no jogo


# Técnicas
Utilizamos matrizes de transformações para cada objeto, mudando a escala, a rotação e a posição. Essas mudanças são colocadas na matriz de modelo, onde cada modelo possui sua própria matriz.
