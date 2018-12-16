#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Bignum{
	long num_of_digits;
	char *digit;
	int is_neg;
}Bignum;

typedef struct Stack{
	struct Bignum *value;
	struct Stack *prev;
}Stack;

extern char * big_add (Bignum * num1, Bignum * num2, Bignum * new_num);
extern char * big_sub (Bignum * num1, Bignum * num2, Bignum * new_num);
extern char * div_by_two(Bignum * dividend, Bignum* answer);

int isEmpty(Stack *s){
	return (!s);
}
void _push (Stack **s, Bignum *val){
	Stack * new_stack;
	new_stack = (Stack*)malloc(sizeof(Stack));
	new_stack->value = val;
	new_stack->prev = *s;
	*s = new_stack;
}
Bignum* _pop (Stack **s){
	Bignum* ret = NULL;
	Stack * prev_stack = NULL;
	if (isEmpty(*s)){
		exit(-1);
	}
	prev_stack = (*s)->prev;
	ret = (*s)->value;
	free(*s);
	*s = prev_stack;
	return ret;
}
Bignum* peek (Stack **s){
	if (isEmpty(*s)){
		exit(1);
	}
	return (*s)->value;
}

long get_max_size(long first_size, long second_size){
	if (first_size>second_size){
		return first_size;
	}
	return second_size;    
}
/*
 * This function recive two numbers, and compare them
 * if the first number is bigger - return 1
 * if both the numbers are equal - return 0
 * else, return -1
 */
int get_bigger_num (Bignum *first, Bignum *second){
	if (first->is_neg && !second->is_neg){
		return -1;
	}
	else if (!first->is_neg && second->is_neg){
		return 1;
	}
	else{ // two numbers have the same sign
		if (first->is_neg && second->is_neg){ // both of them negative
			if (first->num_of_digits>second->num_of_digits){
				return -1;
			}
			else if (first->num_of_digits<second->num_of_digits){
				return 1;
			}
			else{
				char *c1 = first->digit;
				char *c2 = second->digit;
				int j=0;
				int i = first->num_of_digits;
				while (j<i){
					if (c1[j] == c2[j]){
						j++;
					}
					else if (c1[j] < c2[j]){
						return 1;
					}
					else{
						return -1;
					}
				}
				return 0;
			}
		}
		else{ // both of them positive
			if (first->num_of_digits>second->num_of_digits){
				return 1;
			}
			else if (first->num_of_digits<second->num_of_digits){
				return -1;
			}
			else{
				char *c1 = first->digit;
				char *c2 = second->digit;
				int j=0;
				int i = first->num_of_digits;
				while (j<i){
					if (c1[j] == c2[j]){
						j++;
					}
					else if (c1[j] > c2[j]){
						return 1;
					}
					else{
						return -1;
					}
				}
				return 0;
			}
		}
	}
}
void arrange_num (Bignum *num){
		char *c = num->digit;
		int i = 0;
		while ((*c++ == '0')&&i<num->num_of_digits){
			i++;
		}
		if (i==num->num_of_digits){
			free(num->digit);
			num->digit = calloc(2, sizeof(char));
			num->digit[0] = '0';
			num->num_of_digits = 1;
		}
		else{
			int x = num->num_of_digits;
			x = x-i;
			num->num_of_digits = x;
			c = num->digit;
			char *temp = calloc (x+1,sizeof(char));
			int j;
			for (j=0; j<x; j++){
				temp[j] = c[j+i];
			}
			temp[j] = 0;
			free(num->digit);
			num->digit = temp;
		}
}
void free_mem (Bignum * nummy){
	free(nummy->digit);
	nummy->is_neg = 0;
	nummy->num_of_digits = 0;
	free(nummy);
}
void clear_stack (Stack **s){
	while (!(isEmpty(*s))){
		Bignum * temp = _pop(s);
		free_mem(temp);
	}
}
int is_zero (Bignum* nummy){
	return (strcmp(nummy->digit, "0")==0);
}
int is_one (Bignum* nummy){
	return (strcmp(nummy->digit, "1")==0);
}
Bignum* copy_num (Bignum* nummy){
	Bignum* result = (Bignum*)malloc(sizeof(Bignum));
	result->is_neg = nummy->is_neg;
	long dig_num = nummy->num_of_digits;
	result->num_of_digits = dig_num;
	result->digit = calloc((dig_num+1),sizeof(char));
	int i;
	for (i=0; i<dig_num; i++){
		(result->digit)[i] = (nummy->digit)[i];
	}
	return result;
}

