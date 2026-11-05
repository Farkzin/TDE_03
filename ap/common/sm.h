// Define a estrutura e as macros utilizadas pelas máquinas de estados

#ifndef SM_H
#define SM_H

struct StateMachine;

typedef void (*pStateFunc)(struct StateMachine *);

typedef struct StateMachine{
    pStateFunc ptr;
    unsigned char entry;
} StateMachine;

#define STATE(name)       void name(StateMachine *_sm_)  // Declara uma função de estado

#define NEXT_STATE(name)  _sm_->ptr = (pStateFunc)name   // Define o próximo estado da máquina

#define INIT(sm,name)     {sm.ptr = (pStateFunc)name; sm.entry = 1;} // Inicializa uma máquina de estados

#define EXEC(sm)          {pStateFunc temp = sm.ptr; sm.ptr(&sm); sm.entry = (temp != sm.ptr);} // Executa o estado atual

#define ENTRY             (_sm_->entry)                   // Indica a entrada em um novo estado

#endif
