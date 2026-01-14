#ifndef CELULARES_H_INCLUDED
#define CELULARES_H_INCLUDED

#include <stdio.h>

typedef struct Celular {
    int codc;
    char marcac[50];
    char modeloc[50];
    double precoc;
} TCelular;

TCelular *celular(int codc, char *marcac, char *modeloc, double precoc);
void salva_celular(TCelular *cel, FILE *out);
TCelular *le_celular(FILE *in);
void imprime_celular(TCelular *cel);
int qtdRegistros_cel(FILE *arq);
void imprimirBaseCelular(FILE *in);
void criarBaseDesordenadaCelular(FILE *in, int tam, int qtdTrocas);
TCelular *buscaSequencial_cel(int chave, FILE *in);
TCelular *busca_binaria_cel(int chave, FILE *in, int inicio, int fim);
int classificacao_interna_cel(FILE *arq, int M);
void intercalacao_basica_cel(FILE *out, int num_p);

// Hash
void insere_cel(FILE *arq_Hash, TCelular *novo);
void exclui_cel(FILE *arq_Hash, int codc);
TCelular* busca_cel(FILE *arq_Hash, int codc);
void tabela_hash_cel(FILE *arq_Hash, FILE *arq);
void imprimir_tabela_hash_cel(FILE *arq_Hash);

#endif
