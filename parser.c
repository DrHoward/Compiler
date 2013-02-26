/*	Jacob Howard and Alexander Darino 
	10/15/09
	Assignment 3: C/0 Parser-Code Generator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_CODE_LENGTH 500

#define CONSUME(token_name, error_code) if (consume(token_name, error_code)) return EXIT_FAILURE
#define LOOKUP(name) if (lookUp(name)) return EXIT_FAILURE

typedef enum { 
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqlsym, neqsym,
lessym, leqsym, gtrsym, geqsym, lparentsym,
rparentsym, commasym, semicolonsym, periodsym, becomessym,
beginsym, endsym, ifsym, thensym, whilesym,
dosym, callsym, constsym, intsym, procsym,
writesym, readsym, begincomment, endcomment, arraysym,
lbracksym, rbracksym, elsesym
} token_type;

typedef enum {
	LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, WRT, RIN, STX, STY, LDX, SSY
} op_code;

struct symbol  
{ 
	int kind; 		// const = 1, var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val; 		// number (ASCII value) 
	int level; 		// L level
	int addr; 		// M address
};

struct LL
{
	struct LL *next;
	struct symbol *symtab;	// Pointer to an array
	int size;
};

struct Instruction {
	unsigned int op, l, m;
};

char *ttos(int token);
char *error(int code);
int insertConst(char *name, int val);
int insertVar(char *name, int l, int m);
int lookUp(char *name);
int emit(int op, int l, int m);
void getToken();
int program (void);
int block (void);
int statement(void);
int condition (void);
int expression (void);
int term (void);
int factor (void);
int isRelational (void);
void perr(int code);
char *op_name(int op);
int consume(int expected_token, int errcode);
struct LL *parentSymTab(int L);

/*	Compiler State Variables	*/
char *token;
int i_token;
char **input;
int input_size = 0, token_index = 0;
int L = 0;	// Current lexicographical level
struct symbol main_symtab[MAX_SYMBOL_TABLE_SIZE];
struct LL *symtabLL;
#define symbol_table (symtabLL->symtab)


int deltaL, lookup_index; // used by lookUp function
int line_number = 0, entry_line = 0, main_entry, main_entry_line;

struct proc_list
{
	int code_index, jump_index;
	struct proc_list *next;
	int level;
};

struct proc_list *pl = NULL;


struct Instruction code[MAX_CODE_LENGTH];
int code_size = 0;

