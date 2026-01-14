#include "celulares.h"
#include "computadores.h"
#include "compra.h"
#include <string.h>
#include <stdlib.h>

void comprarProduto(TComputador *comp, TCelular *cel)
{
    double valorTotal = comp->precopc + cel->precoc;

    printf("\n>>> COMPROVANTE DE VENDA <<<\n");
    printf("------------------------------\n");
    printf("1. COMPUTADOR\n");
    printf("   Modelo: %s %s\n", comp->marcapc, comp->modelopc);
    printf("   Valor:  R$ %.2f\n", comp->precopc);
    printf("------------------------------\n");
    printf("2. CELULAR\n");
    printf("   Modelo: %s %s\n", cel->marcac, cel->modeloc);
    printf("   Valor:  R$ %.2f\n", cel->precoc);
    printf("------------------------------\n");
    printf("TOTAL:     R$ %.2f\n", valorTotal);
    printf("==============================\n");
}
