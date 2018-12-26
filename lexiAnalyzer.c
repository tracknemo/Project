//Lexical Analyzer: implents a lexical analyzer for the PL/0 programming language
//	program reads source program, identifies errors, prints
//		the program source the source lexeme table, and a list of lexemes

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

//Declare your constants
#define NUM_RESERVED_WORDS 15
#define MAX_INTEGER_VAL 32767
#define MAX_CHAR_COUNT 11
#define MAX_BLOCK_DEPTH 5
#define MAX_DIGIT_LEN 5
#define MAX_STR_LEN 256

//Global array of reserved word names
char* res_words[] = {"null", "begin", "call", "const", "do", "else", "end","if",
				"odd", "procedure", "read", "then", "var", "while", "write"};

//Global array of internal representation of reserved words
char* internal_word[] = {"nulsym", "beginsym", "callsym", "constsym", "dosym", "elsesym", "endsym",
					"ifsym", "oddsym", "procsym", "readsym", "thensym", "varsym", "whilesym", "writesym"};

//Special Symbols array					
char special_symbols[] = {'+', '-', '*', '/', '(', ')', '=', ',','.','<','>',';',':'};
					
					
//Tokens and values
typedef enum{ nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6, 
			slashsym = 7, oddsym = 8,  eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, 
			gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, 
			semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, 
			ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28, 
			varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsesym = 33} token_values;


//Symbol table struct
typedef struct
{
		int type; 		//1= id 2 = digit 4=res
		int token_type; //null = 1 , id =2, number =3 and etc internal vals
		char name[12]; //name up to 11 chars as indicated in rubrcic
		int val; 		//ASCII value
		int level;		//L level
		int address;	//M address
		int token_length; //length of the token
		
}namerecord_t;

//array for symbol table
namerecord_t symbol_table[500]; //Max number of tokens
int num_tokens = 0;

//Error values
int invalid_start =0;
int invalid_char =0;
int global_long = 0;
int global_digit_long =0;


//Regarding input from the file
char char_indv; //bc we are reading in from the file character by character
char potential_token[12]; //concatonation of char_indv's until terminating symbol found
int potential_token_digit[6]; //concatonation of char_indv's until terminating symbol found


//Function Calls
void printInput(FILE *input);
void analyze_alpha(char* tok, int len);
void analyze_digit(int number, int number_len);
void print_error();
void invalid_symbol();
void print_lex_table();
void print_lex_list();


int main(int argc, char* argv[])
{
	//Open our input file
	FILE *input = fopen(argv[1],"r");
    if(input==NULL)
    {
		printf("Invalid filename");
        return 0;
    }
	
	//Begin printing the input
	printInput(input);
	printf("\n");
	
	//Print your Lexeme table
	print_lex_table();
	
	//Print your Lexeme list
	print_lex_list();
	
	//print errors
	print_error();
	
	return 0;
}

