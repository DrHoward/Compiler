/*
COP 3402
10/01/2009
PROJECT PART 2
Jacob Howard
Ryan McConnell
Diego Velasquez
*/

#include <stdio.h>
#include <string.h>
#define MAX_IDENTIFIER 1000 /* maximum depth of block nesting */
#define MAX_DIGITS 5 /* maximum number of chars for idents */
#define MAXSTRINGS 1000
#define NUM_RESERVEDWORD 15         /* number of reserved words */
#define INT_MAX   99999       /* maximum integer value */
#define MAX_IDEN_FOR_BLOCK 1000
#define MAX_NUM_FOR_BLOCK 1000

#define  strmax   256         /* maximum length of strings */
enum token_type {
     nulsym = 1, identsym, numbersym, plussym, minussym,
     multsym,  slashsym, oddsym, eqlsym, neqsym, lessym, leqsym,
     gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
     periodsym, becomessym, beginsym, endsym, ifsym, thensym,
     whilesym, dosym, callsym, constsym, intsym, procsym, writesym, readsym,  
     begincomment, endcomment, arraysym, lbracksym, rbracksym, elsesym
};
int n=0;
char lexemelist[MAXSTRINGS];
char token[MAXSTRINGS];
char identifiers[MAX_IDENTIFIER];
char digits[MAX_DIGITS];
int numbers[MAX_NUM_FOR_BLOCK];
char reservedwords[NUM_RESERVEDWORD];
char words[MAX_IDEN_FOR_BLOCK][MAX_IDENTIFIER];
char symbols[20];
void getTokens(char token[MAXSTRINGS],FILE *outfile);
void comments (char token[MAXSTRINGS]);
int check_words(char identifiers[MAX_IDENTIFIER],FILE *outfile);
int check_symbols(char symbols[20],FILE *outfile);
void no_spaces(char identifiers[MAX_IDENTIFIER]);
void print(char list[MAX_IDENTIFIER],FILE *outfile);
int scanner (int argc, char **argv){

    int i=0,size=0;
    char temp[MAXSTRINGS];
    FILE * infile;
    FILE * outfile;
    FILE * outtemp;
    //open the file
    infile  = fopen(argv[1],  "r" );
    outtemp=  fopen("newSource.txt", "w");
    outfile = fopen( "Lexeme.txt" , "w");
   
    //run until the last line to read the first file the original one.
     while(!feof(infile))
    {
             token[i] = fgetc(infile);
             i++;
    }
    
    //I get the size and then I call my function of coments
    size=i;
    printf("Source Program:\n\n");
    for (i=0; i<size-1; i++){
        printf("%c",token[i]);
        fprintf(outfile,"%c",token[i]);
        }
    comments(token);
   //Them I just create a new file with out the coments, I just Changed the coments for ' ' blank
   // because we will have to check later for that anyways.
    for (i=0; i<size-1; i++)
     fprintf(outtemp,"%c",token[i]);
      fclose(outtemp);
      //Now we have a new input file with out comments :)
      infile=  fopen("newSource.txt", "r");
      i=0;
      // From here we should start now to check the other stuff
       while(!feof(infile))
       {
             token[i] = fgetc(infile);
          //system("pause");
       // token = getTokens(token,tokens,location);
        i++;

        }
      //This is to check of is really working
      size=i;
      printf("\nLexeme Table:\nlexeme\t\ttoken type\n\n");
      fprintf(outfile,"\nLexeme Table:\nlexeme\t\ttoken type\n\n");
      getTokens(token,outfile);

    fclose(infile);
    fclose(outfile);
//this will delete the extra text files
 remove( "newSource.txt" ); //just the same code with out comments
 remove( "Lexeme.txt" ); //just the output in a txt file
// system("pause");
 return 0;
}

