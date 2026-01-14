#include "celulares.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define NUM_MARCAS 5
#define NUM_MODELOS 5
#define TABLE_SIZE 10

TCelular *celular(int codc, char *marcac, char *modeloc, double precoc) {
    TCelular *cel = (TCelular *) malloc(sizeof(TCelular));
    if (cel) memset(cel, 0, sizeof(TCelular));
    cel->codc = codc;
    strcpy(cel->marcac, marcac);
    strcpy(cel->modeloc, modeloc);
    cel->precoc = precoc;
    return cel;
}

void salva_celular(TCelular *cel, FILE *out) {
    fwrite(&cel->codc, sizeof(int), 1, out);
    fwrite(cel->marcac, sizeof(char), sizeof(cel->marcac), out);
    fwrite(cel->modeloc, sizeof(char), sizeof(cel->modeloc), out);
    fwrite(&cel->precoc, sizeof(double), 1, out);
}

TCelular *le_celular(FILE *in) {
    TCelular *cel = (TCelular *) malloc(sizeof(TCelular));
    if (0 >= fread(&cel->codc, sizeof(int), 1, in)) {
        free(cel);
        return NULL;
    }
    fread(cel->marcac, sizeof(char), sizeof(cel->marcac), in);
    fread(cel->modeloc, sizeof(char), sizeof(cel->modeloc), in);
    fread(&cel->precoc, sizeof(double), 1, in);
    return cel;
}

void imprime_celular(TCelular *cel) {
    printf("**********************************************\n");
    printf("Celular Cod: %d\nMarca: %s\nModelo: %s\nPreco: %.2f\n",
           cel->codc, cel->marcac, cel->modeloc, cel->precoc);
    printf("**********************************************\n");
}

int tamanho_registro_celular() {
    return sizeof(int) + sizeof(char) * 50 + sizeof(char) * 50 + sizeof(double);
}

int qtdRegistros_cel(FILE *arq) {
    fseek(arq, 0, SEEK_END);
    return trunc(ftell(arq) / tamanho_registro_celular());
}

void imprimirBaseCelular(FILE *out) {
    printf("--- BASE DE DADOS: CELULARES ---\n");
    rewind(out);
    TCelular *c;
    while ((c = le_celular(out)) != NULL) {
        imprime_celular(c);
        free(c);
    }
}

// CORRECAO: Nome especifico
void embaralha_cel(int *vet, int max, int trocas) {
    srand(time(NULL));
    for (int i = 0; i <= trocas; i++) {
        int j = rand() % (max);
        int k = rand() % (max);
        int tmp = vet[j];
        vet[j] = vet[k];
        vet[k] = tmp;
    }
}

void criarBaseDesordenadaCelular(FILE *in, int tam, int qtdTrocas) {
    int vet[tam];
    for (int i = 0; i < tam; i++) vet[i] = i + 1;
    embaralha_cel(vet, tam, qtdTrocas); // Chamada corrigida

    const char *marcas[NUM_MARCAS] = {"Samsung", "Apple", "Xiaomi", "Motorola", "Huawei"};
    const char *modelos[NUM_MODELOS] = {"Galaxy", "iPhone", "Redmi", "Moto G", "P40"};

    for (int i = 0; i < tam; i++) {
        const char *marca = marcas[rand() % NUM_MARCAS];
        const char *modelo = modelos[rand() % NUM_MODELOS];
        double preco = 500.0 + (rand() % 1000);
        TCelular *c = celular(vet[i], (char*)marca, (char*)modelo, preco);
        salva_celular(c, in);
        free(c);
    }
}

TCelular *buscaSequencial_cel(int chave, FILE *in) {
    TCelular *c;
    rewind(in);
    while ((c = le_celular(in)) != NULL) {
        if (c->codc == chave) return c;
        free(c);
    }
    return NULL;
}

TCelular *busca_binaria_cel(int chave, FILE *in, int inicio, int fim) {
    TCelular *c = NULL;
    int codc = -1;
    while (inicio <= fim && codc != chave) {
        int meio = trunc((inicio + fim) / 2);
        fseek(in, (meio) * tamanho_registro_celular(), SEEK_SET);
        c = le_celular(in);
        if (c) {
            codc = c->codc;
            if (codc > chave) {
                fim = meio - 1;
                free(c);
            } else if (codc < chave) {
                inicio = meio + 1;
                free(c);
            } else return c;
        } else break;
    }
    return NULL;
}

