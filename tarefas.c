#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tarefas.h"

// Cores ANSI para o terminal
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/* === Utils === */

// Exibe a data no formato correto
void printData(struct tm data) 
{
    if (data.tm_year < 0) {
        printf("Não concluída");
        return;
    }

    printf("%02d/%02d/%d %02d:%02d:%02d",
        data.tm_mday,          
        data.tm_mon + 1,       
        data.tm_year + 1900,   
        data.tm_hour,          
        data.tm_min,           
        data.tm_sec            
    );
}

Tarefa* buscarTarefaPorId(Stack* s, int idx)
{
    Tarefa* tx = s->top;
    while (tx != NULL)
    {
        if (tx->id == idx) return tx;
        tx = tx->nextptr;
    }
    return NULL;
}

// Busca Avançada por Título
void buscarPorTexto(Stack* s, const char* termo) 
{
    Tarefa* tx = s->top;
    int encontrou = 0;
    while (tx != NULL) {
        // strstr procura uma substring (termo) dentro da string principal (tx->texto)
        if (strstr(tx->texto, termo) != NULL) {
            readt(*tx);
            encontrou = 1;
        }
        tx = tx->nextptr;
    }
    if (!encontrou) puts("Nenhuma tarefa encontrada com esse texto.");
}

// Busca Avançada por Prioridade
void buscarPorPrioridade(Stack* s, unsigned pri) 
{
    Tarefa* tx = s->top;
    int encontrou = 0;
    while (tx != NULL) {
        if (tx->prioridade == pri) {
            readt(*tx);
            encontrou = 1;
        }
        tx = tx->nextptr;
    }
    if (!encontrou) puts("Nenhuma tarefa encontrada com essa prioridade.");
}

void tarefaParaString(Tarefa t, char* buffer, size_t tamanho) 
{
    snprintf(buffer, tamanho, "%d;%s;%u;%u;%02d/%02d/%d %02d:%02d:%02d;%02d/%02d/%d %02d:%02d:%02d;%02d/%02d/%d %02d:%02d:%02d\n",
        t.id, t.texto, t.concluida, t.prioridade,
        t.dataCriacao.tm_mday, t.dataCriacao.tm_mon + 1, t.dataCriacao.tm_year + 1900,
        t.dataCriacao.tm_hour, t.dataCriacao.tm_min, t.dataCriacao.tm_sec,
        t.dataConclusao.tm_mday, t.dataConclusao.tm_mon + 1, t.dataConclusao.tm_year + 1900,
        t.dataConclusao.tm_hour, t.dataConclusao.tm_min, t.dataConclusao.tm_sec,
        t.dataPrazo.tm_mday, t.dataPrazo.tm_mon + 1, t.dataPrazo.tm_year + 1900,
        t.dataPrazo.tm_hour, t.dataPrazo.tm_min, t.dataPrazo.tm_sec
    );
}

int gerarIdUnico(Stack* s)
{
    int id;
    do {
        id = (rand() % 10000) + 1;
    } while (buscarTarefaPorId(s, id) != NULL);
    return id; 
}

/* === CRUD === */

Tarefa* createt(Stack* s, char* txt, unsigned pri, struct tm dpra) 
{
    Tarefa* t = (Tarefa*) malloc(sizeof(Tarefa));
    if (t == NULL) return NULL;

    t->id = gerarIdUnico(s);
    t->texto = txt;
    t->concluida = 0;
    t->prioridade = pri;
    
    time_t t_atual = time(NULL);
    t->dataCriacao = *localtime(&t_atual);
    t->dataConclusao.tm_year = -1;
    t->dataPrazo = dpra;

    t->prevptr = NULL;
    t->nextptr = s->top;
    if (s->top != NULL) s->top->prevptr = t;
    if (s->top == NULL) s->bot = t;
    s->top = t;

    return t;
}

void readt(Tarefa t) 
{
    // RFE01 - Destaca verde se concluído
    if (t.concluida) {
        printf(ANSI_COLOR_GREEN "[X] " ANSI_COLOR_RESET);
    } else {
        printf("[ ] ");
    }
    printf("%s", t.texto);
    printf("\n    ID: %d", t.id);

    // RFE01 - Destacando com as Cores
    switch (t.prioridade)
    {
    case 0:
        printf("\n    Prioridade: " ANSI_COLOR_GREEN "Baixa" ANSI_COLOR_RESET);
        break;
    case 1:
        printf("\n    Prioridade: " ANSI_COLOR_YELLOW "Média" ANSI_COLOR_RESET);
        break;
    case 2:
        printf("\n    Prioridade: " ANSI_COLOR_RED "Alta" ANSI_COLOR_RESET);
        break;
    default:
        break;
    }
    printf("\n    Prazo: "); printData(t.dataPrazo);
    printf("\n    Data de Criação: "); printData(t.dataCriacao);
    printf("\n    Data de Conclusão: "); printData(t.dataConclusao); // RFE02 
    printf("\n\n");
}