Bignum * create_num(){
	Bignum * ret_val = (Bignum*)malloc(sizeof(Bignum));
	ret_val->digit = calloc(2, sizeof(char));
	ret_val->digit[0] = '0';
	ret_val->num_of_digits= 1;
	ret_val->is_neg = 0;
	return ret_val;
}
Bignum* add_nums (Bignum* first_bignum, Bignum* second_bignum){
	long first_size = first_bignum->num_of_digits;
	long second_size = second_bignum->num_of_digits;
	if (is_zero(first_bignum)){
		Bignum * result = copy_num(second_bignum);
		return result;
	}
	if (is_zero(second_bignum)){
		Bignum * result = copy_num(first_bignum);
		return result;
	}
	Bignum * result = (Bignum*)malloc(sizeof(Bignum));
	long max = get_max_size(first_size, second_size);
	max = max + 2;
	result->is_neg = 0;
	result->digit = (char*)calloc(max, sizeof(char));
	result->num_of_digits = max-1;
	if (first_bignum->is_neg && !second_bignum->is_neg){
		if (first_size==second_size){
			first_bignum->is_neg = 0;
			int x = get_bigger_num(first_bignum, second_bignum);
			if (x==1){
				result->digit = big_sub(first_bignum, second_bignum, result);
				result->is_neg = 1;
			}
			else{
				result->digit = big_sub(second_bignum, first_bignum, result);
			}
		}
		else if (first_size>second_size){
			result->digit = big_sub(first_bignum,second_bignum, result);
			result->is_neg = 1;
		}
		else{
			result->digit = big_sub(second_bignum, first_bignum, result);
		}
	}
	else if (!first_bignum->is_neg && second_bignum->is_neg){
		if (first_size==second_size){
			second_bignum->is_neg = 0;
			int x = get_bigger_num(first_bignum, second_bignum);
			if (x==1){
			result->digit = big_sub(first_bignum, second_bignum, result);
			}
			else{
			result->digit = big_sub(second_bignum, first_bignum, result);
			result->is_neg = 1;
			}
		}
		else if (first_size>second_size){
			result->digit = big_sub(first_bignum, second_bignum, result);
		}
		else{
			result->digit = big_sub(second_bignum, first_bignum, result);
			result->is_neg = 1;
		}
	}
	else if (first_bignum->is_neg && second_bignum->is_neg){
		if (first_size>second_size){
			result->digit = big_add(second_bignum, first_bignum, result);
		}
		else{
			result->digit = big_add(first_bignum, second_bignum, result);
		}
		result->is_neg = 1;
	}
	else{
		if (first_size>second_size){
			result->digit= big_add(first_bignum, second_bignum, result);
		}
		else{
			result->digit= big_add(second_bignum, first_bignum, result);
		}
	}
	arrange_num(result);
	result->digit = realloc(result->digit, sizeof(char)*(result->num_of_digits+1));
	return result;
}

Bignum* sub_nums (Bignum* first_bignum, Bignum* second_bignum){
	long first_size = first_bignum->num_of_digits;
	long second_size = second_bignum->num_of_digits;
	int x = get_bigger_num(first_bignum, second_bignum);
	if (is_zero(first_bignum)){
		Bignum * result  = copy_num(second_bignum);
                if (second_bignum->is_neg){
                    result->is_neg = 0;
                }
                else {
                    result->is_neg = 1;
                }
		return result;
	}
	if (is_zero(second_bignum)){
		Bignum * result =  copy_num(first_bignum);
		return result;
	}
	if (x==0){
		Bignum * result = (Bignum*)malloc(sizeof(Bignum));
		result->is_neg = 0;
		result->num_of_digits = 1;
		result->digit = calloc (2, sizeof(char));
		result->digit[0] = '0';
		return result;
	}
	Bignum * result = (Bignum*)malloc(sizeof(Bignum));
	long max = get_max_size(first_size, second_size);
	max = max + 2;
	result->digit = (char*)calloc(max, sizeof(char));
	result->num_of_digits = max-1;
	if (!first_bignum->is_neg && second_bignum->is_neg){
		result->digit = big_add(first_bignum,second_bignum, result);
		result->is_neg = 0;
	}
	else if (first_bignum->is_neg && second_bignum->is_neg){
		if (x>-1){
			result->digit = big_sub(second_bignum, first_bignum, result);
			result->is_neg = 0;
		}
		else{
			result->digit = big_sub(first_bignum, second_bignum, result);
			result->is_neg = 1;
		}
	}
	else if (first_bignum->is_neg && !second_bignum->is_neg){
		result->digit = big_add(first_bignum, second_bignum, result);
		result->is_neg = 1;
	}
	else {
		if (x>-1){
			result->is_neg = 0;
			result->digit = big_sub (first_bignum, second_bignum, result);
		}
		else{
			result->is_neg = 1;
			result->digit = big_sub (second_bignum, first_bignum, result);
		}
	}
	arrange_num(result);
	result->digit = realloc(result->digit, result->num_of_digits+1);
	return result;    
}

