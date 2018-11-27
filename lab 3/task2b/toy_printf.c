/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Dor Shtarker, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* the states in the printf state-machine */
enum printf_state {
  st_printf_init,
  st_printf_percent,
  print_array
};

typedef struct{
    char *fs;
    int * printed_chars;
} state_args;
typedef struct{
  int printed_chars;
  enum printf_state new_state;
}state_result;
int toy_printf(char *fs, ...);


const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int print_int_helper_signed(int n, int radix, const char *digit) {
  int result;

  if (n < radix) {
    putchar(digit[n]);
    return 1;
  }
  else {
    result = print_int_helper_signed(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}

int print_int_signed(int n, int radix, const char * digit) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

  if (n > 0) {
    return print_int_helper_signed(n, radix, digit);
  }
  if (n == 0) {
    putchar('0');
    return 1;
  }
  else {
    putchar('-');
    return 1 + print_int_helper_signed(-n, radix, digit);
  }
}
int print_int_helper(unsigned int n, int radix, const char *digit) {
  int result;

  if (n < radix) {
    putchar(digit[n]);
    return 1;
  }
  else {
    result = print_int_helper(n / radix, radix, digit);
    putchar(digit[n % radix]);
    return 1 + result;
  }
}

int print_int(unsigned int n, int radix, const char * digit) {
  if (radix < 2 || radix > 16) {
    toy_printf("Radix must be in [2..16]: Not %d\n", radix);
    exit(-1);
  }

  if (n > 0) {
    return print_int_helper(n, radix, digit);
  }
  if (n == 0) {
    putchar('0');
    return 1;
  }
  else {
    putchar('-');
    return 1 + print_int_helper(-n, radix, digit);
  }
}
int print_array_helper (void *arr, char type, int size){
    int printed = 2; // initiallized for 2 because the { and the }
    int *array;
    char **stringArray;
    if (type!='s') array = arr;
    else stringArray = arr;
    putchar('{');
    if (type == 'd'){
       for (int i=0; i<size-1; i++){
           printed+=print_int_signed(array[i], 10, digit);
            putchar(',');
            printed++;
       }
        printed+=print_int_signed(array[size-1], 10, digit);
    }
    else if (type=='c'){
        for (int i=0; i<size-1; i++){
            char c = (char)array[i];
            putchar(c);
            putchar(',');
            printed+=2;
        }
        putchar((char)array[size-1]);
        printed++;
    }
    else if (type=='s'){
        for (int i=0; i<size-1; i++){
            char *temp = stringArray[i];
            printed+=toy_printf("%s,",temp);
        }
        printed+=toy_printf("%s", stringArray[size-1]);
    }
    else{
        int radix;
        int bigDigits=0;
        switch (type){
            case 'u':
                radix = 10;
                break;
            case 'b':
                radix = 2;
                break;
            case 'o':
                radix = 8;
                break;
            case 'x':
                radix = 16;
                break;
            case 'X':
                radix = 16;
                bigDigits = 1;
                break;
            default:
                break;
        }
        for (int i=0; i<size-1; i++){
            if (bigDigits)
                printed+=print_int(array[i], radix, digit);
            else printed+=print_int(array[i], radix, DIGIT);
            putchar(',');
            printed++;
        }
        if (bigDigits)
            printed+=print_int(array[size-1], radix, digit);
        else printed+=print_int(array[size-1], radix, DIGIT);       
    }
    putchar('}');
    return printed;
}
state_result * init_state_handler (va_list args, int * out_printed_chars, state_args* state){
    state_result *ret = malloc(sizeof(state_result));
    switch (*(state->fs)){
        case '%':
        *(state->fs)++;
        ret->printed_chars = 0;
        ret->new_state = st_printf_percent;
        return ret;

      default:
	putchar(*(state->fs));
        *(state->fs)++;
        ret->printed_chars = 1;
        ret->new_state = st_printf_init;
        return ret;
    }
}
state_result * print_array_state_handler (va_list args, int * out_printed_chars, state_args* state){
    state_result *ret = malloc(sizeof(state_result));
    ret->new_state = st_printf_init;
    void *arr;
    int arraySize;
    *(state->fs)++;
    switch(*(state->fs)){
        case 'd':
            arr = va_arg(args, int*);
            arraySize = va_arg(args, int);
            *(state->fs)++;
            ret->printed_chars = print_array_helper(arr, 'd',arraySize);
            return ret;

        case 'u':
            arr = va_arg(args, int*);
            arraySize = va_arg(args, int);
            ret->printed_chars = print_array_helper(arr, 'u',arraySize);
            *(state->fs)++;
            return ret;

        case 'b':
            arr = va_arg(args, int*);
            arraySize = va_arg(args, int);
            ret->printed_chars = print_array_helper(arr, 'b',arraySize);
            *(state->fs)++;
            return ret;

        case 'o':
            arr = va_arg(args, int*);
            arraySize = va_arg(args, int);
            ret->printed_chars = print_array_helper(arr, 'o',arraySize);
            *(state->fs)++;
            return ret;

        case 'x':
            arr = va_arg(args, int*);
            arraySize = va_arg(args, int);
            ret->printed_chars = print_array_helper(arr, 'x',arraySize);
            *(state->fs)++;
            return ret;

        case 'X':
            arr = va_arg(args, int*);
            arraySize = va_arg(args, int);
            ret->printed_chars = print_array_helper(arr, 'X',arraySize);
            *(state->fs)++;
            return ret;
                  
        case 's':
            arr = va_arg(args, char**);
            arraySize = va_arg(args, int);
            ret->printed_chars = print_array_helper(arr, 's',arraySize);
            *(state->fs)++;
            return ret;
            
        case 'c':
            arr = va_arg(args, int*);
            arraySize = va_arg(args, int);
            ret->printed_chars = print_array_helper(arr, 'c',arraySize);
            *(state->fs)++;
            return ret;

        default:
            toy_printf("Unhandled format %%%c...\n", *(state->fs));
            exit(-1);
      }
}
state_result * percent_state_handler (va_list args, int * out_printed_chars, state_args* state){
    int int_value = 0;
    char *string_value;
    char char_value;
    char octal_char;
    state_result * ret = malloc(sizeof(state_result));
    ret->new_state = st_printf_init;
    switch (*(state->fs)){
      case '%':
	putchar('%');
        ret->printed_chars=0;
        *(state->fs)++;
        return ret;
        
      case 'A':
          ret->new_state = print_array;
          return ret;
      case 'd':
	int_value = va_arg(args, int);
        ret->printed_chars= print_int_signed(int_value, 10, digit);
        *(state->fs)++;
        return ret;
        
      case 'b':
	int_value = va_arg(args, int);
	ret->printed_chars= print_int(int_value, 2, digit);
        *(state->fs)++;
        return ret;
        
      case 'o':
	int_value = va_arg(args, int);
	ret->printed_chars= print_int(int_value, 8, digit);
        *(state->fs)++;
        return ret;
        
      case 'u':
        int_value = va_arg(args, int);
	ret->printed_chars= print_int(int_value, 10, digit);
        *(state->fs)++;
	return ret;
	
      case 'x':
	int_value = va_arg(args, int);
	ret->printed_chars= print_int(int_value, 16, digit);
        *(state->fs)++;
	return ret;

      case 'X':
	int_value = va_arg(args, int);
	ret->printed_chars= print_int(int_value, 16, DIGIT);
        *(state->fs)++;
	return ret;
        
      case 's':
	string_value = va_arg(args, char *);
        ret->printed_chars=0;
	while(*string_value){
            ret->printed_chars++;
            putchar(*string_value);
            string_value++;
	}
        ret->printed_chars++;
        *(state->fs)++;
	return ret;

      case 'c':
	char_value = (char)va_arg(args, int);
	putchar(char_value);
        ret->printed_chars=1;
        *(state->fs)++;
        return ret;

      default:
        if (*(state->fs)=='-' || (*(state->fs)>='0' && *(state->fs)<='9')){
            int minus=0, num=0;
            if (*(state->fs)=='-'){
                minus = 1;
                *(state->fs)++;
            }
            while (*(state->fs)>='0' && *(state->fs) <='9'){
                num = num*10 + (*(state->fs)-'0');
                *(state->fs)++;
            }
            if (*(state->fs) == 'd'){
                int_value = va_arg(args, int);
                int size = minus, temp = int_value;
                while (temp!=0){
                    size++;
                    temp = temp/10;
                }
                if (num<=size){
                   ret->printed_chars=print_int_signed(int_value, 10, digit);
                }
                else{
                    int toprint;
                    if (int_value<0){
                       putchar('-');
                       *(out_printed_chars)++;
                       toprint = num-size-1;
                   }
                   else toprint = num-size;
                       for (int i=0; i<toprint; i++){
                           putchar('0');
                           *(out_printed_chars)++;
                    }
                    if(int_value<0) int_value=int_value*(-1);
                    ret->printed_chars=print_int_signed(int_value, 10, digit);
                }
                *(state->fs)++;
                return ret;
            }
            else if (*(state->fs) == 's')
            {
                string_value = va_arg(args, char *);
                char* temp = string_value;
                int size = 0;
                for (; *temp != '\0'; ++temp) {
                    size++;
                }
                if (num<=size)
                    ret->printed_chars= toy_printf("%s",string_value);
                else{
                    if (!minus){
                        ret->printed_chars= toy_printf("%s",string_value);
                        for (int i=0; i<num-size-1; i++){
                            putchar(' ');
                            ret->printed_chars++;
                        }
                        ret->printed_chars++;
                        putchar('#');
                    }
                    else{
                        for (int i=0; i<num-size; i++){
                            putchar(' ');
                            ret->printed_chars++;
                        }
                        ret->printed_chars+= toy_printf("%s",string_value);
                }
                *(state->fs)++;
                return ret;
            }
        }
    }
    else{
	toy_printf("Unhandled format %%%c...\n",*(state->fs));
	exit(-1);
        }
    }
}
#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')



/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */
typedef state_result* (*state_result_func)(va_list, int*, state_args*);
state_result_func arr[3] = {init_state_handler, percent_state_handler, print_array_state_handler};
int toy_printf(char *fs, ...) {
    int * chars_printed = malloc(sizeof(int));
    *chars_printed = 0;
    int printed = 0;
    va_list args;
    va_start(args, fs);
    state_result *res = malloc(sizeof(state_result));
    res->printed_chars = 0;
    res->new_state = st_printf_init;
    state_args state_args ;
    state_args.fs =  fs;
    state_args.printed_chars = 0;
    state_result * temp;
    for(; *fs !='\0'; ++fs){
        temp =(arr[res->new_state](args, chars_printed, &state_args));
        free(res);
        res = temp;
        printed+=res->printed_chars;
    }
    free(chars_printed);
    free(res);
    va_end(args);
    return printed;
}