int classificacao_interna_cel(FILE *arq, int M) {
    rewind(arq);
    int reg = 0;
    int nCel = qtdRegistros_cel(arq);
    int qtdParticoes = 0;
    int t = tamanho_registro_celular();
    char nomeParticao[20];

    while (reg != nCel) {
        TCelular *v[M];
        int i = 0;
        while (!feof(arq) && i < M) {
            fseek(arq, reg * t, SEEK_SET);
            TCelular *c = le_celular(arq);
            if(c) {
                v[i] = c;
                i++;
                reg++;
            } else break;
        }
        if (i == 0) break;

        for (int j = 1; j < i; j++) {
            TCelular *c = v[j];
            int k = j - 1;
            while ((k >= 0) && (v[k]->codc > c->codc)) {
                v[k + 1] = v[k];
                k--;
            }
            v[k + 1] = c;
        }

        sprintf(nomeParticao, "partition_cel%i.dat", qtdParticoes);
        FILE *p = fopen(nomeParticao, "wb+");
        if (p) {
            for (int k = 0; k < i; k++) salva_celular(v[k], p);
            fclose(p);
            qtdParticoes++;
        }
        for (int jj = 0; jj < i; jj++) free(v[jj]);
    }
    return qtdParticoes;
}

void intercalacao_basica_cel(FILE *out, int num_p) {
    typedef struct {
        TCelular *cel;
        FILE *c;
    } TVet;

    char nome[20];
    TVet v[num_p];
    int fim = 0;

    for (int i = 0; i < num_p; i++) {
        sprintf(nome, "partition_cel%i.dat", i);
        v[i].c = fopen(nome, "rb");
        if (v[i].c) {
            v[i].cel = le_celular(v[i].c);
            if (!v[i].cel) v[i].cel = celular(INT_MAX, "", "", 0);
        } else {
            v[i].cel = celular(INT_MAX, "", "", 0);
        }
    }

    while (!fim) {
        int menor = INT_MAX;
        int pos_menor = -1;
        for (int i = 0; i < num_p; i++) {
            if (v[i].cel->codc < menor) {
                menor = v[i].cel->codc;
                pos_menor = i;
            }
        }
        if (menor == INT_MAX) {
            fim = 1;
        } else {
            salva_celular(v[pos_menor].cel, out);
            free(v[pos_menor].cel);
            v[pos_menor].cel = le_celular(v[pos_menor].c);
            if (!v[pos_menor].cel) v[pos_menor].cel = celular(INT_MAX, "", "", 0);
        }
    }
    for (int i = 0; i < num_p; i++) if(v[i].c) fclose(v[i].c);
}

// --- Funcoes Hash Celular ---
void insere_particao_cel(int posicao, TCelular *novo) {
    char nomeParticao[20];
    sprintf(nomeParticao, "partition_cel_%d.dat", posicao);
    FILE *p = fopen(nomeParticao, "rb+");
    if (!p) p = fopen(nomeParticao, "wb+");

    if (p) {
        TCelular temp;
        while (fread(&temp, sizeof(TCelular), 1, p) == 1) {
            if (temp.codc == novo->codc) { fclose(p); return; }
        }
        fwrite(novo, sizeof(TCelular), 1, p);
        fclose(p);
    }
}

void insere_cel(FILE *arq_Hash, TCelular *novo) {
    int pos = novo->codc % TABLE_SIZE;
    fseek(arq_Hash, pos * sizeof(TCelular), SEEK_SET);
    TCelular temp;
    fread(&temp, sizeof(TCelular), 1, arq_Hash);

    if (temp.codc == -1) {
        fseek(arq_Hash, pos * sizeof(TCelular), SEEK_SET);
        fwrite(novo, sizeof(TCelular), 1, arq_Hash);
    } else if (temp.codc != novo->codc) {
        insere_particao_cel(pos, novo);
    }
}

