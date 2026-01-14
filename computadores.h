#ifndef COMPUTADORES_H_INCLUDED
#define COMPUTADORES_H_INCLUDED

#include <stdio.h>

typedef struct Computador {
    int codpc;
    char marcapc[50];
    char modelopc[50];
    double precopc;
} TComputador;

// Funcoes Basicas
TComputador *computador(int codpc, char *marcapc, char *modelopc, double precopc);
void salva_computador(TComputador *comp, FILE *out);
TComputador *le_computador(FILE *in);
void imprime_computador(TComputador *comp);
int qtdRegistros_pc(FILE *arq);
void imprimirBaseComputador(FILE *in);
void criarBaseDesordenadaComputador(FILE *out, int tam, int qtdTrocas);

// Buscas e Ordenacao
TComputador *buscaSequencial_pc(int chave, FILE *in);
TComputador *busca_binaria_pc(int chave, FILE *in, int inicio, int fim);
int classificacao_interna_pc(FILE *in, int M);
void intercalacao_basica_pc(FILE *out, int num_p);

// Hash
void insere_pc(FILE *arq_Hash, TComputador *novo);
void exclui_pc(FILE *arq_Hash, int codpc);
TComputador* busca_pc(FILE *arq_Hash, int codpc);
void tabela_hash_pc(FILE *arq_Hash, FILE *arq);
void imprimir_tabela_hash(FILE *arq_Hash);
void imprimir_particao(const char *nomeParticao);

#endif
