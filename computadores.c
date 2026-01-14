#include "computadores.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define NUM_MARCAS 5
#define NUM_MODELOS 5
#define TABLE_SIZE 10

// Construtor
TComputador *computador(int codpc, char *marcapc, char *modelopc, double precopc) {
    TComputador *comp = (TComputador *) malloc(sizeof(TComputador));
    if (comp) memset(comp, 0, sizeof(TComputador));
    comp->codpc = codpc;
    strcpy(comp->marcapc, marcapc);
    strcpy(comp->modelopc, modelopc);
    comp->precopc = precopc;
    return comp;
}

void salva_computador(TComputador *comp, FILE *out) {
    fwrite(&comp->codpc, sizeof(int), 1, out);
    fwrite(comp->marcapc, sizeof(char), sizeof(comp->marcapc), out);
    fwrite(comp->modelopc, sizeof(char), sizeof(comp->modelopc), out);
    fwrite(&comp->precopc, sizeof(double), 1, out);
}

TComputador *le_computador(FILE *in) {
    TComputador *comp = (TComputador *) malloc(sizeof(TComputador));
    if (0 >= fread(&comp->codpc, sizeof(int), 1, in)) {
        free(comp);
        return NULL;
    }
    fread(comp->marcapc, sizeof(char), sizeof(comp->marcapc), in);
    fread(comp->modelopc, sizeof(char), sizeof(comp->modelopc), in);
    fread(&comp->precopc, sizeof(double), 1, in);
    return comp;
}

void imprime_computador(TComputador *comp) {
    printf("**********************************************\n");
    printf("PC Codigo: %d\nMarca: %s\nModelo: %s\nPreco: %.2f\n",
           comp->codpc, comp->marcapc, comp->modelopc, comp->precopc);
    printf("**********************************************\n");
}

int tamanho_registro_computador() {
    return sizeof(int) + sizeof(char) * 50 + sizeof(char) * 50 + sizeof(double);
}

int qtdRegistros_pc(FILE *arq) {
    fseek(arq, 0, SEEK_END);
    return trunc(ftell(arq) / tamanho_registro_computador());
}

void imprimirBaseComputador(FILE *in) {
    printf("--- BASE DE DADOS: COMPUTADORES ---\n");
    rewind(in);
    TComputador *comp;
    while ((comp = le_computador(in)) != NULL) {
        imprime_computador(comp);
        free(comp);
    }
}

// CORRECAO: Nome especifico para evitar conflito com celular
void embaralha_pc(int *vet, int max, int trocas) {
    srand(time(NULL));
    for (int i = 0; i <= trocas; i++) {
        int j = rand() % (max);
        int k = rand() % (max);
        int tmp = vet[j];
        vet[j] = vet[k];
        vet[k] = tmp;
    }
}

void criarBaseDesordenadaComputador(FILE *out, int tam, int qtdTrocas) {
    int vet[tam];
    for (int i = 0; i < tam; i++) vet[i] = i + 1;

    embaralha_pc(vet, tam, qtdTrocas); // Chamada corrigida

    const char *marcas[NUM_MARCAS] = {"Dell", "HP", "Apple", "Lenovo", "Asus"};
    const char *modelos[NUM_MODELOS] = {"Inspiron", "Pavilion", "MacBook", "ThinkPad", "ZenBook"};

    for (int i = 0; i < tam; i++) {
        const char *marca = marcas[rand() % NUM_MARCAS];
        const char *modelo = modelos[rand() % NUM_MODELOS];
        double preco = 1500.0 + (rand() % 1500);
        TComputador *c = computador(vet[i], (char*)marca, (char*)modelo, preco);
        salva_computador(c, out);
        free(c);
    }
}

TComputador *buscaSequencial_pc(int chave, FILE *in) {
    TComputador *c;
    rewind(in);
    while ((c = le_computador(in)) != NULL) {
        if (c->codpc == chave) return c;
        free(c);
    }
    return NULL;
}

TComputador *busca_binaria_pc(int chave, FILE *in, int inicio, int fim) {
    TComputador *c = NULL;
    int codpc = -1;
    while (inicio <= fim && codpc != chave) {
        int meio = trunc((inicio + fim) / 2);
        fseek(in, (meio) * tamanho_registro_computador(), SEEK_SET);
        c = le_computador(in);
        if(c) {
            codpc = c->codpc;
            if (codpc > chave) {
                fim = meio - 1;
                free(c);
            } else if (codpc < chave) {
                inicio = meio + 1;
                free(c);
            } else return c;
        } else break;
    }
    return NULL;
}

