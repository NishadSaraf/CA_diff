/*
* Title: Program to calculates first order differentiation of a user input function.
* @Author: Nishad Saraf
  @Course: ECE 586 - Computer Architecture-Winter 2017
* @University: Portland State University, Oregon
*
* Description: This program take an input function f(x)from user, sorts the input function according to the level of precedence by inserting curved brackets,
* then calculates the first order derivative and again sorts the differentiated function f'(x) and discards the components which evaluates to zero, and
* if the reduced value f'(x) is totally a constant expression it also evaluates the value.
* But there are some limitations on the input function. No expression than addition, subtraction, multiplication, division, exponential, and logarithm expression in x can be
* calculated. Mathematical operators like sine, cosine, etc. cannot be input to the program. 'log' operator is denoted by & for simplicity.
* Program creates a binary search tree to process the input expression. Tree traversal is in order traversal.
* Program uses a C++ library called calculator written by Kim Walisch. This library parses input string expression, does simple arithmetic and logical operation is outputs the
* result.
*/
// Header files
#include "calculator.hpp"
#include <iostream>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
// Macros definitions
#define NULL 0
#define FALSE 0
#define TRUE 1
#define VERIFY(cond, msg) \
	if(! cond)\
	{	\
		printf("<!><!>Error. %s\n", msg);\
	}

typedef enum
{
	Literal,
	Identifier,
	Operator
} NodeClass;

typedef struct NodeType * NodePtr;

typedef struct NodeType
{
	NodeClass Class;
	char Symbol;
	int LitVal;
	NodePtr LeftNode;
	NodePtr RightNode;
}s_node_tp;

bool isConstExp = true;
int i = 0;
char str[2000];
char NextChar;

s_node_tp OneNode = { Literal, '1',1, NULL,NULL };
s_node_tp NullNode = { Literal, '0',0, NULL,NULL };

/**
* This function creates a node by allocating space dynamically.
* Returns the address to the newly created node which is nothing but pointer to a structure.
*/
NodePtr Create(NodeClass Class, char Symbol, int value, NodePtr LeftNode, NodePtr RightNode)
{
	NodePtr Node = (NodePtr)malloc(sizeof(struct NodeType));
	Node->Class = Class;
	Node->Symbol = Symbol;
	Node->LitVal = value;
	Node->LeftNode = LeftNode;
	Node->RightNode = RightNode;
	return Node;
}
/*
* Function copies the node and its children
*/
NodePtr Copy(NodePtr Root)
{
	if (Root == NULL)
	{
		return NULL;
	}
	else {
		return Create(Root->Class, Root->Symbol, Root->LitVal, Copy(Root->LeftNode), Copy(Root->RightNode));
	}
}
/**
* This is the function where differentiation of input expression is calculated.
* Rule for differentiation are:
* derive( x )      = 1
* derive( a )      = 0		-- any variable ‘a’ etc. other than ‘x’ or ‘X’
* derive( number ) = 0		-- with ‘number’ being a decimal integer
* derive( u + v )  = u' + v' 	-- where u = f2(x) and v = f3(x)
* derive( u - v )  = u' - v'					-- ditto for u and v
* derive( u * v )  = u' * v + u * v'				-- AKA product rule
* derive( u / v )  = ( u' * v - u * v' ) / ( v * v)	-- quotient rule
* derive( u ^ v )  = u' * v * u ^ ( v - 1 ) + & u * v' * u ^ v
* derive( & u )    = u' / u	-- the & operator stands for ln, natural log
*/
NodePtr Derive(NodePtr Root)
{
	if (Root == NULL)
	{
		return NULL;
	}
	else
	{
		switch (Root->Class)
		{
		case Literal:
			return Create(Literal, '0', 0, NULL, NULL);	// differentiation of a literal is zero.
		case Identifier:
			if ((Root->Symbol == 'x') || (Root->Symbol == 'X'))	// differentiation of x is 1.
			{
				return Create(Literal, '1', 1, NULL, NULL);
			}
			else
			{
				return Create(Literal, '0', 0, NULL, NULL);	// differentiation for any other identifier other than x is zero
			}
		case Operator:
			switch(Root -> Symbol)
			{
		case '+':case '-':
			return Create(Operator, Root->Symbol, 0, Derive(Root->LeftNode), Derive(Root->RightNode));

		case '*':
			return Create(Operator, '+', 0,
				Create(Operator, '*', 0, Derive(Root->LeftNode), Copy(Root->RightNode)),
				Create(Operator, '*', 0, Copy(Root->LeftNode), Derive(Root->RightNode)));

		case '/':
			return Create(Operator, '/', 0, Create(Operator, '-', 0,
				Create(Operator, '*', 0, Derive(Root->LeftNode), Copy(Root->RightNode)),
				Create(Operator, '*', 0, Copy(Root->LeftNode), Derive(Root->RightNode))),
				Create(Operator, '*', 0, Copy(Root->RightNode), Copy(Root->RightNode)));

		case '^':
			return Create(Operator, '+', 0,
				Create(Operator, '*', 0, Derive(Root->LeftNode),
					Create(Operator, '*', 0, Copy(Root->RightNode),
						Create(Operator, '^', 0, Copy(Root->LeftNode),
							Create(Operator, '-', 0, Copy (Root->RightNode),
								Copy(&OneNode))
						)
					)
				),
				Create(Operator, '*', 0,
					Create(Operator, '*', 0,
						Create(Operator, '&', 0, NULL, Copy(Root->LeftNode)),
						Derive(Root->RightNode)),
					Create(Operator, '^', 0,
						Copy(Root->LeftNode), Copy(Root->RightNode))
				)
			);

		case '&':
			if (Root->LeftNode != NULL)
			{
				printf("ln has only one operand.\n");
			}
			return Create(Operator, '/', 0,
				Derive(Root->RightNode), Copy(Root->RightNode));

		default:
			printf("Impossible operator.\n");
			return NULL;
			}
		default:
			printf("Unknown Root class.\n");
			return NULL;
		}

	}

}

