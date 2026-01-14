#include <stdio.h>
#include <stdlib.h>
#include "computadores.h"
#include "celulares.h"
#include "compra.h"

int main()
{
    // Ponteiros para arquivos
    FILE *out, *out2;
    FILE *arq, *arq2;
    FILE *arq_Hash_pc, *arq_Hash_cel;

    // Flags para controle de estado
    int baseIntercaladaPc = 0;
    int baseIntercaladaCel = 0;

    printf("----------------------ESCADARIA STORE (SISTEMA UNIFICADO)----------------------\n");

    // Abertura inicial dos arquivos (Cria se não existirem)
    if ((arq = fopen("computadores.dat", "w+b")) == NULL || (arq2 = fopen("celulares.dat", "w+b")) == NULL) {
        printf("Erro critico: Nao foi possivel criar os arquivos de dados.\n");
        exit(1);
    }
    if ((out = fopen("pcintercalado.dat", "w+b")) == NULL || (out2 = fopen("celintercalado.dat", "w+b")) == NULL) {
        printf("Erro critico: Nao foi possivel criar arquivos temporarios.\n");
        exit(1);
    }
    if ((arq_Hash_pc = fopen("tabela_hash_pc.dat", "w+b")) == NULL || (arq_Hash_cel = fopen("tabela_hash_cel.dat", "w+b")) == NULL) {
        printf("Erro critico: Nao foi possivel criar tabelas hash.\n");
        exit(1);
    }

    int opcao, n, chave;
    int tam_cel = 0, tam_pc = 0;
    TComputador *comp = NULL;
    TCelular *cel = NULL;

    do
    {
        printf("\n================ MENU PRINCIPAL ================\n");
        printf("(1)  - Criar Base de Dados (Computadores)\n");
        printf("(2)  - Criar Base de Dados (Celulares)\n");
        printf("(3)  - Imprimir Base de Dados Atual\n");
        printf("(4)  - Busca Sequencial (Arquivo Desordenado)\n");
        printf("(5)  - Ordenar Base de Dados (Intercalacao)\n");
        printf("(6)  - Busca Binaria (Arquivo Ordenado)\n");
        printf("(7)  - Criar Tabela Hash (Indexacao)\n");
        printf("(8)  - Busca na Tabela Hash\n");
        printf("(9)  - Inserir Novo Produto (Hash + Arquivo)\n");
        printf("(10) - Excluir Produto\n");
        printf("(11) - REALIZAR COMPRA (Carrinho)\n");
        printf("(0)  - SAIR\n");
        printf("================================================\n");
        printf("Escolha uma opcao: ");
        scanf("%i", &opcao);

        switch (opcao)
        {
        case 0:
            printf("Saindo do sistema...\n");
            break;

        case 1: // Criar Base PC
            printf("\nQuantos computadores deseja gerar? ");
            scanf("%i", &n);
            tam_pc = n;
            freopen("computadores.dat", "wb+", arq); // Limpa arquivo
            criarBaseDesordenadaComputador(arq, tam_pc, tam_pc); // Usa func renomeada se necessario
            baseIntercaladaPc = 0;
            printf("Base de PCs criada com sucesso!\n");
            break;

        case 2: // Criar Base Celular
            printf("\nQuantos celulares deseja gerar? ");
            scanf("%i", &n);
            tam_cel = n;
            freopen("celulares.dat", "wb+", arq2); // Limpa arquivo
            criarBaseDesordenadaCelular(arq2, tam_cel, tam_cel);
            baseIntercaladaCel = 0;
            printf("Base de Celulares criada com sucesso!\n");
            break;

        case 3: // Imprimir
            printf("\nImprimir: (1) Computadores ou (2) Celulares? ");
            scanf("%i", &n);
            if (n == 1) {
                if (baseIntercaladaPc) imprimirBaseComputador(out);
                else imprimirBaseComputador(arq);
            } else if (n == 2) {
                if (baseIntercaladaCel) imprimirBaseCelular(out2);
                else imprimirBaseCelular(arq2);
            } else printf("Opcao invalida.\n");
            break;

        case 4: // Busca Sequencial
            printf("\nBuscar em: (1) Computadores ou (2) Celulares? ");
            scanf("%i", &n);
            printf("Digite o codigo: ");
            scanf("%i", &chave);
            if (n == 1) {
                comp = buscaSequencial_pc(chave, arq);
                if (comp) { imprime_computador(comp); free(comp); }
                else printf("PC nao encontrado na base desordenada.\n");
            } else if (n == 2) {
                cel = buscaSequencial_cel(chave, arq2);
                if (cel) { imprime_celular(cel); free(cel); }
                else printf("Celular nao encontrado na base desordenada.\n");
            }
            break;

        case 5: // Ordenar
            printf("\nOrdenar: (1) Computadores ou (2) Celulares? ");
            scanf("%i", &n);
            if (n == 1) {
                int particoes = (tam_pc / 10) > 0 ? (tam_pc / 10) : 1;
                freopen("pcintercalado.dat", "wb+", out);
                int tam_class = classificacao_interna_pc(arq, particoes);
                intercalacao_basica_pc(out, tam_class);
                baseIntercaladaPc = 1;
                freopen("pcintercalado.dat", "rb+", out); // Reabre para leitura
                printf("Computadores ordenados com sucesso.\n");
            } else if (n == 2) {
                int particoes = (tam_cel / 10) > 0 ? (tam_cel / 10) : 1;
                freopen("celintercalado.dat", "wb+", out2);
                int tam_class = classificacao_interna_cel(arq2, particoes);
                intercalacao_basica_cel(out2, tam_class);
                baseIntercaladaCel = 1;
                freopen("celintercalado.dat", "rb+", out2); // Reabre para leitura
                printf("Celulares ordenados com sucesso.\n");
            }
            break;

        case 6: // Busca Binaria
            printf("\nBusca Binaria em: (1) Computadores ou (2) Celulares? ");
            scanf("%i", &n);
            if (n == 1) {
                if (!baseIntercaladaPc) { printf("ERRO: Ordene a base de PCs primeiro (Opcao 5).\n"); break; }
                printf("Codigo do PC: ");
                scanf("%i", &chave);
                comp = busca_binaria_pc(chave, out, 0, qtdRegistros_pc(out) - 1);
                if (comp) { imprime_computador(comp); free(comp); }
                else printf("PC nao encontrado.\n");
            } else if (n == 2) {
                if (!baseIntercaladaCel) { printf("ERRO: Ordene a base de Celulares primeiro (Opcao 5).\n"); break; }
                printf("Codigo do Celular: ");
                scanf("%i", &chave);
                cel = busca_binaria_cel(chave, out2, 0, qtdRegistros_cel(out2) - 1);
                if (cel) { imprime_celular(cel); free(cel); }
                else printf("Celular nao encontrado.\n");
            }
            break;

        case 7: // Criar Hash
            printf("\nCriar Tabela Hash para: (1) Computadores ou (2) Celulares? ");
            scanf("%i", &n);
            if (n == 1) {
                freopen("tabela_hash_pc.dat", "wb+", arq_Hash_pc);
                tabela_hash_pc(arq_Hash_pc, arq);
            } else if (n == 2) {
                freopen("tabela_hash_cel.dat", "wb+", arq_Hash_cel);
                tabela_hash_cel(arq_Hash_cel, arq2);
            }
            break;

        case 8: // Busca Hash
            printf("\nBusca Hash em: (1) Computadores ou (2) Celulares? ");
            scanf("%i", &n);
            printf("Codigo: ");
            scanf("%i", &chave);
            if (n == 1) {
                comp = busca_pc(arq_Hash_pc, chave);
                if (comp) { imprime_computador(comp); free(comp); }
                else printf("PC nao encontrado na Hash.\n");
            } else if (n == 2) {
                cel = busca_cel(arq_Hash_cel, chave);
                if (cel) { imprime_celular(cel); free(cel); }
                else printf("Celular nao encontrado na Hash.\n");
            }
            break;

        case 9: // Inserir
            printf("\nInserir em: (1) Computadores ou (2) Celulares? ");
            scanf("%i", &n);
            char marca[50], modelo[50];
            double preco;
            printf("Codigo: "); scanf("%i", &chave);
            printf("Marca: "); scanf("%s", marca);
            printf("Modelo: "); scanf("%s", modelo);
            printf("Preco: "); scanf("%lf", &preco);

            if (n == 1) {
                comp = computador(chave, marca, modelo, preco);
                insere_pc(arq_Hash_pc, comp);
                printf("Computador inserido.\n");
                free(comp);
            } else if (n == 2) {
                cel = celular(chave, marca, modelo, preco);
                insere_cel(arq_Hash_cel, cel);
                printf("Celular inserido.\n");
                free(cel);
            }
            break;

        case 10: // Excluir
            printf("\nExcluir de: (1) Computadores ou (2) Celulares? ");
            scanf("%i", &n);
            printf("Codigo: "); scanf("%i", &chave);
            if (n == 1) exclui_pc(arq_Hash_pc, chave);
            else if (n == 2) exclui_cel(arq_Hash_cel, chave);
            break;

        case 11: // Compra (Integração)
            printf("\n--- SISTEMA DE COMPRAS ---\n");
            printf("Digite o codigo do Computador desejado: ");
            scanf("%i", &chave);
            comp = buscaSequencial_pc(chave, arq);

            printf("Digite o codigo do Celular desejado: ");
            scanf("%i", &chave);
            cel = buscaSequencial_cel(chave, arq2);

            if (comp != NULL && cel != NULL) {
                comprarProduto(comp, cel);
                free(comp);
                free(cel);
            } else {
                printf("\nErro na compra: Um ou ambos os produtos nao foram encontrados.\n");
                if(comp) free(comp);
                if(cel) free(cel);
            }
            break;

        default:
            printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    // Fechamento seguro de arquivos
    if(arq) fclose(arq);
    if(arq2) fclose(arq2);
    if(out) fclose(out);
    if(out2) fclose(out2);
    if(arq_Hash_pc) fclose(arq_Hash_pc);
    if(arq_Hash_cel) fclose(arq_Hash_cel);

    return 0;
}