int classificacao_interna_pc(FILE *arq, int M) {
    rewind(arq);
    int reg = 0;
    int nComp = qtdRegistros_pc(arq);
    int qtdParticoes = 0;
    int t = tamanho_registro_computador();
    char nomeParticao[20];

    while (reg != nComp) {
        TComputador *v[M];
        int i = 0;
        while (!feof(arq) && i < M) {
            fseek(arq, reg * t, SEEK_SET);
            TComputador *c = le_computador(arq);
            if(c) {
                v[i] = c;
                i++;
                reg++;
            } else break;
        }
        if (i == 0) break;

        // Ordenacao por Insercao
        for (int j = 1; j < i; j++) {
            TComputador *c = v[j];
            int k = j - 1;
            while ((k >= 0) && (v[k]->codpc > c->codpc)) {
                v[k + 1] = v[k];
                k--;
            }
            v[k + 1] = c;
        }

        sprintf(nomeParticao, "partition%i.dat", qtdParticoes);
        FILE *p = fopen(nomeParticao, "wb+");
        if (p) {
            for (int k = 0; k < i; k++) salva_computador(v[k], p);
            fclose(p);
            qtdParticoes++;
        }
        for (int jj = 0; jj < i; jj++) free(v[jj]);
    }
    return qtdParticoes;
}

void intercalacao_basica_pc(FILE *out, int num_p) {
    typedef struct {
        TComputador *comp;
        FILE *c;
    } TVet;

    char nome[20];
    TVet v[num_p];
    int fim = 0;

    for (int i = 0; i < num_p; i++) {
        sprintf(nome, "partition%i.dat", i);
        v[i].c = fopen(nome, "rb");
        if (v[i].c) {
            v[i].comp = le_computador(v[i].c);
            if (!v[i].comp) v[i].comp = computador(INT_MAX, "", "", 0);
        } else {
            v[i].comp = computador(INT_MAX, "", "", 0);
        }
    }

    while (!fim) {
        int menor = INT_MAX;
        int pos_menor = -1;
        for (int i = 0; i < num_p; i++) {
            if (v[i].comp->codpc < menor) {
                menor = v[i].comp->codpc;
                pos_menor = i;
            }
        }
        if (menor == INT_MAX) {
            fim = 1;
        } else {
            salva_computador(v[pos_menor].comp, out);
            free(v[pos_menor].comp);
            v[pos_menor].comp = le_computador(v[pos_menor].c);
            if (!v[pos_menor].comp) v[pos_menor].comp = computador(INT_MAX, "", "", 0);
        }
    }
    for (int i = 0; i < num_p; i++) if(v[i].c) fclose(v[i].c);
}

// --- Funcoes Hash PC (Auxiliares e Principais) ---
void insere_particao_pc(int posicao, TComputador *novo) {
    char nomeParticao[20];
    sprintf(nomeParticao, "partition_pc_%d.dat", posicao);
    FILE *p = fopen(nomeParticao, "rb+");
    if (!p) p = fopen(nomeParticao, "wb+");

    if (p) {
        TComputador temp;
        // Evita duplicatas na partição
        while (fread(&temp, sizeof(TComputador), 1, p) == 1) {
            if (temp.codpc == novo->codpc) {
                fclose(p); return;
            }
        }
        fwrite(novo, sizeof(TComputador), 1, p);
        fclose(p);
    }
}

void insere_pc(FILE *arq_Hash, TComputador *novo) {
    int pos = novo->codpc % TABLE_SIZE;
    fseek(arq_Hash, pos * sizeof(TComputador), SEEK_SET);
    TComputador temp;
    fread(&temp, sizeof(TComputador), 1, arq_Hash);

    if (temp.codpc == -1) {
        fseek(arq_Hash, pos * sizeof(TComputador), SEEK_SET);
        fwrite(novo, sizeof(TComputador), 1, arq_Hash);
    } else if (temp.codpc != novo->codpc) {
        insere_particao_pc(pos, novo);
    }
}