/**
* Function returns true if char value is a literal
*/
unsigned IsLit(char value, NodePtr Root)
{
	if (!Root)
	{
		return FALSE;
	}
	else
	{
		return(Root->Class == Literal) && (Root->Symbol == value);
	}
}
/**
* Function check whether both the left and right node are literals
*/
unsigned BothLit(NodePtr LeftNode, NodePtr RightNode)
{
	if (!LeftNode)
	{
		return FALSE;
	}
	else if (!RightNode)
	{
		return FALSE;
	}
	else
	{
		return (LeftNode->Class == Literal) && (RightNode->Class == Literal);
	}
}
/**
* Function checks whether the two characters of any two same classes are equal or not
*/
unsigned IsEqual(NodePtr LeftNode, NodePtr RightNode)
{
	if ((LeftNode == NULL) && (RightNode == NULL))
	{
		return TRUE;
	}
	else if (LeftNode == NULL)
	{
		return FALSE;
	}
	else if (RightNode == NULL)
	{
		return FALSE;
	}
	else if ((LeftNode->Class == Literal) && (RightNode->Class == Literal))
	{
		return ((LeftNode->LitVal) == (RightNode->LitVal));
	}
	else if ((LeftNode->Class == Identifier) && (RightNode->Class == Identifier))
		return ((LeftNode->Symbol) == (RightNode->Symbol));
	else
	{
		if ((LeftNode->Symbol) == (RightNode->Symbol))
		{
			return (IsEqual(LeftNode->LeftNode, RightNode->LeftNode) && IsEqual(LeftNode->RightNode, RightNode->RightNode));
		}
		else
		{
			return FALSE;
		}
	}

	printf("Impossible");
}
/**
* Function simplifies the input function by discarding node which involves addition or subtraction with zero,
* unnecessary multiplication with or division by 1 and multiplication or division by zero and exponential operation where exponent is 1 or 0
*/
NodePtr Simplify(NodePtr Root)
{
	int val = 0;

	if (Root == NULL)
	{
		return Root;
	}
	else
	{
		switch (Root->Class)
		{
		case Literal:
		case Identifier:
			return Root;
		case Operator:
			Root->LeftNode = Simplify(Root->LeftNode);
			Root->RightNode = Simplify(Root->RightNode);
			switch (Root->Symbol)
			{
			case '+':
				if (IsLit('0', Root->LeftNode))
				{
					return Root->RightNode;
				}
				else if (IsLit('0', Root->RightNode))
				{
					return Root->LeftNode;
				}
				else if (BothLit(Root->LeftNode, Root->RightNode))
				{
					val = Root->LeftNode->LitVal + Root->RightNode->LitVal;
					return Create(Literal, (char)(val + '0'), val, NULL, NULL);
				}
				else
				{
					return Root;
				}

			case '-':

				if (IsLit('0', Root->RightNode))
				{
					return Root->LeftNode;
				}
				else if (BothLit(Root->LeftNode, Root->RightNode))
				{
					val = Root->LeftNode->LitVal - Root->RightNode->LitVal;
					return Create(Literal, (char)(val + '0'), val, NULL, NULL);
				}
				else if (IsEqual(Root->LeftNode, Root->RightNode))
				{
					return &NullNode;	// returns node with value 0
				}
				else
				{
					return Root;
				}

			case '*':

				if (IsLit('1', Root->LeftNode))
				{
					return Root->RightNode;
				}
				else if (IsLit('1', Root->RightNode))
				{
					return Root->LeftNode;
				}
				else if (IsLit('0', Root->LeftNode) || IsLit('0', Root->RightNode))
				{
					return &NullNode;
				}
				else
				{
					return Root;
				}

			case '/':

				if (IsLit('1', Root->RightNode))
				{
					return Root->LeftNode;
				}
				else if (IsLit('0', Root->LeftNode))
				{
					return &NullNode;
				}
				else if (IsEqual(Root->LeftNode, Root->RightNode))
				{
					return &OneNode;	// return node with value 1
				}
				else
				{
					return Root;
				}


			case '^':

				if (IsLit('0', Root->RightNode))
				{
					return &OneNode;
				}
				else if (IsLit('1', Root->RightNode))
				{
					return Root->LeftNode;
				}
				else
				{
					return Root;
				}


			case '&':

				if (IsLit('1', Root->RightNode))
				{
					return &NullNode;
				}
				else
				{
					return Root;
				}

			}
		}
	}
}
/**
* Traverse through the entire binary tree following inorder tree traversal algorithm
*/
void PrintTree(NodePtr Root)
{
	if (Root != NULL)
	{
		if (Root->Class == Operator)
		{
			printf("(");
		}
		PrintTree(Root->LeftNode);
		if (Root->Class == Literal)
		{
			printf("%d", Root->LitVal);
		}
		else {
			printf("%c", Root->Symbol);
		}
		PrintTree(Root->RightNode);
		if (Root->Class == Operator)
		{
			printf(")");
		}
	}
}

