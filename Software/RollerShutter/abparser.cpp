#include "abparser.h"
/*
Evaluating arithmetic expressions from string in C++
recursive descent parser. 
https://en.wikipedia.org/wiki/Recursive_descent_parser
http://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
*/

//const char * expressionToParse = "((t!=0)&(a==(3-5)|0)&1";
//const char * expressionToParse = "(p*(2.25>2.15))&1";
char * expressionToParse;
char key[30];

char peek()
{
	return *expressionToParse;
}

char peek2()
{
	return *(expressionToParse+1);
}

char get()
{
	return *expressionToParse++;
}

double expression();
/*
double isdigit(char c){
	return (c >= '0' && c <= '9');
}
*/

void keyterm(){
	char c;
	c = peek2();
	key[0] = peek();
	key[1] = '\0';

	//for(int i=1; i<10 && (c>= 'a' && c<= 'z' ||c >= '0' && c <= '9'); i++){	
	for(int i=1; i<29 && (c>= 'a' && c<= 'z' || c>= 'A' && c<= 'Z' || c >= '*' && c <= ':'); i++){	//per includere indirizzi IP e date
		key[i] = c;
		key[i+1] = '\0';
		get();
		c = peek2();
	}
}

double number()
{
    bool decimal = false;
	
	double result = get() - '0';
	double div =  1;
    while (peek() >= '0' && peek() <= '9' || peek() == '.')
    {
		if(peek() == '.' && decimal == false){
			decimal = true;
			get();
		}
		if(decimal==false){
			result = 10*result + get() - '0';
		}else{
			div /= 10.0;
			result +=  (double) (get() - '0')*div;
		}
    }
    return result;
}

double function()
{   
	get();
	return variables(key);
}

double action(double val)
{
    return actions(key,val);
}

double factor()
{
	//elemento con elevata precedenza (numero, gruppo di lettere, espressione racchiusa tra parentesi)
	if (peek() >= '0' && peek() <= '9'){
        return number();
    }else if (peek() == '(')
    {
        while(peek2()==' ')
			get();
		get(); // '('
        double result = expression();
		while(peek2()==' ')
			get();
        get(); // ')'
        return result;
    }
    else if (peek() == '-')
    {
        get();
        return -factor();
    }else if (peek() == '!')
    {
        get();
        return !factor();
    }else if (peek() >= 'a' && peek() <= 'z'){
		keyterm();
		return function();
	}
    return 0; // error
}

double term()
{
    //gruppo di elementi con elevata precedenza (fattori) ovvero prodotto di fattori (cio termine = monomio)
	//primo parametro
	double result = factor();
    while (peek() == '[' || peek() == '*' || peek() == '/' || peek() == '^'|| peek() == '&' || peek() == '>' || peek() == '<' || peek() == '=' || peek() == '!' || peek() == '?' || peek() == ' '){
        if (peek() == '*'){
			get();
            result *= factor();
        }else if (peek() == '/'){
			get();
            result /= factor();
		}else if (peek() == '&'){
			get();
			result = result && factor();
		}else if (peek() == '>'){
			if(peek2() == '='){
				get();
				get();
				result = (result >= factor());
			}else{
				get();
				result = (result > factor());
			}
		}else if (peek() == '<'){
			if(peek2() == '='){
				get();
				get();
				result = (result <= factor());
			}else{
				get();
				result = (result < factor());
			}
		}else if (peek() == '='){
			if(peek2() == '='){
				get();
				get();
				result = (result == factor());
			}else{//ASSIGNEMENT!
				get();
				result = action(expression());
			}
		}else if (peek() == '!'){
			if(peek2() == '='){
				get();
				get();
				result = (result != factor());
			}else{
				
			}
		}else if (peek() == '^'){
			get();
            result = pow(result, factor());
        }else if (peek() == '?'){ //if corto
			if(result){
				get(); // salto '?'
				result = factor();
				get(); // salto ':'
				while(get()!=';');	
			}else{
				while(get()!=':');
				result = factor();
				get(); // salto ';'
			}
			return result;
		}else if (peek2() == ' '){
			get();
        }else if (peek() == '['){
			double a,b,c;
			bool o;
			unsigned short nc = 0;
			//i#o1,a,b,c,d 
			get();	// '['
			o = factor();
			if (get() == ','){
				nc = 1;
				a = expression();
				if (get() == ','){
					nc = 2;
					b = expression();
					if (get() == ','){
						nc = 3;
						c = expression();
					}
				}
			}
			get(); // ']'

			switch(nc){
				case 0:
					result = (result == o);
				break;
				case 1:
					result = (result > o && result < a); //0<res<a
				break;
				case 2:
					/*if(result < (a-b)){
						result = 0;
					}else if(result > (a+b)){//a-b<res<a+b
						result = 1;
					}else{
						//lo stato non cambia
						result = o;
					}*/
					if(result < (a-b)){
						result = 0;
					}else if(result > (a+b)){//a-b<res<a+b
						result = 0;
					}else{
						result = 1;
					}
				break;
				case 3://?
					if(result < (a-b)){
						result = 0;
					}else if(result > (a+c)){
						result = 1;
					}else{
						//lo stato non cambia
						result = o;
					}
				break;
			}
		}
	}	
		
    return result;
}

double expression()
{
	//gruppo di elementi con bassa precedenza (termini) ovvero somma/differenza di termini (cio espressione = somma_monomi)
    //primo parametro
	double result = term();
    while (peek() == '+' || peek() == '-' || peek() == '|' || peek() == '!'){
        if (peek() == '+'){
			get();
            result += term();
        }else if (peek() == '-'){
			get();
            result -= term();
		}else if (peek() == '|'){
			get();
            result = (result + term()) != 0;
        }else if (peek() == '!'){
			get();
            result = (result + !term()) != 0;
		}
	}
    return result;
}

double eval(const char *str){
	expressionToParse = (char *) str;
	return expression();
}

/*
Based on:
https://stackoverflow.com/questions/9329406/evaluating-arithmetic-expressions-from-string-in-c

const char * expressionToParse = "3*2+4*1+(4+9)*6";

char peek()
{
    return *expressionToParse;
}

char get()
{
    return *expressionToParse++;
}

int expression();

int number()
{
    int result = get() - '0';
    while (peek() >= '0' && peek() <= '9')
    {
        result = 10*result + get() - '0';
    }
    return result;
}

int factor()
{
    if (peek() >= '0' && peek() <= '9')
        return number();
    else if (peek() == '(')
    {
        get(); // '('
        int result = expression();
        get(); // ')'
        return result;
    }
    else if (peek() == '-')
    {
        get();
        return -factor();
    }
    return 0; // error
}

int term()
{
    int result = factor();
    while (peek() == '*' || peek() == '/')
        if (get() == '*')
            result *= factor();
        else
            result /= factor();
    return result;
}

int expression()
{
    int result = term();
    while (peek() == '+' || peek() == '-')
        if (get() == '+')
            result += term();
        else
            result -= term();
    return result;
}

int _tmain(int argc, _TCHAR* argv[])
{

    int result = expression();

    return 0;
}
*/
