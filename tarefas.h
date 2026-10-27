#include <time.h>

#define ARQUIVO "data.txt"

// Estruturas
typedef struct Tarefa {
    int id;
    char* texto;
    unsigned concluida: 1;
    unsigned prioridade: 2;
    struct tm dataCriacao;
    struct tm dataConclusao;
    struct tm dataPrazo;
    struct Tarefa* nextptr;
    struct Tarefa* prevptr;
} Tarefa;

typedef struct Stack {
    Tarefa* top;
    Tarefa* bot;
} Stack;

// Funções Utils
void printData(struct tm data);
Tarefa* buscarTarefaPorId(Stack* s, int idx);
void tarefaParaString(Tarefa t, char* buffer, size_t tamanho);
int gerarIdUnico(Stack* s);

// Funções de busca
void buscarPorTexto(Stack* s, const char* termo);
void buscarPorPrioridade(Stack* s, unsigned pri);

// Funções CRUD
Tarefa* createt(Stack* s, char* txt, unsigned pri, struct tm dpra);
void readt(Tarefa t);
void updatet(Tarefa* t, char* txt, unsigned cnc, unsigned pri, struct tm* dcnc, struct tm* dpra, int flagcnc, int flagpri);
int deletet(Stack* s, Tarefa* t);
void movet(Stack* s, Tarefa* t, int offset);

// Funções de Persistência de Dados
void fsavet(Stack *s);
void floadt(Stack *s);