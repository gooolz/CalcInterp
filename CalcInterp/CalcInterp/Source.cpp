//Julia Foote
//3310
//CalcInterp

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <map>
#include <ctype.h>
#include <sstream>

// Calc Grammar CFG rules
// <parse> -> <stmt_list> $$
// <stmt_list> -> <stmt> <stmt_list> | ε	
// <stmt> -> id := <expr> | read id | write <expr>
// <expr> -> <term> <term_tail>
// <term_tail> -> <add_op> <term> <term_tail> | ε
// <term> -> <factor> <factor_tail>
// <factor_tail> -> <mul_op> <factor> <factor_tail> | ε
// <factor> -> (<expr>) | id | number
// <add_op> -> + | -
// <mul_op> -> * | /


using namespace std;

map<string, double> symbolTable;

int parse();
void stmtList();
void stmt();
double expr();
double termTail(double inval);
double term();
double factorTail(double inval);
double factor();

enum GTokens {
	EOFSY = 0, ASSIGNSY = 1, LPAREN = 2, RPAREN = 3, ADDOP = 4, MINOP = 5, MULOP = 6, DIVOP = 7, ID = 8, NUMCONST = 9,
	READSY = 10, WRITESY = 11,
};

struct TokStruct {
	GTokens TokS;
	string str;
};

#define GETTOKEN_MAX 100
extern char yytext[GETTOKEN_MAX];
bool yylexopen(const char filename[]);
void yytextclear();
void yytextappend(int c);
TokStruct getToken();
int numTokens = 0;
string val;
string rwTok;
TokStruct token;

int main(int argc, char *argv[])
{
	if (argc > 1 && (!yylexopen(argv[1])))
	{
		cout << "Error: Cannot open input file " << argv[1] << endl;
		exit(1);
	}

	while (1)
	{
		token = getToken();
		//Outputs the tokens
		if (token.TokS == ASSIGNSY || token.TokS == LPAREN || token.TokS == RPAREN || token.TokS == ADDOP || token.TokS == MINOP || token.TokS == MULOP
			|| token.TokS == DIVOP || token.TokS == ID || token.TokS == NUMCONST || token.TokS == READSY || token.TokS == WRITESY || token.TokS == EOFSY)
		{
			//	cout << "Token: " << setfill('0') << setw(2) << token.TokS << " " << val << " (" << token.str << ")" << endl;	
				parse();
		}
		if (token.TokS == EOFSY)
		{
			break;
		}
	}
	//Outputs the number of tokens
	//cout << endl << "Number of tokens: " << numTokens << endl;

	return (0);
}

char yytext[GETTOKEN_MAX]; // Global token text buffer
static int yytextpos = 0;
ifstream fin; // lex input file stream
bool yylexopen(const char filename[]) 
{
	fin.open(filename, ios_base::in);
	return fin.is_open();
}
void yytextclear() 
{
	yytextpos = 0; yytext[yytextpos] = 0;
}
void yytextappend(int c) 
{
	if (yytextpos >= GETTOKEN_MAX - 1)
		return; // ignore char
	yytext[yytextpos++] = (char)c;
	yytext[yytextpos] = 0; // null sentinel at end
}
//makes all characters uppercase
void makeUpper()
{
	for (unsigned int i = 0; i < strlen(yytext); i++)
	{
		yytext[i] = toupper(yytext[i]);
	}
}
// checks to see if the token was expected or not and gets next token
void match(GTokens expectedTok) 
{
	if (token.TokS == expectedTok) 
	{
		if (token.TokS != EOFSY) 
		{
			token = getToken();
		}
	}
	else
	{
		cout << "Token " << expectedTok
			<< " expected, but instead "
			<< "encountered " << yytext << endl;
	}
}
// <parse> -> <stmt_list> $$
int parse() 
{
	stmtList();
	match(EOFSY);
	return 0;
}
// <stmt_list> -> <stmt> <stmt_list> | ε
void stmtList() {
	if (token.TokS != EOFSY) 
	{
		stmt();
		stmtList();
	}
}
// <stmt> -> id := <expr> | read id | write <expr>
void stmt() 
{
	if (token.TokS == ID) 
	{
		string idStr = token.str;
		match(ID);
		match(ASSIGNSY);
		symbolTable[idStr] = expr();
		cout << "Assign: " << idStr << " = " << symbolTable[idStr] << endl;
	}
	else if (token.TokS == WRITESY) 
	{
		match(WRITESY);
		double writes = expr();
		cout << "Write: " << writes << endl;
	}
	else if (token.TokS == READSY) 
	{
		match(READSY);
		string reads;
		cout << "Read: Enter value for " << token.str << "> ";
		cin >> reads;

		//Add symbol to the table
		symbolTable[token.str] = strtod(reads.c_str(), NULL);
		match(ID);
		cin.clear();
	}
	else 
	{
		match(EOFSY);
	}
}
// <expr> -> <term> <term_tail>
double expr() 
{
	double val1 = term();
	double val2 = termTail(val1);
	return val2;
}
// <term_tail> -> <add_op> <term> <term_tail> | ε
double termTail(double inval) 
{
	if (token.TokS == ADDOP) 
	{
		match(ADDOP);
		double val1 = term();
		double val2 = termTail(inval + val1);
		return val2;
	}
	else if (token.TokS == MINOP) 
	{
		match(MINOP);
		double val1 = term();
		double val2 = termTail(inval - val1);
		return val2;
	}
	return inval;
}
// <term> -> <factor> <factor_tail>
double term() 
{
	double val1 = factor();
	double val2 = factorTail(val1);
	return val2;
}
// <factor_tail> -> <mul_op> <factor> <factor_tail> | ε
double factorTail(double inval) 
{
	if (token.TokS == MULOP) 
	{
		match(MULOP);
		double val1 = factor();
		double val2 = factorTail(inval * val1);
		return val2;
	}
	else if (token.TokS == DIVOP) 
	{
		match(DIVOP);
		double val1 = factor();
		double val2 = factorTail(inval / val1);
		return val2;
	}
	return inval;
}
// <factor> -> (<expr>) | id | number
double factor() 
{
	if (token.TokS == LPAREN) 
	{
		match(LPAREN);
		double val = expr();
		match(RPAREN);
		return val;
	}
	else if (token.TokS == NUMCONST) 
	{
		//changes string to double
		double val = strtod(token.str.c_str(), NULL);
		match(NUMCONST);
		return val;
	}
	else 
	{
		double val = symbolTable[token.str];
		match(ID);
		return val;

	}
}