Bignum * div_nums_rec (Bignum ** dividend, Bignum ** divisor, Bignum ** two_power, Bignum * answer){
	if(get_bigger_num(*dividend, *divisor) == -1){ // dividend < divisor
		return create_num();
	}
	else{ // dividend >= divisor
		Bignum * temp_divisor = add_nums(*divisor, *divisor);
		Bignum * temp_two_power = add_nums(*two_power, *two_power);
		answer = div_nums_rec(dividend, &temp_divisor,  &temp_two_power, answer);
		Bignum * temp_ans = sub_nums(*dividend, *divisor);
		if (!temp_ans->is_neg){
			
			Bignum * sum = add_nums(answer, *two_power);
			free((*dividend)->digit);
			(*dividend)->is_neg = 0;
			(*dividend)->num_of_digits = 0;
			free(*dividend);
			*dividend = copy_num(temp_ans);
			
			free_mem(temp_ans);
			free_mem(temp_divisor);
			free_mem(temp_two_power);
			Bignum * ret_val = copy_num(sum);
			free_mem(sum);
			free_mem(answer);
			return ret_val;
		}
		free_mem(temp_ans);
		free_mem(temp_divisor);
		free_mem(temp_two_power);
	}

	return answer;
}

Bignum * div_nums (Bignum ** dividend, Bignum ** divisor){
	int sign = 0;
	if (((*dividend)->is_neg && !(*divisor)->is_neg) || (!(*dividend)->is_neg && (*divisor)->is_neg)){
		sign = 1;
	}
	if (is_zero(*divisor)){
		printf("can't divide by 0\n");
		return NULL;
	}
	if (is_one(*divisor)){ // dividend / 1 = dividend
		Bignum * ret_val = copy_num(*dividend);
		if (!is_zero(ret_val)){
			ret_val->is_neg = sign;
		}
		return ret_val;
	}
	(*dividend)->is_neg = 0;
	(*divisor)->is_neg = 0;
	if(get_bigger_num(*dividend, *divisor)!=1){ // dividend <= divisor
		Bignum * ret_val = create_num();
		ret_val->digit[0] = '0';
		return ret_val;
	}
	Bignum * two_power = create_num();
	two_power->digit[0] = '1';
	Bignum * answer = create_num();
	Bignum * rec = div_nums_rec(dividend, divisor, &two_power, answer);
	rec->is_neg = sign;
	free_mem(two_power);
	free_mem(answer);
	return rec;
}

Bignum * divide_by_two(Bignum* dividend){
	Bignum * result = (Bignum*)malloc(sizeof(Bignum));
	result->digit = calloc(dividend->num_of_digits+2, sizeof(char));
	result->num_of_digits = dividend->num_of_digits;
	result->is_neg = dividend->is_neg;
        result->digit = div_by_two(dividend, result);
	arrange_num(result);
	return result;
}

Bignum * mult_nums_rec(Bignum * lhs, Bignum * rhs){
	Bignum * answer;
	if (strcmp(rhs->digit,"1")==0){
		if (rhs->digit[(rhs->num_of_digits)-1]%2==1){
			answer = copy_num(lhs);
		}
		else{
			answer = create_num();
		}
		return answer;
	}
	else {
			Bignum * rhs_div_by_two = divide_by_two(rhs);
			Bignum * lhs_times_two = add_nums(lhs,lhs);
			answer = mult_nums_rec(lhs_times_two, rhs_div_by_two);
			if (rhs->digit[(rhs->num_of_digits)-1]%2==1){
				
				Bignum * temp = add_nums(answer,lhs);
				free_mem(answer);
				answer = temp;
			}
			free_mem(rhs_div_by_two);
			free_mem(lhs_times_two);
			return answer;
	}
}

