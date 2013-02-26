/*
JORGE RIVAS
COP 3402
ASSIGNMENT 1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

struct Instruction
{
   int op;   // opcode 
   int  l;   // L            
   int  m;   // M
};

struct CPU
{
    int sp;
    int bp;
    int pc;
    int indexx;
    int indexy;
    struct Instruction iRegister;
};

int base(int l,int basepointer);
void initialize(struct CPU *cpu, struct Instruction *iRegister);
void fetch(struct CPU *cpu, struct Instruction *code);
void execute(struct CPU *cpu, int *stack);
char* translate(int op);
int printpipe();
int stack[MAX_STACK_HEIGHT];
int userIn;
void printStack(struct CPU *cpu);
int vm(void)
{
    struct CPU cpu;
    struct Instruction code[MAX_CODE_LENGTH];
    int counter=0;
    int basepointer=0;
    //char* filename="a.txt";
    //char* filename;
    stack[1] =0;
    stack[2] =0;
    stack[3] =0;
    int i,j;

    /*filename = malloc(256 * sizeof(char));
    
    printf("Enter the name of the file to read\n");
    scanf("%s", filename);*/
    FILE *fin;
    fin = fopen ("vma.out", "r");
    
    printf("Line\tOP\tL\tM\n");
    
    while(!feof(fin)) //counts the number of instructions in the file
    {
        fscanf(fin, "%d %d %d ", &code[counter].op, &code[counter].l, &code[counter].m);
        printf("%d\t", counter);
        printf("%s", translate(code[counter].op));
        printf("\t");
        printf("%d\t%d\n", code[counter].l, code[counter].m);
        counter++;
    }
        
    printf("\t\t\t\tpc\tbp\tsp\tstack\n");
    initialize(&cpu, &cpu.iRegister);
    printf("Initial Values  \t\t%d\t%d\t%d\n", cpu.pc, cpu.bp, cpu.sp);
    
    //while(cpu.bp!=0)
    while (cpu.pc < counter)
    {
        printf("%d\t", cpu.pc);
        printf("%s\t", translate(code[cpu.pc].op));
        printf("%d\t%d", code[cpu.pc].l, code[cpu.pc].m);
        fetch(&cpu, code);
        
        // Halt if issuing return statement in main
        if (cpu.iRegister.op == 2 && cpu.iRegister.m == 0 && cpu.bp == 1)
        {
			cpu.bp = cpu.sp = cpu.pc = 0;
			printStack(&cpu);
			break;
		}
     
        //execute(&cpu, &stack[cpu.sp]);
        execute(&cpu, stack);
        //printf("\t%d\t%d\t%d\n", cpu.pc, cpu.bp, cpu.sp);
       
        printStack(&cpu);
        /*
        for(j=0; j<=cpu.sp; j++)
        {
            //if(code[cpu.pc].op==5)
              //  printf("| ");
            printf("%d", stack[j]);            
        }
        */
        printf("\n");
    }
        
    fclose(fin);
    return 0;
}

void initialize(struct CPU *cpu, struct Instruction *iRegister)
{
    cpu->sp=0;
    cpu->bp=1;
    cpu->pc=0;
    iRegister->l=0;
    iRegister->m=0;
    iRegister->op=0;
    
}

char* translate(int op)
{
    switch(op)
    {
        case 1: return "LIT";
            break;
        case 2: return "OPR";
            break;
        case 3: return "LOD";
            break;
        case 4: return "STO";
            break;
        case 5: return "CAL";
            break;
        case 6: return "INC";
            break;
        case 7: return "JMP";
            break;
        case 8: return "JPC";
            break;
        case 9: return "WRT";
            break;
        case 10: return "RIN";
            break;                          
        case 11: return "STX";
            break;
        case 12: return "STY";
            break;      
        case 13: return "LDX";
            break;        
        case 14: return "SSY";
            break;                 
    }
}

//performs fetch cycle function of reading the code into the instruction register and then incrementing the pc by 1
void fetch(struct CPU *cpu, struct Instruction *code)
{
    cpu->iRegister=code[cpu->pc];
    cpu->pc++;
}