/**
* Functions much similar to PrintTree but aslo converts the binary tree back to a string format
* String is stored in a array of characters.
*/
void PrintTree1(NodePtr Root)
{
   char temp[1000];
	if (Root != NULL)
	{
		if (Root->Class == Operator)
		{
			str[i] = '(';
			i++;
		}
		PrintTree1(Root->LeftNode);
		if (Root->Class == Literal)
		{
			sprintf(temp, "%d", Root->LitVal); // converts a int to char and stores it in temp
            int z = strlen(temp);
            for(int a = 0;a < z;a++){
                str[i]=temp[a];
                i++;
            }
		}
		else
        {
			str[i] = (Root->Symbol);
			i++;
		}
		PrintTree1(Root->RightNode);
		if (Root->Class == Operator)
		{
			str[i] = ')';
			i++;
		}
	}
}
/**
* Copy the next character in the input string into NextChar unless "$" is found.
*/
void GetNextChar()
{
	if (NextChar != '$')
	{
		// take one char at a a time from stdin omitting the blank space
		NextChar = getchar();
		if (NextChar == ' ')
			GetNextChar();
	}

}
/**
* Check whether the char is nunmeral
*/
unsigned IsDigit(char c)
{
	return ((c >= '0') && (c <= '9'));
}
/**
* Check whether the char is an alphabet in low case or in upper case
*/
unsigned IsLetter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

