// cc -o example example.c
// ./example --int 69420 --float 3.14 --string "Hello, World!"

#define ARGS_IMPLEMENTATION
#include "args.h"

#include <stdio.h>

static void show_help_message() {
  printf("Usage: example [options]\n");
  printf("Options:\n");
  printf("  -h, --help                      Show this help message\n");
  printf("  -i <number>, --int <number>     Print an integer\n");
  printf("  -f <number>, --float <number>   Print a floating point number\n");
  printf("  -s <string>, --string <string>  Print a string\n");
}

int main(int argc, char **argv) {
  args_parse(argc, argv);

  bool show_help = args_bool("-h|--help");
  int int_number = args_int("-i|--int");
  double float_number = args_float("-f|--float");
  const char *string = args_string("-s|--string");

  if (show_help) {
    show_help_message();
    return 0;
  }

  if (int_number || float_number || string) {
    if (int_number) printf("Int: %d\n", int_number);
    if (float_number) printf("Float: %f\n", float_number);
    if (string) printf("String: %s\n", string);
    return 0;
  }

  return 0;
}
