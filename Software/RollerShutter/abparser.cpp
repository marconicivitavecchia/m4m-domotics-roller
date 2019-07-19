#include "abparser.h"
/*
Evaluating arithmetic expressions from string in C++
recursive descent parser.
https://stackoverflow.com/questions/9329406/evaluating-arithmetic-expressions-from-string-in-c
*/

//const char * expressionToParse = "((t!=0)&(a==(3-5)|0)&1";
//const char * expressionToParse = "(p*(2.25>2.15))&1";
char * expressionToParse;
char key[20];

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

float expression();
/*
float isdigit(char c){
	return (c >= '0' && c <= '9');
}
*/

void keyterm(){
	char c;
	c = peek2();
	key[0] = peek();
	key[1] = '\0';

	for(int i=1; i<10 && (c>= 'a' && c<= 'z' ||c >= '0' && c<= '9'); i++){	
		key[i] = c;
		key[i+1] = '\0';
		get();
		c = peek2();
	}
}

float number()
{
    bool decimal = false;
	
	float result = get() - '0';
	float div =  1;
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
			result +=  (float) (get() - '0')*div;
		}
    }
    return result;
}

float function()
{   
	get();
	return variables(key);
}

float action(float val)
{
    return actions(key,val);
}

float factor()
{
	if (peek() >= '0' && peek() <= '9'){
        return number();
    }else if (peek() == '(')
    {
        get(); // '('
        float result = expression();
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

float term()
{
    //primo parametro
	float result = factor();
    while (peek() == '[' || peek() == '*' || peek() == '/' || peek() == '^'|| peek() == '&' || peek() == '>' || peek() == '<' || peek() == '=' || peek() == '!'){
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
        }else if (peek() == '['){
			float a,b,c;
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
					result = (result > o && result < a);
				break;
				case 2:
					if(result < (a-b)){
						result = 0;
					}else if(result > (a+b)){
						result = 1;
					}else{
						//lo stato non cambia
						result = o;
					}
				break;
				case 3:
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

float expression()
{
    float result = term();
    while (peek() == '+' || peek() == '-' || peek() == '|' || peek() == '!')
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
    return result;
}

float eval(const char *str){
	expressionToParse = (char *) str;
	return expression();
}