Bignum* mult_nums (Bignum* lhs, Bignum* rhs){
	if (lhs->num_of_digits<rhs->num_of_digits){
		Bignum *big = lhs;
		lhs = rhs;
		rhs = big;
	}
	int sign = (lhs->is_neg + rhs->is_neg)%2;
	lhs->is_neg = 0;
	rhs->is_neg = 0;
	Bignum * result;
	if (is_zero(lhs)||is_zero(rhs)){
		result = create_num();
		return result;
	}
	if (is_one(lhs)){
		result = copy_num(rhs);
		result->is_neg = sign;
		return result;
	}
	if (is_one(rhs)){
		result = copy_num(lhs);
		result->is_neg = sign;
		return result;
	}
	result = mult_nums_rec(lhs, rhs);
        result->is_neg = sign;
	return result;
}
int main(int argc, char **argv) {
	FILE *f = stdin;
	char *fs;
	char c;
	int len_of_num = 0, size = 1, neg = 0;
	fs = (char*)calloc(1,sizeof(char));
	Stack *s = NULL;
	while (1){
		c = fgetc(f);
		if (size==len_of_num)
			fs = realloc(fs, sizeof(char)*(size+=16));
		if (c == '_'){
			neg = 1;
		}
		else if (c>='0' && c<='9'){
			fs[len_of_num++] = c;
		}
		else{
			if (len_of_num>0){
				fs[len_of_num] = '\0';
				fs = realloc(fs, sizeof(char)*len_of_num+1);
				Bignum * curr_number = (Bignum*)malloc(sizeof(Bignum));
				curr_number->num_of_digits = len_of_num;
				curr_number->digit = fs;
				curr_number->is_neg = neg;
				_push (&s, curr_number);
				len_of_num = 0; 
				size = 1;
				neg = 0;
				fs = realloc (NULL, sizeof(char)*1);
			}
			if (c == '+'){
				if (!isEmpty(s)){
					Bignum * first_bignum = _pop(&s);
					if (!isEmpty(s)){
						Bignum * second_bignum = _pop(&s);
						Bignum * result = add_nums(second_bignum, first_bignum);
						_push(&s, result);
						free_mem(first_bignum);
						free_mem(second_bignum);
					}
					else{
						printf("empty stack!\n");
					}
				}
				else{
					printf("empty stack!\n");
				}
			}
			else if (c == '-'){
				if (!isEmpty(s)){
					Bignum * first_bignum = _pop(&s);
					if (!isEmpty(s)){
						Bignum * second_bignum = _pop(&s);
						Bignum * result = sub_nums(second_bignum, first_bignum);
						_push(&s, result);
						free_mem(first_bignum);
						free_mem(second_bignum);
					}
					else{
						printf("empty stack!\n");
					}
				}
				else{
					printf("empty stack!\n");
				}
			}
			else if (c == '*'){ 
				if (!isEmpty(s)){
					Bignum * first_bignum = _pop(&s);
					if (!isEmpty(s)){
						Bignum * second_bignum = _pop(&s);
						Bignum * result = mult_nums(first_bignum, second_bignum);
						_push(&s, result);
						free_mem(first_bignum);
						free_mem(second_bignum);
					}
					else{
						printf("empty stack!\n");
					}
				}
				else{
					printf("empty stack!\n");
				}
			}
			else if (c == '/'){ 
				if (!isEmpty(s)){
					Bignum * divisor = _pop(&s);
					if (!isEmpty(s)){
						Bignum * dividend = _pop(&s); // divisor
						Bignum * result = div_nums(&dividend, &divisor);
						if (result!=NULL){
							_push(&s, result);
						}
						free_mem(divisor);
						free_mem(dividend);
					}
					else{
						printf("empty stack!\n");
					}
				}
				else{
					printf("empty stack!\n");
				}
			}
			else if (c == 'p'){ 
				Bignum * to_print = peek(&s);
				if (to_print->is_neg) printf("-");
				printf("%s\n", to_print->digit);
			}
			else if (c == 'q'){ 
				clear_stack(&s);
				free(fs);
				free(s);
				return 0;
			}
			else if (c == 'c'){ 
				clear_stack(&s);
			}
			else if (c <= ' '){ // space and etc
				// do nothing
			}
			else{
				clear_stack(&s);
				free(fs);
				free(s);
				exit(-1);          
			}
		}
	}
	return 0;
}