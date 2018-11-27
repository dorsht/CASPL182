/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* the states in the printf state-machine */
enum printf_state {
  st_printf_init,
  st_printf_percent,
  st_printf_octal2,
  st_printf_octal3,
  print_array
};

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

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
            putchar(' ');
            printed+=2;
       }
        printed+=print_int_signed(array[size-1], 10, digit);
    }
    else if (type=='c'){
        for (int i=0; i<size-1; i++){
            char c = (char)array[i];
            putchar(c);
            putchar(',');
            putchar(' ');
            printed+=3;
        }
        putchar((char)array[size-1]);
        printed++;
    }
    else if (type=='s'){
        for (int i=0; i<size-1; i++){
            char *temp = stringArray[i];
            printed+=toy_printf("%s, ",temp);
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
            putchar(' ');
            printed+=2;
        }
        if (bigDigits)
            printed+=print_int(array[size-1], radix, digit);
        else printed+=print_int(array[size-1], radix, DIGIT);       
    }
    putchar('}');
    return printed;
}
/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
  int chars_printed = 0;
  int int_value = 0;
  char *string_value;
  char char_value;
  char octal_char;
  va_list args;
  enum printf_state state;
  void *arr;
  int arraySize;

  va_start(args, fs);

  state = st_printf_init; 

  for (; *fs != '\0'; ++fs) {
    switch (state) {
    case st_printf_init:
      switch (*fs) {
      case '%':
	state = st_printf_percent;
	break;

      default:
	putchar(*fs);
	++chars_printed;
      }
      break;

    case st_printf_percent:
      switch (*fs) {
      case '%':
	putchar('%');
	++chars_printed;
	state = st_printf_init;
	break;
        
      case 'A':
          state = print_array;
          break;
      case 'd':
	int_value = va_arg(args, int);
	chars_printed += print_int_signed(int_value, 10, digit);
	state = st_printf_init;
	break;

      case 'b':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 2, digit);
	state = st_printf_init;
	break;

      case 'o':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 8, digit);
	state = st_printf_init;
	break;
        
      case 'u':
        int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 10, digit);
	state = st_printf_init;
	break;  
	
      case 'x':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 16, digit);
	state = st_printf_init;
	break;

      case 'X':
	int_value = va_arg(args, int);
	chars_printed += print_int(int_value, 16, DIGIT);
	state = st_printf_init;
	break;
        
      case 's':
	string_value = va_arg(args, char *);
	while(*string_value){
		chars_printed++;
		putchar(*string_value);
		string_value++;
	}
	state = st_printf_init;
	break;

      case 'c':
	char_value = (char)va_arg(args, int);
	putchar(char_value);
	++chars_printed;
	state = st_printf_init;
	break;

      default:
        
        if (*fs=='-' || (*fs>='0' && *fs<='9')){
            int minus=0, num=0;
            if (*fs=='-'){
                minus = 1;
                fs++;
            }
            while (*fs>='0' && *fs <='9'){
                num = num*10 + (*fs-'0');
                fs++;
            }
            if (*fs == 'd'){
                int_value = va_arg(args, int);
                int size = minus, temp = int_value;
                while (temp!=0){
                    size++;
                    temp = temp/10;
                }
                if (num<=size){
                    chars_printed+=print_int_signed(int_value, 10, digit);
                }
                else{
                    int toprint;
                   if (int_value<0){
                       putchar('-');
                       chars_printed++;
                       toprint = num-size-1;
                   }
                   else toprint = num-size;
                       for (int i=0; i<toprint; i++){
                           putchar('0');
                           chars_printed++;
                       }
                    if(int_value<0) int_value=int_value*(-1);
                    chars_printed+=print_int_signed(int_value, 10, digit);
                }
                state = st_printf_init;
                break;
            }
            else if (*fs == 's')
            {
                string_value = va_arg(args, char *);
                char* temp = string_value;
                int size = 0;
                for (; *temp != '\0'; ++temp) {
                    size++;
                }
                if (num<=size)
                    chars_printed += toy_printf("%s",string_value);
                else{
                    if (!minus){
                        chars_printed += toy_printf("%s",string_value);
                        for (int i=0; i<num-size-1; i++){
                            putchar(' ');
                            chars_printed++;
                        }
                        chars_printed++;
                        putchar('#');
                    }
                    else{
                        for (int i=0; i<num-size; i++){
                            putchar(' ');
                            chars_printed++;
                        }
                        chars_printed += toy_printf("%s",string_value);
                }
                state = st_printf_init;
                break;
            }
        }
        }
        else{
	toy_printf("Unhandled format %%%c...\n", *fs);
	exit(-1);
        }
      }
      break;
      case print_array:
          switch(*fs){
              case 'd':
                  arr = va_arg(args, int*);
                  arraySize = va_arg(args, int);
                  chars_printed+=print_array_helper(arr, 'd',arraySize);
                  state = st_printf_init;
                  break;
              case 'u':
                arr = va_arg(args, int*);
                  arraySize = va_arg(args, int);
                  chars_printed+=print_array_helper(arr, 'u',arraySize);
                  state = st_printf_init;
                  break; 
              case 'b':
                arr = va_arg(args, int*);
                  arraySize = va_arg(args, int);
                  chars_printed+=print_array_helper(arr, 'b',arraySize);
                  state = st_printf_init;
                  break;
              case 'o':
                arr = va_arg(args, int*);
                  arraySize = va_arg(args, int);
                  chars_printed+=print_array_helper(arr, 'o',arraySize);
                  state = st_printf_init;
                  break;
              case 'x':
                arr = va_arg(args, int*);
                  arraySize = va_arg(args, int);
                  chars_printed+=print_array_helper(arr, 'x',arraySize);
                  state = st_printf_init;
                  break;
              case 'X':
                arr = va_arg(args, int*);
                  arraySize = va_arg(args, int);
                  chars_printed+=print_array_helper(arr, 'X',arraySize);
                  state = st_printf_init;
                  break;
                  
              case 's':
                arr = va_arg(args, char**);
                  arraySize = va_arg(args, int);
                  chars_printed+=print_array_helper(arr, 's',arraySize);
                  state = st_printf_init;
                  break;
                  
              case 'c':
                arr = va_arg(args, int*);
                  arraySize = va_arg(args, int);
                  chars_printed+=print_array_helper(arr, 'c',arraySize);
                  state = st_printf_init;
                  break;
                  
            default:
                toy_printf("Unhandled format %%%c...\n", *fs);
                exit(-1);
      }break;
          
    default:
      toy_printf("toy_printf: Unknown state -- %d\n", (int)state);
      exit(-1);
    }
  }

  va_end(args);

  return chars_printed;
}