//performs the execute cycle function by performing the operation indicated by the opcode.
void execute(struct CPU *cpu, int *stack)
{
    if(cpu->iRegister.op==1)//LIT
    {
        int j;
        cpu->sp++;;
        stack[cpu->sp]=cpu->iRegister.m;
    }
    
    if(cpu->iRegister.op==2)//OPR
    {
        switch(cpu->iRegister.m)
        {
            case 0://RET
                cpu->sp=cpu->bp-1;
                cpu->pc=stack[cpu->sp+3];
                cpu->bp=stack[cpu->sp+2];
                break;
            case 1://NEG
                stack[cpu->sp]=stack[cpu->sp]*(-1);
                break;
            case 2://ADD
                cpu->sp=cpu->sp-1;
                stack[cpu->sp]=stack[cpu->sp]+stack[cpu->sp+1];
                break;
            case 3://SUB
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]-stack[cpu->sp+1];
                break;
            case 4://MUL
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]*stack[cpu->sp+1];
                break;
            case 5://DIV
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]/stack[cpu->sp+1];
                break;
            case 6://ODD
                stack[cpu->sp]=stack[cpu->sp]%2;
                break;
            case 7://MOD
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]%stack[cpu->sp+1];
                break;
            case 8://EQL
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]==stack[cpu->sp+1];
                break;
            case 9://NEQ
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]!=stack[cpu->sp+1];
                break;
           //The new four instructions..............
            case 10://LSS
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]<stack[cpu->sp+1];
                break;
            case 11://LEQ
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]<=stack[cpu->sp+1];
                break;
            case 12://GTR
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]>stack[cpu->sp+1];
                break;
            case 13://GEQ
                cpu->sp--;
                stack[cpu->sp]=stack[cpu->sp]>=stack[cpu->sp+1];
                break;
        }
    }
    
    if(cpu->iRegister.op==3)//LOD
    {
        cpu->sp++;
        stack[cpu->sp]=stack[base(cpu->iRegister.l, cpu->bp) + cpu->iRegister.m];
    }
    
    if(cpu->iRegister.op==4)//STO
    {
        int j;
        stack[base(cpu->iRegister.l, cpu->bp) + cpu->iRegister.m]=stack[cpu->sp];
        cpu->sp--;
    }
    
    if(cpu->iRegister.op==5)//CAL
    {
        //since this doesn't increase the stack pointer how do you print out when cal is performed?
        stack[cpu->sp+1]=base(cpu->iRegister.l, cpu->bp);
        stack[cpu->sp+2]=cpu->bp;
        stack[cpu->sp+3]=cpu->pc;
        cpu->bp=cpu->sp+1;
        cpu->pc=cpu->iRegister.m;   
    }
    
    if(cpu->iRegister.op==6)//INC
    {
        cpu->sp=cpu->sp+cpu->iRegister.m;   
    }
    
    if(cpu->iRegister.op==7)//JMP
    {
        cpu->pc=cpu->iRegister.m;
    }
    
    if(cpu->iRegister.op==8)//JPC
    {
        if(stack[cpu->sp]==0)
           cpu->pc=cpu->iRegister.m;
        cpu->sp--;
        
    }
    
    if(cpu->iRegister.op==9)//WRT
    {
		printf("\nOutput: %d\n\t\t\t", stack[cpu->sp]);
        cpu->sp = cpu->sp - 1;   
    }
    
    if(cpu->iRegister.op==10)//RIN
    {
        //cpu->sp = cpu->sp + 1;
        //stack[cpu->sp] = userIn;
        
        cpu->sp = cpu->sp + 1;
        printf("\nInput: ");
        scanf("%d", stack + cpu->sp);
        printf("\t\t\t");
    }
    //this 4 instructions were addded to the VM, why 4 because we will use one for store and the other one for load, it is more organize
    //and is working.
    if(cpu->iRegister.op==11)//STX
    {
         cpu->indexx = stack[cpu->sp];
         cpu->sp--;
    }
    if(cpu->iRegister.op==12)//STY
    {
         cpu->indexy = stack[cpu->sp];
         cpu->sp--;
    }
    if(cpu->iRegister.op==13)//LDX
    {
     cpu->sp++;
     stack[cpu->sp]=stack[base(cpu->iRegister.l, cpu->bp)+cpu->iRegister.m+cpu->indexx];
     
    }
    if(cpu->iRegister.op==14)//SSY
    {
      stack[base(cpu->iRegister.l, cpu->bp)+cpu->iRegister.m+cpu->indexy] = stack[cpu->sp];
      cpu->sp--;
    }
    
}

int base(int l,int basepointer) // l stand for L in the instruction format
{
  int b1; //find base L levels down
  b1 = basepointer; 
  while (l > 0 && b1 != 1)
  {
    do
	{
		b1 = stack[b1];
	}
	while (b1 != 1 && stack[b1] == stack[b1 + 1]);
	
    l--;
  }
  return b1;
}
void printStack(struct CPU *cpu)
{
    int i, j, tempBP;
    printf("\t%d\t%d\t%d", cpu->pc, cpu->bp, cpu->sp);
    printf("\t");
    if(cpu->sp == 0)
    {
        printf("%d ", stack[0]);
    }

    for(i = 1; i < cpu->sp + 1; i++)
    {
        tempBP = cpu->bp;

        while(tempBP != 1)
        {
            if(i == tempBP)
            {
                printf("| ");
                tempBP = 1;
            }
            else
            {
                tempBP = stack[tempBP + 1];
            }
        }

        printf("%d ", stack[i]);
    }
    if(cpu->iRegister.op== 5)
        {    
        printf("| ");
        for(j = i; j < i + 3; j++)
        {
            printf("%d ", stack[j]);
        }
    }

    printf("\n");
  //  system("pause");

}