//Function input: function takes in the input file name
//function output: function returns nothing BUT it does print the 
//				input file and relays the info from this file to 
//				other functions within the code
void printInput(FILE *input)
{
	//Header for the output
	printf("\nSource Program: \n");

	char_indv = fgetc(input);
	printf("%c", char_indv);

	while( char_indv != EOF)
	{
		//Remove (in this case ignore) white space & Nl & etc commands
		if(char_indv == ' ' || char_indv == '\t' || char_indv == '\n' 
		|| char_indv == '\v' || char_indv == '\f' || char_indv =='\r')
		{		
		//We don't want an infinite loop so scan in the next variable	
		char_indv = fgetc(input);
		printf("%c",char_indv);
		
		//Continue as if it never happened
		continue;
		}
		
		//THE FOLOWING CODE IS STRRUCTURED FROM THE LECTURE 5 NOTES- page 28
		//If the character scanned is a letter..
		if(isalpha(char_indv))
		{
			//Start fresh: clear the global string so we can put all potential token chars
			//		into a string
			memset(potential_token,0,sizeof potential_token);
			int word_length = 0;
			int too_long = 0;
			
			//Put the char we are curretnly on into the array
			potential_token[word_length] = char_indv;
			word_length++;
			char_indv = fgetc(input);
			printf("%c",char_indv);
			
			while(isalpha(char_indv) || isdigit(char_indv) )
			{
				//If the word length is greater than we anticipated
				//	just continue scanning until we have reached the end of the string
				if(word_length>= 11)
				{
				
					while(isalpha(char_indv) || isdigit(char_indv))
					{
						char_indv = fgetc(input);
						printf("%c",char_indv);
					}
					
					too_long = 1;
					break;
				}
				
				potential_token[word_length] = char_indv;
				word_length++;
				char_indv = fgetc(input);
				printf("%c",char_indv);
			}
			
			//If our function variable was not too long
			// ignore it and continue analyzing the next potential token
			if(too_long ==1)
			{
				global_long = 1;
				continue;
			}
			
			//Now that we have a vlid string, let's make it official by putting
			// it into a string of its actual size
			char *final_word = malloc(sizeof(char) * (word_length +1));
			strcpy(final_word,potential_token);
			
			//if string we have is not too long we can "potentially"
			//	tokenize it!!
			if(too_long == 0)
			{
				analyze_alpha(final_word,word_length);
			}
			
			free(final_word);
			continue;
		}
		
		//In the evnt that we have a digit to consider
		if(isdigit(char_indv))
		{
			//Start fresh: clear the global string so we can put all potential token chars
			//		into a string
			memset(potential_token_digit,0,sizeof potential_token_digit);
			int digit_length = 0;
			int too_big = 0;
			int temp_wrong_start =0;
			
			//Put the char (as an integer) we are curretnly on into the array
			potential_token_digit[digit_length] = char_indv-'0';
			digit_length++;
			char_indv = fgetc(input);
			printf("%c",char_indv);
			
			while(isdigit(char_indv) || isalpha(char_indv))
			{
				//If our digit contains a letter, continue scanning its
				//numbers until we reach a terminationg chracter
				if(isalpha(char_indv))
				{
					while(isdigit(char_indv) ||isalpha(char_indv))
					{
						char_indv = fgetc(input);
						printf("%c",char_indv);
						digit_length++;
						if(digit_length>5)
						{
							global_digit_long =1;
							too_big = 1;
						}
						
						continue;
					}
					
					invalid_start = 1;
					temp_wrong_start =1;
					
					break;
				}
				
				//If our digit is greater than length 5, continue scanning its
				//numbers until we reach a terminationg chracter
				if(digit_length>5)
				{
					while(isdigit(char_indv) ||isalpha(char_indv))
					{
						char_indv = fgetc(input);
						printf("%c",char_indv);
					}
					global_digit_long =1;
					too_big = 1;
				}
				
				potential_token_digit[digit_length] = char_indv-'0';
				digit_length++;
				char_indv = fgetc(input);
				printf("%c",char_indv);
			}
			
			//If the digit is invalid, return to the start of the loop
			if(too_big ==1 || temp_wrong_start ==1)
			{
				continue;
			}
		
		//convert array of digits to single integer
		int as_digit = 0;
		int count;
		for(count =0; count<digit_length;count++)
		{
			as_digit = 10 * as_digit + potential_token_digit[count];
		}
		
		//Store this integer value into you struct
		analyze_digit(as_digit,digit_length);
		continue;
		}
		
		//If we have made it this far, then we have encountered a symbol
		if(char_indv == '+' )
		{
		symbol_table[num_tokens].type =3;
		symbol_table[num_tokens].token_type =4;
		symbol_table[num_tokens].name[0] ='+';
		num_tokens++;
		char_indv = fgetc(input);
		printf("%c",char_indv);
		continue;
		}
		
		else if(char_indv == '-' )
		{
		symbol_table[num_tokens].type =3;
		symbol_table[num_tokens].token_type =5;
		symbol_table[num_tokens].name[0] ='-';
		num_tokens++;
		char_indv = fgetc(input);
		printf("%c",char_indv);
		continue;
		}
		
		else if(char_indv == '*' )
		{
		symbol_table[num_tokens].type =3;
		symbol_table[num_tokens].token_type =6;
		symbol_table[num_tokens].name[0] ='*';
		num_tokens++;
		char_indv = fgetc(input);
		printf("%c",char_indv);
		continue;
		}
		
		else if(char_indv == '/' )
		{
			//check to see if we have a comment
			char_indv = fgetc(input);
			printf("%c",char_indv);
			
			if(char_indv =='*')
			{
				int in_comment = 1;
				//comtinue to scan but do not do
				// anything with these values because we are in a 
				//comment block
				
				while(in_comment)
				{
				char_indv = fgetc(input);
				printf("%c",char_indv);
				
				//we do not want an infinite loop
				// so break if the comment was never closed
				if(char_indv ==EOF)
					break;
				
				//we might have reached the end of a comment!!
				if(char_indv == '*')
				{
					char_indv = fgetc(input);
					printf("%c",char_indv);
					
					if(char_indv =='/')
					{
						char_indv = fgetc(input);
						printf("%c",char_indv);
						in_comment = 0;
						break;
					}
					else
						continue;
				}
				
				}
				
			}
			
			else
			{
				symbol_table[num_tokens].type =3;
				symbol_table[num_tokens].token_type =7;
				symbol_table[num_tokens].name[0] ='/';
				num_tokens++;
			}
			continue;
		}
		
		else if(char_indv == '(' )
		{
		symbol_table[num_tokens].type =3;
		symbol_table[num_tokens].token_type =15;
		symbol_table[num_tokens].name[0] ='(';
		num_tokens++;
		char_indv = fgetc(input);
		printf("%c",char_indv);
		continue;
		}
		
		else if(char_indv == ')' )
		{
		symbol_table[num_tokens].type =3;
		symbol_table[num_tokens].token_type =16;
		symbol_table[num_tokens].name[0] =')';
		num_tokens++;
		char_indv = fgetc(input);
		printf("%c",char_indv);
		continue;
		}
		
		else if(char_indv == '=' )
		{
		symbol_table[num_tokens].type =3;
		symbol_table[num_tokens].token_type =9;
		symbol_table[num_tokens].name[0] ='=';
		num_tokens++;
		char_indv = fgetc(input);
		printf("%c",char_indv);
		continue;
		}
		
		else if(char_indv == ',' )
		{
		symbol_table[num_tokens].type =3;
		symbol_table[num_tokens].token_type =17;
		symbol_table[num_tokens].name[0] =',';
		num_tokens++;
		char_indv = fgetc(input);
		printf("%c",char_indv);
		continue;
		}
		
		else if(char_indv == '.' )
		{
		symbol_table[num_tokens].type =3;
		symbol_table[num_tokens].token_type =19;
		symbol_table[num_tokens].name[0] ='.';
		num_tokens++;
		char_indv = fgetc(input);
		printf("%c",char_indv);
		continue;
		}
		
		else if(char_indv == '<' )
		{
			//check to see if it is actuall <=
			char_indv = fgetc(input);
			printf("%c",char_indv);
			
			if(char_indv == '=')
			{
				symbol_table[num_tokens].type =3;
				symbol_table[num_tokens].token_type =12;
				symbol_table[num_tokens].name[0] ='<';
				symbol_table[num_tokens].name[1] ='=';
				num_tokens++;
				char_indv = fgetc(input);
				printf("%c",char_indv);
			}
			
			else
			{
			
			symbol_table[num_tokens].type =3;
			symbol_table[num_tokens].token_type =11;
			symbol_table[num_tokens].name[0] ='<';
			num_tokens++;
			}
			
			continue;
		}
		
		else if(char_indv == '>' )
		{
			//check to see if it is actuall <=
			char_indv = fgetc(input);
			printf("%c",char_indv);
			
			if(char_indv == '=')
			{
				symbol_table[num_tokens].type =3;
				symbol_table[num_tokens].token_type =14;
				symbol_table[num_tokens].name[0] ='>';
				symbol_table[num_tokens].name[1] ='=';
				num_tokens++;
				char_indv = fgetc(input);
				printf("%c",char_indv);
			}
			
			else
			{
			
			symbol_table[num_tokens].type =3;
			symbol_table[num_tokens].token_type =13;
			symbol_table[num_tokens].name[0] ='>';
			num_tokens++;
			}
			
			continue;
		}
		
		else if(char_indv == ';' )
		{
		symbol_table[num_tokens].type =3;
		symbol_table[num_tokens].token_type =18;
		symbol_table[num_tokens].name[0] =';';
		num_tokens++;
		char_indv = fgetc(input);
		printf("%c",char_indv);
		continue;
		}
		
		else if(char_indv == ':' )
		{
			//check to see if it is actuall <=
			char_indv = fgetc(input);
			printf("%c",char_indv);
			
			if(char_indv == '=')
			{
				symbol_table[num_tokens].type =3;
				symbol_table[num_tokens].token_type =20;
				symbol_table[num_tokens].name[0] =':';
				symbol_table[num_tokens].name[1] ='=';
				num_tokens++;
				char_indv = fgetc(input);
				printf("%c",char_indv);
			}
			
			else
			{
				invalid_char = 1;
			}
			
			continue;
		}
		
		else if(char_indv == '!' )
		{
			//check to see if it is actuall <=
			char_indv = fgetc(input);
			printf("%c",char_indv);
			
			if(char_indv == '=')
			{
				symbol_table[num_tokens].type =3;
				symbol_table[num_tokens].token_type =10;
				symbol_table[num_tokens].name[0] ='!';
				symbol_table[num_tokens].name[1] ='=';
				num_tokens++;
				char_indv = fgetc(input);
				printf("%c",char_indv);
			}
			
			else
			{
				invalid_char = 1;
			}
			
			continue;
		}
		
		//if we have made it this far
		//	then the symbol is invalid
		else
		{
			invalid_char = 1;
			char_indv = fgetc(input);
			printf("%c",char_indv);
			continue;
		}
		
	}
	
	return ;
}