//this function will check what is each token
void getTokens(char token[MAXSTRINGS],FILE *outfile){
       int i=0, j=0,Lexical=0,k=0,m=0;
       enum token_type Whattoken;
       while(token[i] != '\0'){
                      Lexical=0;
           if(isalpha(token[i])){///Is a leter
              j=0;
               strcpy(identifiers, "           ");
              while (1){//it will stop until find a sybol or space
				 if(isspace(token[i])){
                    i--;
                    break;
                    }
                  if (token[i]== '+' || token[i]=='-' || token[i]=='*' || token[i]=='/' || token[i]=='(' || token[i]==')'
                       || token[i]=='=' || token[i]==',' || token[i]=='.'|| token[i]=='<' || token[i]=='>' || token[i]==';' || token[i]==':'
                       || token[i]=='[' || token[i]==']'){
                    i--;
                    break;
                    }
                 if(isdigit(token[i])){
                     identifiers[j]=token[i];
                     j++;
                     i++;
                     }
                 else{
                     identifiers[j]=token[i];
                     j++;
                     i++;
                     }
                    }
                    if(j > MAX_IDENTIFIER-1)
                    {
                        printf("Identifier too long\n");
                    }
                    else{
                    no_spaces(identifiers);
                    printf("%s\t\t",identifiers);///print the word
                    fprintf(outfile,"%s\t\t",identifiers);
                    Lexical = check_words(identifiers,outfile);
					}
                 }
            else if(isdigit(token[i])){///Is a number
                j=0;
                strcpy(digits, "     ");
                Whattoken=numbersym ;
                while(1){///it will stop until find a symbol or space
                   if(isspace(token[i])){
                       i--;
                       break;
                       }
                   else if (token[i]== '+' || token[i]=='-' || token[i]=='*' || token[i]=='/' || token[i]=='(' || token[i]==')'
                        || token[i]=='=' || token[i]==',' || token[i]=='.'|| token[i]=='<' || token[i]=='>' || token[i]==';' || token[i]==':'
                        || token[i]=='[' || token[i]==']'){
                       i--;
                       break;
                       }
                   else if(isdigit(token[i])){
                       digits[j]=token[i];
                     j++;
                     i++;
                     }
                     else{
					 	  printf("Variable does not start with letter\n");
					 	  while(1){
	 	     	             if(isspace(token[i])){
						     i--;
                             break;
                            }
                            else
                              i++;
                          }
                          Whattoken=0;
                          break;
						  }

                 }
                 if(Whattoken!=0){
                     no_spaces(digits);
                     int number =atoi (digits);///covert a sring to a number
                     if(number > INT_MAX)
                       {
                           printf("Number too big\n");
                       }
					   else if(j > MAX_DIGITS)
                       {
                           printf("Number too long\n");
                       }
					  else{
                         numbers[m]=number;
                         m++;
                         printf("%d\t\t%d\n", number,Whattoken);
                         fprintf(outfile,"%d\t\t%d\n", number,Whattoken);
                         Lexical = Whattoken;
						 }
				}

             }
           else if(token[i]=='{'|| token[i]=='}' ){//I am about to start here the reserved words.
               j=0;
               reservedwords[j]=token[i];
               if(token[i]=='{')
                   Whattoken =beginsym;
               else
                  Whattoken =endsym;
               printf("%s\t\t%d\n",reservedwords,Whattoken);
               fprintf(outfile,"%s\t\t%d\n",reservedwords,Whattoken);
               Lexical=Whattoken;
              }
             //check for the symbols and the symbol ":="
            else if (token[i]== '+' || token[i]=='-' || token[i]=='*' || token[i]=='/' || token[i]=='(' || token[i]==')'
                || token[i]=='=' || token[i]==',' || token[i]=='.'|| token[i]=='<' || token[i]=='>' || token[i]==';' ||
				token[i]==':' || (token[i]== '!' && token[i+1]== '='|| token[i]=='[' || token[i]==']')){

                    j=0;
                    strcpy(symbols, "  ");
                   if(token[i] == ':' && token[i+1]== '='){
                      symbols[j] = token[i];
                      symbols[j+1] = token[i+1];
                      i++;
                      Whattoken = becomessym ;
                      printf("%s\t\t%d\n",symbols,Whattoken);
                      fprintf(outfile,"%s\t\t%d\n",symbols,Whattoken);
                      Lexical = Whattoken;
                   }
                   else if(token[i] == '>' && token[i+1]== '='){
                      symbols[j] = token[i];
                      symbols[j+1] = token[i+1];
                      i++;
                      Whattoken = geqsym;
                      printf("%s\t\t%d\n",symbols,Whattoken);
                      fprintf(outfile,"%s\t\t%d\n",symbols,Whattoken);
                      Lexical = Whattoken;
                   }
                   else if(token[i] == '<' && token[i+1]== '='){
                      symbols[j] = token[i];
                      symbols[j+1] = token[i+1];
                      i++;
                      Whattoken = leqsym ;
                      printf("%s\t\t%d\n",symbols,Whattoken);
                      fprintf(outfile,"%s\t\t%d\n",symbols,Whattoken);
                      Lexical = Whattoken;
                   }
                   else if(token[i]== '!' && token[i+1]== '='){
                      symbols[j] = token[i];
                      symbols[j+1] = token[i+1];
                      i++;
                      Whattoken = neqsym;
                      printf("%s\t\t%d\n",symbols,Whattoken);
                      fprintf(outfile,"%s\t\t%d\n",symbols,Whattoken);
                      Lexical = Whattoken;
                   }
                   else{
                      symbols[j]=token[i];
                      printf("%s\t\t",symbols);
                      fprintf(outfile,"%s\t\t",symbols);
                      Lexical=check_symbols(symbols,outfile);
                      }


             }
              else{
              	   if(isspace(token[i]));
              	   else if(token[i+1]=='\0');
					 else {
             	        printf("Invalid symbols\n");
						}

						}
             if(Lexical>0){
                lexemelist[k]=Lexical;
                k++;
                }
             i++;
           }
           printf("\nLexeme List:\n");
           fprintf(outfile,"\nLexeme List:\n");
           print(lexemelist,outfile);
           printf("\n");
           fprintf(outfile,"\n");
       }