int parser()	
{
	FILE *fin;
	FILE *fout = fopen("vma.out", "w");
	int i;
	
	// Initialize the Symbol Table LL
	symtabLL = malloc(sizeof(struct LL));
	symtabLL->next = NULL;
	symtabLL->symtab = main_symtab;
	symtabLL->size = 0;

	fin = fopen("lexemelist.txt", "r");
	while (!feof(fin))
	{
		input_size++;
		input = (char **)realloc(input, input_size * sizeof(char *));
		input[token_index] = (char *)malloc(12 * sizeof(char));
		fscanf(fin, "%s", input[token_index]);
		token_index++;
	}
	input_size--;
	token_index = 0;
	fclose(fin);
	
	// Print contents of token file (ie. _input_ array)
	puts("\nContents of Token File:");
	for (i = 0; i < input_size; i++)
	{
		printf("%s ", input[i]);
	}
	puts("\n\nSymbolic Representation of Token File:");
	
	// Print symbolic representation of token file (ie. _input_ array)
	for (i = 0; i < input_size; i++)
	{
		i_token = atoi(input[i]);
		printf("%s", ttos(i_token));
		if (i_token == identsym)
		{
			printf(".%s", input[++i]);
		}
		else if (i_token == numbersym)
		{
			printf(".%d", atoi(input[++i]));
		}
		printf(" ");
	}
	i_token = 0;
	puts("\n");
	
	// Generate code
	puts("Generating Code:");
	if (program() != 0)
		return EXIT_FAILURE;
	
	puts("\nNo errors, program is syntactically correct\n");
	
	for (i = 0; i < code_size; i++)
	{
		fprintf(fout, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
	}
	fclose(fout);
	return 0;
}

char *ttos(int token)
{
	static char name[][20] = {	"NULL",
					"IDENTIFIER",
					"NUMBER",
					"+",
					"-",
					"*",
					"/",
					"ODD",
					"=",
					"<>",
					"<",
					"<=",
					">",
					">=",
					"(",
					")",
					",",
					";",
					".",
					":=",
					"{",
					"}",
					"IF",
					"THEN",
					"WHILE",
					"DO",
					"CALL",
					"CONST",
					"INT",
					"PROCEDURE",
					"WRITE",
					"READ",
					"/*",
					"*/",
					"ARRAY",
					"[",
					"]", 
                    "ELSE"};
	if (token == -1)
		return "TOO MANY OCCURANCES";
	if (token == 0)
		return "COULD NOT IDENTIFY";
		
	return name[token - 1];
}

int insertConst(char *name, int val)
{
	if (symtabLL->size >= MAX_SYMBOL_TABLE_SIZE)
		return 1;
	symbol_table[symtabLL->size].kind = 1;
	strcpy(symbol_table[symtabLL->size].name, name);
	symbol_table[symtabLL->size].val = val;
	symtabLL->size++;
	return 0;
}

int insertVar(char *name, int l, int m)
{
	if (symtabLL->size >= MAX_SYMBOL_TABLE_SIZE)
		return 1;
	symbol_table[symtabLL->size].kind = 2;
	strcpy(symbol_table[symtabLL->size].name, name);
	symbol_table[symtabLL->size].level = l;
	symbol_table[symtabLL->size].addr = m + 3;
	symtabLL->size++;
	return 0;
}

int insertProc(char *name, int l)
{
	if (symtabLL->size >= MAX_SYMBOL_TABLE_SIZE)
		return 1;
	symbol_table[symtabLL->size].kind = 3;
	strcpy(symbol_table[symtabLL->size].name, name);
	symbol_table[symtabLL->size].level = l;
	symbol_table[symtabLL->size].addr = line_number;
	symtabLL->size++;
	return 0;
}

struct LL *parentSymTab(int L)
{
	int i;
	struct LL *original = symtabLL, *r_val;
	for (i = 0; i < L; i++)
	{
		if (symtabLL == NULL)
		{
			symtabLL = original;
			return NULL;
		}
		symtabLL = symtabLL->next;
	}
	r_val = symtabLL;
	symtabLL = original;
	return r_val;
}
int lookUp(char *name)
{
	int i;
	struct LL *original = symtabLL;
	deltaL = 0;
	while (symtabLL != NULL)
	{
		for (i = 0; i < symtabLL->size; i++)
		{
			if(strcmp(symbol_table[i].name, name) == 0)
			{
				symtabLL = original;
				lookup_index = i;
				return 0;
			}
		}
		symtabLL = symtabLL->next;
		deltaL++;
	}
	symtabLL = original;
	perr(10);                                          //////////////////////////////////////////////////////////////
    return -1;
}

int emit(int op, int l, int m)
{
	if (code_size >= MAX_CODE_LENGTH)
		return 1;
	code[code_size].op = op;
	code[code_size].l = l;
	code[code_size].m = m;
	code_size++;
	printf("%s %d %d\n", translate(op), l, m);
	line_number++;
	return 0;
}

void getToken(){
	if (token_index == input_size)
	{
		token = NULL;
		return;
	}
	
	token = input[token_index++];
	i_token = atoi(token);
}

int program (void)
{
	getToken();
	emit(INC, 0, 3);	// INC 0, 3
	pl = NULL;
	if (block() != 0)
		return 1;
	code[main_entry_line].m = main_entry;
	CONSUME(periodsym, 8);
	// emit(JMP, 0, MAX_CODE_LENGTH);
	emit (OPR, 0, 0);	// Return in main
	return 0; 
}
int block (void)
{
	int sym_index;
	int offset = 0;
	char name[12];
	int const_val;
	int old_proc_start_pos;
	
	if (i_token == constsym)   // const-declaration
	{
		do{
			getToken();
			
			CONSUME(identsym, 3);

			strcpy(name, token);
			getToken();
			if (i_token != eqlsym)
			{
				if (i_token == becomessym)
					{perr(0);}
				else
					perr(26);                                              ////////////////////////////////////////////////////
				return 1;
			}
			getToken();
			
			CONSUME(numbersym, 1);
			
			insertConst(name, i_token);
			getToken();
		}
		while (i_token == commasym);
		
		CONSUME(semicolonsym, 16); 
		
	}
	if (i_token == intsym)     // var-declaration
	{
		do
		{
			getToken();
			
			CONSUME(identsym, 3);
	
			insertVar(token, L, offset++);
			
			emit (INC, 0, 1);
			getToken();
		} while (i_token == commasym);
		CONSUME(semicolonsym, 16);  
	}
	if (i_token == arraysym)       // array-dectlaration
	{
		do
		{
			getToken();	// token = identsym
			
			CONSUME(identsym, 3); // token = name of identifier
			
			insertVar(token, L, offset);
			getToken();
			
			CONSUME(lbracksym, 29); // token = numbersym
			CONSUME(numbersym, 30); // token = array_size
			emit (INC, 0, i_token);	// INC 0, array_size
			offset += i_token;
			
			getToken(); // token = rbracksym
			CONSUME(rbracksym, 28);
		} while (i_token == commasym);
		CONSUME(semicolonsym, 16);
	}
	if (L == 0)
	{
		main_entry_line = code_size;
		emit(JMP, 0, 0);	// Placeholder
	}
	
	if (pl != NULL)
	{
		pl->code_index = line_number;
		emit(JMP, 0, 0);	// Placeholder
	}
	while (i_token == procsym)     // procedure-declaration
	{
		int i_pos;
		getToken();
		
		CONSUME(identsym, 3);
		insertProc(token, L+1);
		getToken();
		
		if (pl == NULL)
		{
			pl = malloc(sizeof(struct proc_list));
			pl->next = NULL;
			pl->level = 1;
		}
		else
		{
			struct proc_list *temp = malloc(sizeof(struct proc_list));
			temp->next = pl;
			temp->level = pl->level + 1;
			pl = temp;
		}
		
		CONSUME(semicolonsym, 16);                                        
		 
		L++;
		
		struct symbol new_symtab[MAX_SYMBOL_TABLE_SIZE];
		struct LL *temp = malloc(sizeof(struct LL));
		temp->next = symtabLL;
		symtabLL = temp;
		symtabLL->symtab = new_symtab;
		symtabLL->size = 0;
		
		entry_line = line_number;
		
		emit (INC, 0, 3);
		block();

		L--;
		symtabLL = symtabLL->next;
		
		CONSUME(semicolonsym, 16);	
	}				
	
	main_entry = line_number;

	if (pl != NULL && pl->level == L)
	{
		code[pl->code_index].m = line_number;
		pl = pl->next;
	}
	if (statement() != 0)
		return 1; 
	emit (OPR, 0, 0);
	return 0;
}

int statement(void)
{
	#define sym_index lookup_index
	int code_index;
	int STO_L, STO_M;

	if (i_token == identsym)
	{
		getToken();
		LOOKUP(token);
		STO_L = deltaL;
		STO_M = lookup_index;
		getToken();
		if (i_token == lbracksym)	// We're looking at an array
		{
			getToken();
			expression();
			emit(STY, 0, 0);
			
			CONSUME(rbracksym, 28);
			
			
			CONSUME(becomessym, 12);
			
			if (expression() != 0)
				return 1;
			emit(SSY, STO_L, parentSymTab(STO_L)->symtab[STO_M].addr);
		}
		else
		{
			if (sym_index < 0)
			{
				perr(10);
				return 1;
			}
			//if (parentSymTab(deltaL)->symtab[lookup_index].kind == 1)
			if (parentSymTab(STO_L)->symtab[STO_M].kind == 1)
			{
				perr(11);
				return 1;
			}

			
			CONSUME(becomessym, 12);
			
			if (expression() != 0)
				return 1;

			emit(STO, STO_L, parentSymTab(STO_L)->symtab[STO_M].addr);	// STO 0, M
		}
	}
	else if (i_token == callsym){
        getToken();
        
        CONSUME(identsym, 27); // token = name of identifier
        
        LOOKUP(token);
        
        if (parentSymTab(deltaL)->symtab[lookup_index].kind == 3)
            emit(5 , 0, parentSymTab(deltaL)->symtab[lookup_index].addr);
        else 
        {
            perr(14);
            return 1;
        }
        getToken();
    }
	else if (i_token == beginsym)
	{
		getToken();
		if (statement() != 0)
			return 1;
		while (i_token == semicolonsym)
		{
			getToken();
			if (statement() != 0)
				return 1;
		}
		CONSUME(endsym, 16);
	}
	else if (i_token == ifsym)
	{
		getToken();
		if (condition() != 0)
			return 1;

		code_index = code_size;	// Point to the following jump instruction
		emit(JPC, 0, 0);			// Place-holder jump location
		
		CONSUME(thensym, 15);
		
		if (statement() != 0)
			return 1;
		code[code_index].m = code_size; // point to the following instructions
		
		if (i_token == elsesym)
		{
            code[code_index].m++;
            getToken();
            code_index = code_size;
            emit(JPC, 0, 0);
            
            if (statement() != 0)
			    return 1;
			code[code_index].m = code_size;
        }
	}
	else if (i_token == whilesym)
	{
        int condition_index;
        getToken();
        condition_index = code_size;
		if (condition() != 0)
			return 1;
			
		code_index = code_size;
		emit(JPC, 0, 0);
		
		CONSUME(dosym, 17);
		
		if (statement() != 0)
			return 1;
        
		emit(JMP, 0, condition_index);
		code[code_index].m = code_size;		
    }
    else if (i_token == readsym)
    {
		int STO_L, STO_M;
		
		getToken();
        
		CONSUME(identsym, 27);
        LOOKUP(token);
        STO_L = deltaL;
        STO_M = lookup_index;
        getToken();
        
        if (i_token == lbracksym)
        {
			getToken();
			expression();
			emit(STY, 0, 0);
			emit(RIN, 0, 0);
			emit(SSY, STO_L, parentSymTab(STO_L)->symtab[STO_M].addr);
			CONSUME(rbracksym, 28);
		}
		else
		{
			emit(RIN, 0, 0);
			emit(STO, STO_L, parentSymTab(STO_L)->symtab[STO_M].addr);

		}
    }
    else if (i_token == writesym)
    {
		int STO_L, STO_M;
        getToken();
        CONSUME(identsym, 27);
        LOOKUP(token);
        STO_L = deltaL;
        STO_M = lookup_index;
        
        getToken();
        if (i_token == lbracksym)
        {
			getToken();
			expression();
			emit(STX, 0, 0);
			emit(LDX, STO_L, parentSymTab(STO_L)->symtab[STO_M].addr);
			emit(WRT, 0, 0);
			CONSUME(rbracksym, 28);
		}
		else
		{
			emit (LOD, STO_L, parentSymTab(STO_L)->symtab[STO_M].addr);
			emit(WRT, 0, 0);
		}
    }
    return 0;
	#undef sym_index
}

int condition (void)
{
    int operation;
    
    if (i_token == oddsym)
    {
        getToken();
        if (expression() != 0)
		    return 1;
		    
		emit(OPR, 0, 6);        // ODD
    }else
    {
        if (expression() != 0)
            return 1;
        operation = i_token;
        
        if (!isRelational())
        {
            perr(19);
            return 19;
        }
        getToken();
        if (expression() != 0)
            return 1;
            
        switch (operation){
            case eqlsym: 
                emit(OPR, 0, 8); 
                break;
            case neqsym: 
                emit(OPR, 0, 9); 
                break;
            case lessym: 
                emit(OPR, 0, 10); 
                break;
            case leqsym: 
                emit(OPR, 0, 11); 
                break;
            case gtrsym: 
                emit(OPR, 0, 12); 
                break;
            case geqsym: 
                emit(OPR, 0, 13); 
                break;
        }
    }
    return 0;
}
              
int expression (void)
{
	int operation;
    if (i_token == plussym || i_token == minussym)
    {
		operation = i_token;
        getToken();
	}
    if (term() != 0)
        return 1;
    if (operation == minussym)
    	emit (OPR, 0, 1);	// NEG	- toggles sign of value at top of stack
    while (i_token == plussym || i_token == minussym)
    {
		operation = i_token;
        getToken();
        if (term() != 0)
            return 1;
    	if (operation == minussym)
    		emit (2, 0, 3);
    	else
    		emit (2, 0, 2);
    }
    return 0;
}

int term (void)
{
    int operation;
    
    if (factor() != 0)
        return 1;
    while (i_token == multsym || i_token == slashsym)
    {
        operation = i_token;
        getToken();
        
        if (factor() != 0)
            return 1;
            
        emit(2, 0, operation == multsym ? 4 : 5);
    }
    return 0;
}
              
int factor (void)
{
	int STO_L, STO_M;
    #define sym_index lookup_index
    if (i_token == identsym){
        getToken();     // identifier

        LOOKUP(token);
        
        STO_L = deltaL;
		STO_M = lookup_index;
		if (parentSymTab(STO_L)->symtab[STO_M].kind == 3)
        {
            perr(20);
            return 1;
        }
        if (sym_index < 0)
		{
			perr(10);
			return 1;
		}
		
		getToken();
		
		if (i_token == lbracksym)
		{
			getToken();
			expression();
			emit(11, 0, 0);	// STX
			CONSUME(rbracksym, 28); 
			emit(LDX, STO_L, parentSymTab(STO_L)->symtab[STO_M].addr);
		}
		else
		{
	        if (parentSymTab(deltaL)->symtab[lookup_index].kind == 2)
	        {
				emit (3, STO_L, parentSymTab(STO_L)->symtab[STO_M].addr);
	        	//emit(3, STO_L, STO_M + 3);
	        	//emit(3, deltaL, parentSymTab(deltaL)->symtab[lookup_index].addr);
			}
	        else
	        {
				emit (3, STO_L, parentSymTab(STO_L)->symtab[STO_M].val);
	        	//emit(1, 0, parentSymTab(STO_L)->symtab[STO_M].val);
	        	//emit(1, 0, parentSymTab(deltaL)->symtab[lookup_index].val);
			}
		}
        
        
    }else if (i_token == numbersym){
        getToken();

        emit(1, 0, i_token);
        
        getToken();        
    }else if (i_token == lparentsym){
        getToken();
        if (expression() != 0)
            return 1;
        if (i_token != rparentsym)
        {
            perr(21);
            return 21;
        }
        getToken();
    }
	else{
        perr(23);
        return 23;
    }
    return 0;
    #undef sym_index
}

int isRelational (void)
{
    return (i_token == eqlsym || i_token == neqsym || i_token == lessym || i_token == leqsym || i_token == gtrsym || i_token == geqsym);
}

void perr(int code)
{
	printf("%s\n\n", error(code));
}

int consume(int expected_token, int errcode)
{
	if (i_token == expected_token)
	{
		getToken();
		return 0;
	}
    
	perr(errcode);
	return EXIT_FAILURE;
}

char *error(int code)
{
	static char msg[][256] ={
		"Use = instead of :=.",													// 0
		"= must be followed by a number.",										// 1
		"Identifier must be followed by :=.",									// DO NOT USE
		"const, var, procedure must be followed by identifier.",				// 3
		"Semicolon or comma missing.",											// DO NOT USE
		"Incorrect symbol after procedure declaration.",						// DO NOT USE
		"Statement expected.",													// DO NOT USE
		"Incorrect symbol after statement part in block.",						// DO NOT USE
		"Period expected.",														// 8
		"Semicolon between statements missing.",								// DO NOT USE
		"Undeclared identifier.",												// 10
		"Assignment to constant or procedure is not allowed.",					// 11
		"Assignment operator expected.",										// 12
		"call must be followed by an identifier.",								// DO NOT USE
		"Call of a constant or variable is meaningless.",						// 14
		"then expected.",														// 15
		"Semicolon or } expected.",												// 16
		"do expected.",															// 17
		"Incorrect symbol following statement.",								// DO NOT USE
		"Relational operator expected.",										// 19
		"Expression must not contain a procedure identifier.",					// 20
		"Right parenthesis missing.",											// 21
		"The preceding factor cannot begin with this symbol.",					// DO NOT USE
		"An expression cannot begin with this symbol.",							// 23
		"This number is too large.",											// 24
		"int expected.",														// DO NOT USE
		"= expected.",															// 26
		"identifier expected.",													// 27
		"] expected.",															// 28
		"[ expected.",															// 29
		"Number expected."};													// 30
		

	return msg[code];
}