//Function input: Nothing
//Function output:Function will print the Lexeme List
void print_error()
{
	printf("\nERRORS FOUND IN PROGRAM:\n");
	if(invalid_start ==1)
		printf("At some point in your program you wrote a variable that does not start with a letter\n");
	
	else if(invalid_char==1)
		printf("At some point in your program you entered an invalid symbol\n");
	
	else if(global_long==1)
		printf("At some point in your program you entered a variable name that was too long\n");
	
	else if(global_digit_long==1)
		printf("At some point in your program you entered a number that was too big\n");
	
	else 
		return;
}

//Function input: Nothing
//Function output:Function will print the Lexeme List
void print_lex_list()
{
	int i;
	printf("\nLexeme List:\n");
	
	for(i=0; i<num_tokens;i++)
	{
		//if the token is an id or a digit print it to the lexeme list
		if(symbol_table[i].type == 1)
		{
			printf("%d %s  ",symbol_table[i].token_type,symbol_table[i].name);
		}
		else if(symbol_table[i].type == 2)
		{
			printf("%d %d  ",symbol_table[i].token_type,symbol_table[i].val);
		}
		
		else
		{
			printf("%d  ",symbol_table[i].token_type);
		}
	}
	printf("\n");
	return;
	
}

//Function input: Nothing
//Function output:Function will print the Lexeme Table
void print_lex_table()
{
	printf("\nLexeme Table:\n");
	printf("lexeme\t\t\ttoken type\n");
	
		int i;
		for(i=0; i<num_tokens; i++)
		{
			if(symbol_table[i].type !=2)
			printf("%s\t\t\t%d\n",symbol_table[i].name,symbol_table[i].token_type);         
			
			else
			printf("%d\t\t\t%d\n",symbol_table[i].val,symbol_table[i].token_type);         

		}
		
		return;
}

