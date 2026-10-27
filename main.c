#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tarefas.h"

typedef struct TarefaTemp {
    char* texto;
    unsigned prioridade: 2;
    struct tm dataPrazo;
} TarefaTemp;

Stack s = {NULL, NULL};
TarefaTemp tt;
Tarefa* tx;

int main() {
    srand(time(NULL));
    int op, opId, intTemp, dia, mes, ano;
    char buffer[256];

    puts("====================");
    puts("Bem-vindo ao SGT!");
    puts("====================");
    puts("");

    floadt(&s);

    while (1)
    {
        puts("====================");
        puts("Tarefas:");
        puts("");
        
        if (s.top != NULL) {
            Tarefa *tx_print = s.top;
            while (tx_print != NULL) 
            {
                readt(*tx_print);
                tx_print = tx_print->nextptr; 
            }
        } else {
            puts("Lista de tarefas vazia!");
        }
        
        puts("====================");
        puts("Escolha uma opção:");
        puts("");
        puts("1: Criar nova tarefa");
        puts("2: Marcar uma tarefa como concluída");
        puts("3: Modificar uma tarefa");
        puts("4: Alterar posição de uma tarefa");
        puts("5: Deletar uma tarefa");
        puts("6: Busca Avançada (Título ou Prioridade)"); 
        puts("0: Salvar e sair");
        puts("====================");
        
        printf("Opção: ");
        if (scanf("%d", &op) != 1) {
            while (getchar() != '\n'); 
            continue;
        }
        getchar(); 

        switch (op)
        {
        case 0:
            fsavet(&s);
            printf("Salvando e saindo do programa...\n");
            return 0; 
            
        case 1:
            printf("Digite o Texto: ");
            scanf(" %255[^\n]", buffer);
            tt.texto = strdup(buffer);
            
            while (1) 
            {
                puts("");
                printf("Qual a prioridade? [0 - Baixa][1 - Média][2 - Alta]: ");
                scanf("%d", &intTemp);
                getchar();
                if (intTemp < 0 || intTemp > 2) {
                    puts("Opção inválida! Escolha uma opção entre 0-2.");
                } else {
                    tt.prioridade = intTemp;
                    break;
                }
            }
            while (1)
            {
                puts("");
                printf("Digite o prazo da tarefa (DD/MM/AAAA): ");
                if (scanf("%d/%d/%d", &dia, &mes, &ano) == 3) {
                    tt.dataPrazo.tm_mday = dia;
                    tt.dataPrazo.tm_mon  = mes - 1;    
                    tt.dataPrazo.tm_year = ano - 1900; 
                    
                    tt.dataPrazo.tm_hour = 23;
                    tt.dataPrazo.tm_min  = 59;
                    tt.dataPrazo.tm_sec  = 59;
                    getchar();
                    break;
                } else {
                    while (getchar() != '\n'); 
                    puts("Data inválida! Escreva no formato DD/MM/AAAA.");
                }
            }
            puts("");
            createt(&s, tt.texto, tt.prioridade, tt.dataPrazo);
            puts("Tarefa criada com sucesso!");
            break;
            
        case 2:
            while (1)
            {
                puts("");
                printf("Digite o ID da tarefa que deseja marcar como concluída (0 para cancelar): ");
                scanf("%d", &opId);
                getchar();
                
                if (opId == 0) break;
                
                tx = buscarTarefaPorId(&s, opId);
                if (tx == NULL) {
                    puts("Não foi encontrada nenhuma tarefa com esse ID.");
                } else {
                    time_t t_atual = time(NULL);
                    struct tm dtcnc = *localtime(&t_atual);
                    updatet(tx, NULL, 1, 0, &dtcnc, NULL, 1, 0); 
                    puts("Tarefa concluída!");
                    break;
                }
            }
            break; 
            
        case 3:
            puts("");
            printf("Digite o ID da tarefa que deseja modificar (0 para cancelar): ");
            scanf("%d", &opId);
            getchar();
            
            if (opId == 0) break;
            
            tx = buscarTarefaPorId(&s, opId);
            if (tx == NULL) {
                puts("Não foi encontrada nenhuma tarefa com esse ID.");
            } else {
                char resp;
                
                // 1. Modificar Texto
                printf("Deseja alterar o texto? [S/N]: ");
                scanf(" %c", &resp);
                getchar();
                if (resp == 'S' || resp == 's') {
                    printf("Digite o novo Texto: ");
                    scanf(" %255[^\n]", buffer);
                    tx->texto = strdup(buffer); 
                }

                // 2. Modificar Prioridade
                printf("Deseja alterar a prioridade? [S/N]: ");
                scanf(" %c", &resp);
                getchar();
                if (resp == 'S' || resp == 's') {
                    printf("Nova prioridade [0 - Baixa][1 - Média][2 - Alta]: ");
                    scanf("%d", &intTemp);
                    getchar();
                    if (intTemp >= 0 && intTemp <= 2) {
                        updatet(tx, NULL, 0, intTemp, NULL, NULL, 0, 1);
                    } else {
                        puts("Prioridade inválida. Mantendo a antiga.");
                    }
                }
                
                // 3. Modificar Prazo
                printf("Deseja alterar o prazo? [S/N]: ");
                scanf(" %c", &resp);
                getchar();
                if (resp == 'S' || resp == 's') {
                    while (1) {
                        printf("Digite o novo prazo (DD/MM/AAAA): ");
                        if (scanf("%d/%d/%d", &dia, &mes, &ano) == 3) {
                            struct tm novo_prazo = {0};
                            novo_prazo.tm_mday = dia;
                            novo_prazo.tm_mon  = mes - 1;    
                            novo_prazo.tm_year = ano - 1900; 
                            novo_prazo.tm_hour = 23;
                            novo_prazo.tm_min  = 59;
                            novo_prazo.tm_sec  = 59;
                            getchar();
                            
                            // Atualizando apenas o prazo
                            updatet(tx, NULL, 0, 0, NULL, &novo_prazo, 0, 0);
                            break;
                        } else {
                            while (getchar() != '\n'); 
                            puts("Data inválida! Escreva no formato DD/MM/AAAA.");
                        }
                    }
                }

                puts("Tarefa modificada!");
            }
            break;

        case 4:
            puts("");
            printf("Digite o ID da tarefa que deseja mover (0 para cancelar): ");
            scanf("%d", &opId);
            getchar();
            
            if (opId == 0) break;

            tx = buscarTarefaPorId(&s, opId);
            if (tx == NULL) {
                puts("Não foi encontrada nenhuma tarefa com esse ID.");
            } else {
                printf("Quantas posições deseja mover? (+ para o topo, - para baixo): ");
                int offset;
                scanf("%d", &offset);
                getchar();
                
                movet(&s, tx, offset);
                puts("Posição alterada com sucesso!");
            }
            break;
            
        case 5:
            puts("");
            printf("Digite o ID da tarefa que deseja deletar (0 para cancelar): ");
            scanf("%d", &opId);
            getchar();
            
            if (opId == 0) break;
            
            tx = buscarTarefaPorId(&s, opId);
            if (tx == NULL) {
                puts("Não foi encontrada nenhuma tarefa com esse ID.");
            } else {
                deletet(&s, tx);
                puts("Tarefa deletada com sucesso!");
            }
            break;

        case 6:
            puts("");
            puts("1 - Buscar por título/palavra-chave");
            puts("2 - Buscar por prioridade");
            printf("Escolha o método de busca: ");
            scanf("%d", &intTemp);
            getchar();

            puts("\n=== Resultados da Busca ===");
            if (intTemp == 1) {
                printf("Digite o termo que deseja buscar: ");
                scanf(" %255[^\n]", buffer);
                buscarPorTexto(&s, buffer);
            } else if (intTemp == 2) {
                printf("Qual prioridade deseja filtrar? [0-Baixa][1-Média][2-Alta]: ");
                int filtroPri;
                scanf("%d", &filtroPri);
                if (filtroPri >= 0 && filtroPri <= 2) {
                    buscarPorPrioridade(&s, filtroPri);
                } else {
                    puts("Prioridade inválida.");
                }
            } else {
                puts("Opção inválida.");
            }
            puts("===========================");
            break;

        default:
            puts("Opção inválida! Escolha uma opção entre 0-6.");
            break;
        }
        
        puts("\nPressione Enter para continuar...");
        getchar();
    }
    
    return 0;
}