TokStruct getToken() 
{
	int c = 0;
	//Used to save enum term to use in main function
	val = "";
	//Struct used to go through the tokens
	TokStruct tok;
	start:
	while (1) 
	{
		//Keeps track of number of tokens
		numTokens++;
		//Clears before finding next token
		yytextclear();
		//Used for read and write tokens
		rwTok = "";
		//ignore white spaces
		while ((c = fin.get()) == ' ' || c == '\t' || c == '\n')
			;
		//EOFSY
		if (c == EOF)
		{
			tok.TokS = EOFSY;
			val = "EOFSY-$$";
			numTokens--;
			return tok;
		}
		//ASSIGNSY
		if (c == ':') 
		{
			string assign;
			assign = c;
			c = fin.get();
			if (c == '=') 
			{
				assign += c;
				tok.TokS = ASSIGNSY;
				tok.str = assign;
				val = "ASSIGNSY";
				return tok;
			}
		}
		//LPAREN 
		if (c == '(') 
		{
			tok.TokS = LPAREN;
			tok.str = c;
			val = "LPAREN";
			return tok;
		}
		//RPAREN 
		if (c == ')') 
		{
			tok.TokS = RPAREN;
			tok.str = c;
			val = "RPAREN";
			return tok;
		}
		//ADDOP 
		if (c == '+') 
		{
			tok.TokS = ADDOP;
			tok.str = c;
			val = "ADDOP";
			return tok;
		}
		//MINOP 
		if (c == '-') 
		{
			tok.TokS = MINOP;
			tok.str = c;
			val = "MINOP";
			return tok;
		}
		//MULOP 
		if (c == '*') 
		{
			tok.TokS = MULOP;
			tok.str = c;
			val = "MULOP";
			return  tok;
		}
		//DIVOP
		if (c == '/')
		{
			string div = "";
			div = c;
			c = fin.get();
			if (c != '*') {
				tok.TokS = DIVOP;
				tok.str = div;
				val = "DIVOP";
				fin.unget();
				return tok;
			}
			//Checks for comments and ignores them
			c = fin.get();
			while (1)
			{
				while (c != '*' && c != '\n' && c != EOF)
					c = fin.get();
				if (c == '\n') goto start;
				if (c == EOF) return tok;
				c = fin.get();
				if (c == '/') goto start;
				if (c == '\n') goto start;
				if (c == EOF) return tok;
			}
		}
		//NUMCONST
		if (isdigit(c)) 
		{
			string num;
			fin.unget();
			while ((c = fin.get()) >= '0' && c <= '9')
			{
				num += c;
			}
			if (c == '.')
			{
				num += c;
				while ((c = fin.get()) >= '0' && c <= '9')
				{
					num += c;
				}
			}
			tok.TokS = NUMCONST;
			tok.str = num;
			val = "NUMCONST";
			fin.unget();
			return tok;
		}
		//Checks if it is a letter
		if (isalpha(c)) 
		{
			while (isalpha(c) || isdigit(c) || c == '_') 
			{
				if (c != ' ')
				{
					rwTok += c;
					yytextappend(c);
					c = fin.get();
				}
				else
					break;
			}
			fin.unget();
			//READSY
			if (strcmp(yytext, "read") == 0) 
			{
				makeUpper();
				val = "READSY";
				tok.TokS = READSY;
				tok.str = yytext;
				return tok;
			}
			//WRITESY
			if (strcmp(yytext, "write") == 0) 
			{
				makeUpper();
				val = "WRITESY";
				tok.TokS = WRITESY;
				tok.str = yytext;
				return tok;
			}
			//ID
			else {
				makeUpper();
				val = "ID";
				tok.TokS = ID;
				tok.str = yytext;
				return tok;
			}
		}
		return tok;
	}
	return tok;
}