//Function input: Function takes in a valid digit and its length
//Function output: Function tokenizes the digit
void analyze_digit(int number, int number_len)
{
	symbol_table[num_tokens].type = 2; //type is 2 for digit
	symbol_table[num_tokens].token_type = 3;
	symbol_table[num_tokens].val = number;
	num_tokens++;
	return;
}


//Function input: Function takes in a valid string and the length of the string
//Function output: function will tokenize it as a reserved word or variable
void  analyze_alpha(char *token,int word_length)
{
	int k;
			
	for(k=0; k<NUM_RESERVED_WORDS;k++)
	{
		if(strcmp(token,res_words[k])==0)
		{
			if(strcmp(token,"null")==0)
			{
				//int p;
				//for(p=0; p<word_length;p++)
				//printf("%c ",token[p]);
			
				//it is a reserved word whose flag is 4
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 1;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"const")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 28;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"var")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 29;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"procedure")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 30;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"call")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 27;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"begin")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 21;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"end")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 22;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"if")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 23;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"then")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 24;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"else")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 33;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"while")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 25;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"do")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 26;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"read")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 32;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			
			else if(strcmp(token,"write")==0)
			{
				symbol_table[num_tokens].type = 4;
				symbol_table[num_tokens].token_type = 31;
				symbol_table[num_tokens].token_length = word_length;
				strcpy(symbol_table[num_tokens].name,token);
				num_tokens++;
			}
			return;
		}
	}
	
	//if we have made it this far, then it means we have found an id
	// flag for id is 1
	symbol_table[num_tokens].type = 1;
	symbol_table[num_tokens].token_type = 2;
	strcpy(symbol_table[num_tokens].name,token);
	symbol_table[num_tokens].token_length = word_length;
	num_tokens++;
	
	return;
}