void exclui_pc(FILE *arq_Hash, int codpc) {
    int pos = codpc % TABLE_SIZE;
    fseek(arq_Hash, pos * sizeof(TComputador), SEEK_SET);
    TComputador temp;
    fread(&temp, sizeof(TComputador), 1, arq_Hash);

    char nomeParticao[20];
    sprintf(nomeParticao, "partition_pc_%d.dat", pos);

    if (temp.codpc == codpc) {
        // Remover da tabela principal
        temp.codpc = -1;
        fseek(arq_Hash, pos * sizeof(TComputador), SEEK_SET);
        fwrite(&temp, sizeof(TComputador), 1, arq_Hash);

        // Tenta promover item da particao se existir
        FILE *p = fopen(nomeParticao, "rb");
        if(p) {
            TComputador promo;
            if(fread(&promo, sizeof(TComputador), 1, p) == 1) {
                fseek(arq_Hash, pos * sizeof(TComputador), SEEK_SET);
                fwrite(&promo, sizeof(TComputador), 1, arq_Hash);

                // Reescreve particao sem o primeiro
                FILE *tempP = fopen("temp_pc.dat", "wb");
                TComputador aux;
                while(fread(&aux, sizeof(TComputador), 1, p) == 1) {
                    fwrite(&aux, sizeof(TComputador), 1, tempP);
                }
                fclose(p); fclose(tempP);
                remove(nomeParticao);
                rename("temp_pc.dat", nomeParticao);
            } else {
                fclose(p);
                remove(nomeParticao); // Particao vazia
            }
        }
        printf("Registro excluido.\n");
    } else {
        // Tenta remover da particao
         FILE *p = fopen(nomeParticao, "rb");
         if(p) {
             FILE *tempP = fopen("temp_pc.dat", "wb");
             TComputador aux;
             int achou = 0;
             while(fread(&aux, sizeof(TComputador), 1, p) == 1) {
                 if(aux.codpc == codpc) achou = 1;
                 else fwrite(&aux, sizeof(TComputador), 1, tempP);
             }
             fclose(p); fclose(tempP);
             remove(nomeParticao);
             rename("temp_pc.dat", nomeParticao);
             if(achou) printf("Registro excluido da particao.\n");
             else printf("Registro nao encontrado.\n");
         } else {
             printf("Registro nao encontrado.\n");
         }
    }
}

TComputador* busca_pc(FILE *arq_Hash, int codpc) {
    int pos = codpc % TABLE_SIZE;
    fseek(arq_Hash, pos * sizeof(TComputador), SEEK_SET);
    TComputador *temp = (TComputador*) malloc(sizeof(TComputador));
    fread(temp, sizeof(TComputador), 1, arq_Hash);

    if (temp->codpc == codpc) return temp;

    // Procura na particao
    free(temp);
    char nomeParticao[20];
    sprintf(nomeParticao, "partition_pc_%d.dat", pos);
    FILE *p = fopen(nomeParticao, "rb");
    if(p) {
        temp = (TComputador*) malloc(sizeof(TComputador));
        while(fread(temp, sizeof(TComputador), 1, p) == 1) {
            if(temp->codpc == codpc) {
                fclose(p); return temp;
            }
        }
        free(temp);
        fclose(p);
    }
    return NULL;
}

void tabela_hash_pc(FILE *arq_Hash, FILE *arq) {
    rewind(arq);
    TComputador vazio; vazio.codpc = -1;
    for(int i=0; i<TABLE_SIZE; i++) fwrite(&vazio, sizeof(TComputador), 1, arq_Hash);

    // Limpar particoes antigas
    for(int i=0; i<TABLE_SIZE; i++) {
        char nome[30]; sprintf(nome, "partition_pc_%d.dat", i); remove(nome);
    }

    TComputador *c;
    while((c = le_computador(arq)) != NULL) {
        insere_pc(arq_Hash, c);
        free(c);
    }
    imprimir_tabela_hash(arq_Hash);
}

void imprimir_tabela_hash(FILE *arq_Hash) {
    rewind(arq_Hash);
    TComputador temp;
    printf("\n--- Tabela Hash PC ---\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        fseek(arq_Hash, i * sizeof(TComputador), SEEK_SET);
        fread(&temp, sizeof(TComputador), 1, arq_Hash);

        printf("[%d] ", i);
        if (temp.codpc != -1) printf("COD %d (%s)", temp.codpc, temp.marcapc);
        else printf("LIVRE");

        // Verifica particao
        char nomeParticao[20];
        sprintf(nomeParticao, "partition_pc_%d.dat", i);
        imprimir_particao(nomeParticao);
        printf("\n");
    }
}

void imprimir_particao(const char *nomeParticao) {
    FILE *p = fopen(nomeParticao, "rb");
    if (!p) return;
    TComputador c;
    while (fread(&c, sizeof(TComputador), 1, p) == 1) {
        printf(" -> [%d]", c.codpc);
    }
    fclose(p);
}