int check_words(char identifiers[MAX_IDENTIFIER],FILE *outfile){
    // check for    call, const, do, if, odd, procedure, then, int, while
    enum token_type Whattoken;
    int i=0;
    if ( strcmp(identifiers,"null") == 0)
        Whattoken = nulsym;
    else if ( strcmp(identifiers,"call") == 0)
        Whattoken = callsym;
    else if ( strcmp(identifiers,"const") == 0)
        Whattoken = constsym ;
    else if ( strcmp(identifiers,"do") == 0)
      Whattoken = dosym;
    else if ( strcmp(identifiers,"if") == 0)
       Whattoken = ifsym;
   else if ( strcmp(identifiers,"odd") == 0)
      Whattoken = oddsym ;
    else if ( strcmp(identifiers,"procedure") == 0)
       Whattoken = procsym ;
    else if ( strcmp(identifiers,"then") == 0)
       Whattoken = thensym;
    else if ( strcmp(identifiers,"int") == 0)
      Whattoken = intsym ;
    else if ( strcmp(identifiers,"while") == 0)
      Whattoken = whilesym ;
    else if ( strcmp(identifiers,"write") == 0)
      Whattoken = writesym;
    else if( strcmp(identifiers,"read") == 0)
       Whattoken=readsym;
   else if( strcmp(identifiers,"array") == 0)
       Whattoken=arraysym;
   else if( strcmp(identifiers,"else") == 0)
       Whattoken = elsesym;
    else{
      Whattoken = identsym ;
      strcpy(words[n],identifiers);
      n++;
      }
    printf("%d\n",Whattoken);
    fprintf(outfile,"%d\n",Whattoken);
    return Whattoken;

    }
int check_symbols(char symbols[20],FILE *outfile)
{
     //check for Special Symbols: '+', '-', '*', '/', '(', ')', '=', ',' , '.', ' <', '>',  ';' , ':' .
     //here I put all the symbols that are in the structure.
     enum token_type Whattoken;
     if ( strcmp(symbols,"+ ") == 0)
        Whattoken = plussym;
    else if ( strcmp(symbols,"- ") == 0)
        Whattoken = minussym;
    else if ( strcmp(symbols,"* ") == 0)
       Whattoken = multsym;
    else if ( strcmp(symbols,"/ ") == 0)
      Whattoken = slashsym;
    else if ( strcmp(symbols,"( ") == 0)
       Whattoken = lparentsym ;
    else if ( strcmp(symbols,") ") == 0)
       Whattoken = rparentsym ;
    else if ( strcmp(symbols,"= ") == 0)
      Whattoken = eqlsym;
    else if ( strcmp(symbols,", ") == 0)
      Whattoken = commasym;
    else if ( strcmp(symbols,". ") == 0)
      Whattoken = periodsym ;
    else if ( strcmp(symbols,"< ") == 0)
      Whattoken = lessym;
    else if ( strcmp(symbols,"> ") == 0)
      Whattoken = gtrsym;
    else if ( strcmp(symbols,"; ") == 0)
      Whattoken = semicolonsym ;
    else if ( strcmp(symbols,"[ ") == 0)
      Whattoken =  lbracksym;
    else if ( strcmp(symbols,"] ") == 0)
      Whattoken = rbracksym ;
    else
      Whattoken = becomessym ;

    printf("%d\n",Whattoken);
    fprintf(outfile,"%d\n",Whattoken);
    return Whattoken;
}
void no_spaces(char identifiers[MAX_IDENTIFIER]){
     char *p1 = identifiers;
     char *p2 = identifiers;
     p1 = identifiers;
     while(*p1 != 0) {
       if(isspace(*p1)) {
          ++p1;
       }
       else
         *p2++ = *p1++;
       }
       *p2 = 0;

}

void comments (char token[MAXSTRINGS]){
     int i=0,j=0;
     //Run to the program until is over
     while(token[i] != '\0'){
     //Check if the tokens '/' and '*' are together
     if (token[i]=='/' && token [i+1] == '*'){
        //Then I replace them for spaces
        token[i]=' ';
        token[i+1]= ' ';
        i=i+2;
        //run Until we found the oposite and then I replace them by blanks
       // while(token[i] != '*' && token [i+1] != '/'){
          while(1){
             if (token[i] == '*' && token [i+1] == '/')
                break;
             else{
             token[i]=' ';
               i++;
               }
                            }
             //finally I replace the last tokens '*' and '/' for blanks
             token[i]=' ';
             token[i+1]=' ';
             }
             i++;
        }
     }

void print(char list[MAX_IDENTIFIER],FILE *outfile){
     int i,k=0,m=0;
    FILE *temp1;
    //open the file
    temp1 = fopen("lexemelist.txt","w"); 
     for (i=0; i<strlen(list); i++){
          printf("%d ",list[i]);
          fprintf(outfile,"%d ",list[i]);
          fprintf(temp1,"%d ",list[i]);
          if(list[i] == 2){
             printf("%s ",words[m]);
             fprintf(outfile,"%s ",words[m]);
             fprintf(temp1,"%s ",words[m]);
             m++;
             }
          if(list[i] == 3){
             printf("%d ",numbers[k]);
             fprintf(outfile,"%d ",numbers[k]);
             fprintf(temp1,"%d ",numbers[k]);
             k++;
             }
          }
          fclose(temp1);

}
