#ifndef IPC_H 
#define IPC_H 

typedef enum {
	Inst_Exit, 
	Inst_Frame
} Inst_Type;

struct Inst {
	Inst_Type type;	
};


struct Inst_Set {
	struct Inst *set; 
	int i; 
	int cap;	
};


void *new_socket_session(void *arg); 

#endif