void updatet(Tarefa* t, char* txt, unsigned cnc, unsigned pri, struct tm* dcnc, struct tm* dpra, int flagcnc, int flagpri)
{
    if (txt != NULL) t->texto = txt;
    if (flagcnc == 1) t->concluida = cnc;
    if (flagpri == 1) t->prioridade = pri;

    if (dcnc != NULL) t->dataConclusao = *dcnc;
    if (dpra != NULL) t->dataPrazo = *dpra;
}

int deletet(Stack* s, Tarefa* t) 
{
    if (s->top == NULL || s->bot == NULL || t == NULL) return -1; 
    int v = t->id;

    if ( (s->top == s->bot) && (s->top == t) && (s->bot == t) ) { 
        s->top = NULL;
        s->bot = NULL;
    } else if (s->top == t) { 
        s->top = t->nextptr;
        s->top->prevptr = NULL;
    } else if (s->bot == t) { 
        t->prevptr->nextptr = NULL;
        s->bot = t->prevptr;
    } else { 
        t->prevptr->nextptr = t->nextptr;
        t->nextptr->prevptr = t->prevptr;
    }
    free(t);

    return v;
}

void movet(Stack* s, Tarefa* t, int offset) 
{
    if (s == NULL || t == NULL || offset == 0 || s->top == s->bot) return;

    Tarefa* target = t;
    int steps = offset > 0 ? offset : -offset;

    if (offset > 0) { 
        while (steps > 0 && target->prevptr != NULL) {
            target = target->prevptr;
            steps--;
        }
    } else { 
        while (steps > 0 && target->nextptr != NULL) {
            target = target->nextptr;
            steps--;
        }
    }

    if (target == t) return;

    if (t->prevptr != NULL) t->prevptr->nextptr = t->nextptr;
    if (t->nextptr != NULL) t->nextptr->prevptr = t->prevptr;

    if (s->top == t) s->top = t->nextptr;
    if (s->bot == t) s->bot = t->prevptr;

    if (offset > 0) {
        t->nextptr = target;
        t->prevptr = target->prevptr;

        if (target->prevptr != NULL) {
            target->prevptr->nextptr = t;
        } else {
            s->top = t; 
        }
        target->prevptr = t;

    } else {
        t->prevptr = target;
        t->nextptr = target->nextptr;

        if (target->nextptr != NULL) {
            target->nextptr->prevptr = t;
        } else {
            s->bot = t; 
        }
        target->nextptr = t;
    }
}

/* === Funções de Persistência de Dados === */
void fsavet(Stack *s)
{
    FILE *f = fopen(ARQUIVO, "w");
    if (f == NULL) return;

    Tarefa *tx = s->top;
    while (tx != NULL)
    {
        char buffer[512];
        tarefaParaString(*tx, buffer, sizeof(buffer));
        fputs(buffer, f);
        tx = tx->nextptr;
    }
    fclose(f);
}

void floadt(Stack *s)
{
    FILE *f = fopen(ARQUIVO, "r");
    if (f == NULL) return;

    char linha[1024];
    int id;
    char texto_buffer[256];
    unsigned concluida, prioridade;

    int cr_ano, cr_mes, cr_dia, cr_hora, cr_min, cr_sec;
    int co_ano, co_mes, co_dia, co_hora, co_min, co_sec;
    int pr_ano, pr_mes, pr_dia, pr_hora, pr_min, pr_sec;

    while (fgets(linha, sizeof(linha), f) != NULL)
    {
        int itensLidos = sscanf(linha, "%d;%255[^;];%u;%u;%d/%d/%d %d:%d:%d;%d/%d/%d %d:%d:%d;%d/%d/%d %d:%d:%d",
            &id, texto_buffer, &concluida, &prioridade,
            &cr_dia, &cr_mes, &cr_ano, &cr_hora, &cr_min, &cr_sec,
            &co_dia, &co_mes, &co_ano, &co_hora, &co_min, &co_sec,
            &pr_dia, &pr_mes, &pr_ano, &pr_hora, &pr_min, &pr_sec
        );

        if (itensLidos >= 4) {
            struct tm dcri = {0}, dcnc = {0}, dpra = {0};

            dcri.tm_sec = cr_sec; dcri.tm_min = cr_min; dcri.tm_hour = cr_hora;
            dcri.tm_mday = cr_dia; dcri.tm_mon = cr_mes - 1; dcri.tm_year = cr_ano - 1900;

            dcnc.tm_sec = co_sec; dcnc.tm_min = co_min; dcnc.tm_hour = co_hora;
            dcnc.tm_mday = co_dia; dcnc.tm_mon = co_mes - 1; dcnc.tm_year = co_ano - 1900;

            dpra.tm_sec = pr_sec; dpra.tm_min = pr_min; dpra.tm_hour = pr_hora;
            dpra.tm_mday = pr_dia; dpra.tm_mon = pr_mes - 1; dpra.tm_year = pr_ano - 1900;

            char* texto_alocado = strdup(texto_buffer);
            Tarefa* nova_t = createt(s, texto_alocado, prioridade, dpra);
            
            // Restaura as datas originais e o status
            if (nova_t) {
                nova_t->id = id;
                nova_t->concluida = concluida;
                nova_t->dataCriacao = dcri;
                nova_t->dataConclusao = dcnc;
            }
        }
    }
    fclose(f);
}