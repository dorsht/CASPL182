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
state_result * init_state_handler_percent(va_list args, int * out_printed_chars, state_args* state){
        state_result *ret = malloc(sizeof(state_result));
        *(state->fs)++;
        ret->printed_chars = 0;
        ret->new_state = st_printf_percent;
        return ret;
}
state_result * init_state_handler_other(va_list args, int * out_printed_chars, state_args* state){
        state_result *ret = malloc(sizeof(state_result));
	putchar(*(state->fs));
        *(state->fs)++;
        ret->printed_chars = 1;
        ret->new_state = st_printf_init;
        return ret;
}
typedef state_result* (*state_result_init_handler_func)(va_list, int*, state_args*);
state_result * init_state_handler (va_list args, int * out_printed_chars, state_args* state){
    state_result_init_handler_func arr[128];
    int i;
    for (i=0; i<128; i++){
        arr[i] = init_state_handler_other;
    }
    arr[37] = init_state_handler_percent;
    return (arr[*(state)->fs](args, out_printed_chars, state));
}
typedef void* (*state_result_array_handler_func)(va_list, int*, state_args*);
void print_array_state_other(va_list args, int * out_printed_chars, state_args* state){
    // for default case
    toy_printf("Unhandled format %%%c...\n", *(state->fs));
    exit(-1);
}
void print_array_state_d(va_list args, int * out_printed_chars, state_args* state){
    void *arr;
    int arraySize;
    arr = va_arg(args, int*);
    arraySize = va_arg(args, int);
    *(state->fs)++;
    *out_printed_chars = print_array_helper(arr, 'd',arraySize);
}
void print_array_state_u(va_list args, int * out_printed_chars, state_args* state){
    void *arr;
    int arraySize;
    arr = va_arg(args, int*);
    arraySize = va_arg(args, int);
    *(state->fs)++;
    *out_printed_chars = print_array_helper(arr, 'u',arraySize);
}
void print_array_state_b(va_list args, int * out_printed_chars, state_args* state){
    void *arr;
    int arraySize;
    arr = va_arg(args, int*);
    arraySize = va_arg(args, int);
    *(state->fs)++;
    *out_printed_chars = print_array_helper(arr, 'b',arraySize);
}
void print_array_state_o(va_list args, int * out_printed_chars, state_args* state){
    void *arr;
    int arraySize;
    arr = va_arg(args, int*);
    arraySize = va_arg(args, int);
    *(state->fs)++;
    *out_printed_chars = print_array_helper(arr, 'o',arraySize);
}
void print_array_state_small_x(va_list args, int * out_printed_chars, state_args* state){
    void *arr;
    int arraySize;
    arr = va_arg(args, int*);
    arraySize = va_arg(args, int);
    *(state->fs)++;
   *out_printed_chars = print_array_helper(arr, 'x',arraySize);
}
void print_array_state_big_x(va_list args, int * out_printed_chars, state_args* state){
    void *arr;
    int arraySize;
    arr = va_arg(args, int*);
    arraySize = va_arg(args, int);
    *(state->fs)++;
   *out_printed_chars = print_array_helper(arr, 'X',arraySize);
}
void print_array_state_s(va_list args, int * out_printed_chars, state_args* state){
    void *arr;
    int arraySize;
    arr = va_arg(args, char**);
    arraySize = va_arg(args, int);
    *out_printed_chars = print_array_helper(arr, 's',arraySize);
    *(state->fs)++;
}
void print_array_state_c(va_list args, int * out_printed_chars, state_args* state){
    void *arr;
    int arraySize;
    arr = va_arg(args, int*);
    arraySize = va_arg(args, int);
   *out_printed_chars = print_array_helper(arr, 'c',arraySize);
    *(state->fs)++;
}
state_result * print_array_state_handler (va_list args, int * out_printed_chars, state_args* state){
    state_result *ret = malloc(sizeof(state_result));
    ret->new_state = st_printf_init;
    state_result_array_handler_func arr[128];
    int i;
    for (i=0; i<128; i++){
        arr[i] = print_array_state_other;
    }
    arr['b'] = print_array_state_b;
    arr['c'] = print_array_state_c;
    arr['d'] = print_array_state_d;
    arr['o'] = print_array_state_o;
    arr['s'] = print_array_state_s;
    arr['x'] = print_array_state_small_x;
    arr['X'] = print_array_state_big_x;
    *(state->fs)++;
    (arr[*(state)->fs](args, out_printed_chars, state));
    ret->printed_chars= *out_printed_chars;
    return ret;
}
typedef enum printf_state (*state_result_percent_handler_func)(va_list, int*, state_args*);
enum printf_state percent_state_other(va_list args, int * out_printed_chars, state_args* state){
    // default case
    char *string_value;
    int int_value;
    int printed=0;
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
            printed+=print_int_signed(int_value, 10, digit);
        }
        else{
            int toprint;
            if (int_value<0){
                putchar('-');
                printed++;
                toprint = num-size-1;
            }
            else{
                toprint = num-size;
            }
            for (int i=0; i<toprint; i++){
                putchar('0');
                printed++;
            }
            if(int_value<0) int_value=int_value*(-1);
            printed+=print_int_signed(int_value, 10, digit);
        }
        *(state->fs)++;
    }
    else if (*(state->fs) == 's'){
        string_value = va_arg(args, char *);
        char* temp = string_value;
        int size = 0;
        for (; *temp != '\0'; ++temp) {
            size++;
        }
        if (num<=size){
            printed+= toy_printf("%s",string_value);
        }
        else{
            if (!minus){
                printed+= toy_printf("%s",string_value);
                for (int i=0; i<num-size-1; i++){
                    putchar(' ');
                    printed++;
                }
                printed++;
                putchar('#');
            }
            else{
                for (int i=0; i<num-size; i++){
                    putchar(' ');
                    printed++;
                }
                printed+= toy_printf("%s",string_value);
            }
            *(state->fs)++;
        }
    }
    *out_printed_chars = printed;
    return st_printf_init;
    }
    else{
	toy_printf("Unhandled format %%%c...\n",*(state->fs));
	exit(-1);
    }
}
enum printf_state percent_state_percent(va_list args, int * out_printed_chars, state_args* state){
    putchar('%');
    *out_printed_chars = 1;
    *(state->fs)++;
    return st_printf_init;
}
enum printf_state percent_state_A(va_list args, int * out_printed_chars, state_args* state){
    return print_array;
}
enum printf_state percent_state_d(va_list args, int * out_printed_chars, state_args* state){
    int int_value = va_arg(args, int);
    *out_printed_chars= print_int_signed(int_value, 10, digit);
    *(state->fs)++;
    return st_printf_init;
}
enum printf_state percent_state_b(va_list args, int * out_printed_chars, state_args* state){
    int int_value = va_arg(args, int);
    *out_printed_chars= print_int(int_value, 2, digit);
    *(state->fs)++;
    return st_printf_init;
}
enum printf_state percent_state_o(va_list args, int * out_printed_chars, state_args* state){
    int int_value = va_arg(args, int);
    *out_printed_chars= print_int(int_value, 8, digit);
    *(state->fs)++;
    return st_printf_init;    
}
enum printf_state percent_state_u(va_list args, int * out_printed_chars, state_args* state){
    int int_value = va_arg(args, int);
    *out_printed_chars= print_int(int_value, 10, digit);
    *(state->fs)++;
    return st_printf_init;
}
enum printf_state percent_state_small_x(va_list args, int * out_printed_chars, state_args* state){
    int int_value = va_arg(args, int);
    *out_printed_chars= print_int(int_value, 16, digit);
    *(state->fs)++;
    return st_printf_init;
}
enum printf_state percent_state_big_x(va_list args, int * out_printed_chars, state_args* state){
    int int_value = va_arg(args, int);
    *out_printed_chars= print_int(int_value, 16, DIGIT);
    *(state->fs)++;
    return st_printf_init;
}
enum printf_state percent_state_s(va_list args, int * out_printed_chars, state_args* state){
    char *string_value = va_arg(args, char *);
    *out_printed_chars=0;
    while(*string_value){
        *out_printed_chars++;
        putchar(*string_value);
        string_value++;
    }
    *out_printed_chars++;
    *(state->fs)++;
    return st_printf_init;
}
enum printf_state percent_state_c(va_list args, int * out_printed_chars, state_args* state){
    char char_value = (char)va_arg(args, int);
    putchar(char_value);
    *out_printed_chars=1;
    *(state->fs)++;
    return st_printf_init;
}

state_result * percent_state_handler (va_list args, int * out_printed_chars, state_args* state){
    state_result * ret = malloc(sizeof(state_result));
    state_result_percent_handler_func arr[128];
    int i;
    for (i=0; i<128; i++){
        arr[i] = percent_state_other;
    }
    arr['%'] = percent_state_percent;
    arr['A'] = percent_state_A;
    arr['b'] = percent_state_b;
    arr['c'] = percent_state_c;
    arr['d'] = percent_state_d;
    arr['o'] = percent_state_o;
    arr['s'] = percent_state_s;
    arr['u'] = percent_state_u;
    arr['x'] = percent_state_small_x;
    arr['X'] = percent_state_big_x;
    ret->new_state = (arr[*(state)->fs](args, out_printed_chars, state));
    ret->printed_chars= *out_printed_chars;
    return ret;
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