void exclui_cel(FILE *arq_Hash, int codc) {
    int pos = codc % TABLE_SIZE;
    fseek(arq_Hash, pos * sizeof(TCelular), SEEK_SET);
    TCelular temp;
    fread(&temp, sizeof(TCelular), 1, arq_Hash);

    char nomeParticao[20];
    sprintf(nomeParticao, "partition_cel_%d.dat", pos);

    if (temp.codc == codc) {
        temp.codc = -1;
        fseek(arq_Hash, pos * sizeof(TCelular), SEEK_SET);
        fwrite(&temp, sizeof(TCelular), 1, arq_Hash);

        FILE *p = fopen(nomeParticao, "rb");
        if(p) {
             TCelular promo;
             if(fread(&promo, sizeof(TCelular), 1, p) == 1) {
                 fseek(arq_Hash, pos * sizeof(TCelular), SEEK_SET);
                 fwrite(&promo, sizeof(TCelular), 1, arq_Hash);

                 FILE *tempP = fopen("temp_cel.dat", "wb");
                 TCelular aux;
                 while(fread(&aux, sizeof(TCelular), 1, p) == 1) {
                     fwrite(&aux, sizeof(TCelular), 1, tempP);
                 }
                 fclose(p); fclose(tempP);
                 remove(nomeParticao);
                 rename("temp_cel.dat", nomeParticao);
             } else {
                 fclose(p);
                 remove(nomeParticao);
             }
        }
        printf("Registro excluido.\n");
    } else {
        FILE *p = fopen(nomeParticao, "rb");
        if(p) {
            FILE *tempP = fopen("temp_cel.dat", "wb");
            TCelular aux;
            int achou=0;
            while(fread(&aux, sizeof(TCelular), 1, p) == 1) {
                if(aux.codc == codc) achou=1;
                else fwrite(&aux, sizeof(TCelular), 1, tempP);
            }
            fclose(p); fclose(tempP);
            remove(nomeParticao);
            rename("temp_cel.dat", nomeParticao);
            if(achou) printf("Registro excluido da particao.\n");
            else printf("Registro nao encontrado.\n");
        } else {
            printf("Registro nao encontrado.\n");
        }
    }
}

TCelular* busca_cel(FILE *arq_Hash, int codc) {
    int pos = codc % TABLE_SIZE;
    fseek(arq_Hash, pos * sizeof(TCelular), SEEK_SET);
    TCelular *temp = (TCelular*) malloc(sizeof(TCelular));
    fread(temp, sizeof(TCelular), 1, arq_Hash);

    if (temp->codc == codc) return temp;

    free(temp);
    char nomeParticao[20];
    sprintf(nomeParticao, "partition_cel_%d.dat", pos);
    FILE *p = fopen(nomeParticao, "rb");
    if(p) {
        temp = (TCelular*) malloc(sizeof(TCelular));
        while(fread(temp, sizeof(TCelular), 1, p) == 1) {
            if(temp->codc == codc) { fclose(p); return temp; }
        }
        free(temp); fclose(p);
    }
    return NULL;
}

void tabela_hash_cel(FILE *arq_Hash, FILE *arq) {
    rewind(arq);
    TCelular Vazio; Vazio.codc = -1;
    for(int i=0; i<TABLE_SIZE; i++) fwrite(&Vazio, sizeof(TCelular), 1, arq_Hash);

    for(int i=0; i<TABLE_SIZE; i++) {
        char nome[30]; sprintf(nome, "partition_cel_%d.dat", i); remove(nome);
    }

    TCelular *c;
    while((c = le_celular(arq)) != NULL) {
        insere_cel(arq_Hash, c);
        free(c);
    }
    imprimir_tabela_hash_cel(arq_Hash);
}

void imprimir_tabela_hash_cel(FILE *arq_Hash) {
    rewind(arq_Hash);
    TCelular temp;
    printf("\n--- Tabela Hash Celulares ---\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        fseek(arq_Hash, i * sizeof(TCelular), SEEK_SET);
        fread(&temp, sizeof(TCelular), 1, arq_Hash);

        printf("[%d] ", i);
        if (temp.codc != -1) printf("COD %d (%s)", temp.codc, temp.marcac);
        else printf("LIVRE");

        // Particao
        char nome[20]; sprintf(nome, "partition_cel_%d.dat", i);
        FILE *p = fopen(nome, "rb");
        if(p) {
            TCelular t;
            while(fread(&t, sizeof(TCelular), 1, p)) printf(" -> [%d]", t.codc);
            fclose(p);
        }
        printf("\n");
    }
}