NodePtr Expression();
/**
* This function is used to identify the whether the input symbol is a digit, a letter or a log operator.
* Creates node node accordingly.
*/
NodePtr Factor()
{
	char Symbol;
	NodePtr Temp;
	Symbol = NextChar;
	GetNextChar();
	if (IsDigit(Symbol)) 	// if the char is zero or is not a numeral there is not need to create a new node
		return Create(Literal, Symbol, (int)(Symbol -'0'), NULL, NULL);
	else if (IsLetter(Symbol))
		return Create(Identifier, tolower(Symbol), 0, NULL, NULL);	// accept any case alphabet but while creating a node store them to lower case
	else if (Symbol == '(')
	{
		Temp = Expression();
		if (NextChar != ')')
			printf(") expected\n");
		GetNextChar();
		return Temp;

	}
	else if (Symbol == '&')
	{
		return Create(Operator, '&', 0, NULL, Factor());	// a natural log expression will only have a right node
	}
	else {
		printf("Illegal character '%c'\n", Symbol); // if input charater is not a literal, a letter or natural log
	}

}
/**
*  This function works to filter out exponential operator, returns pointer to its newly created node.
*  At the same time it creates a left child node by branching to Factor().
*/
NodePtr Primary()
{
	NodePtr LeftNode = Factor();
	if (NextChar == '^')		// a exponential operator may have both left child nodes
	{
		GetNextChar();
		LeftNode = Create(Operator, '^', 0, LeftNode, Factor());
	}
	return LeftNode;
}

/**
* This function filters out any multiplication or division operations.
*/
NodePtr Term()
{
	NodePtr LeftNode;
	char Op;
	LeftNode = Primary();
	while (NextChar == '*' || NextChar == '/')
	{
		Op = NextChar;
		GetNextChar();
		LeftNode = Create(Operator, Op, 0, LeftNode, Primary());
	}
	return LeftNode;

}
 /**
 * The lowest precedence operators are handled at the very last step.
 * This function filters out addition and subtraction operation.
 */
NodePtr Expression()
{
	NodePtr LeftNode;
	char Op;
	LeftNode = Term();
	while (NextChar == '-' || NextChar == '+')
	{
		Op = NextChar;
		GetNextChar();
		LeftNode = Create(Operator, Op, 0, LeftNode, Term());
	}
	return LeftNode;
}
/**
* Let the user know about the limitations of program.
*/
void Initialize()
{
	printf("~~~~~~~ Program to calculate first order derivative ~~~~~~~\n");
	printf("Rules:\n");
	printf("1. Input function should  only be written in x.\n");
	printf("2. Avoid use of trigonometric and inverse trigonometric function.\n");
	printf("3. Only use natural log and symbol used is '&'.\n");
	printf("4. End your input function with '$'.\n");
	printf("\nEnter f(x):");
	NextChar = ' ';
	GetNextChar();
}

#define SHOW(msg, r) \
printf("%s", msg); \
PrintTree(r) ; \
printf("\n") \

int main()
{
    int j;
	NodePtr root = NULL;
	Initialize();
	root = Expression();	// create a binary search tree for the input expression. root returns a pointer to the top most node
	VERIFY((NextChar == '$'), "Insert $ at the end of expression\n");	// if the last char in the input string is not $ display error message
	SHOW("Original f(x) = ", root); // display the saved binary tree
	root = Simplify(root); //
	SHOW("Simplified f(x) = ", root);
	root = Derive(root);
	SHOW("Derived f'(x) = ", root);
	root = Simplify(root);
	SHOW("Reduced f'(x) = ", root);
	// parse the final reduced expression of f'(x) into an array
    PrintTree1(root);
    int l = strlen(str);
	// check if CEE can be applied. fails if its still a function of x
    for (j = 0; j < l; j++)
    {
        if(str[j] == 'x' )
        {
             isConstExp = false;
             break;
        }
    }
    if(isConstExp)
    {
        printf("Constant Expression Evaluation of Reduced f'(x) = ");
        std::cout<< calculator::eval(str) <<std::endl;	// pass the string to function eval defined in calculator library
    }
	return 0;

}
