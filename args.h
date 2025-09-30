/*

LICENSE:

    SPDX-License-Identifier: Zlib

    Copyright (c) 2025 Vlad Krupinskii <mrvladus@yandex.ru>

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

DESCRIPTION:

    args.h is a simple command-line parsing library designed for C/C++ projects.
    It consists of a single stb-style header file that you can copy into your project.

USAGE:

    // Define ARGS_IMPLEMENTATION in ONE file to include the functions implementation.
    #define ARGS_IMPLEMENTATION
    #include "args.h"

    #include <stdio.h>

    static void show_help_message() {
      printf("Usage: example [options]\n");
      printf("Options:\n");
      printf("  -h, --help, help                                 Show this help message\n");
      printf("  -i=<number>, --int=<number>, int=<number>        Print an integer\n");
      printf("  -f=<number>, --float=<number>, float=<number>    Print a floating point number\n");
      printf("  -s=<string>, --string=<string>, string=<string>  Print a string\n");
    }

    int main(int argc, char **argv) {
      args_parse(argc, argv);

      bool show_help = args_bool("-h|--help|help");
      int int_number = args_int("-i|--int|int");
      double float_number = args_float("-f|--float|float");
      const char *string = args_string("-s|--string|string");

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

*/

#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Parse command-line arguments.
// MUST be called before any other argument access functions.
void args_parse(int argc, char **argv);

// Print command-line arguments.
void args_print();

// Argument access functions.
// They all accept `arg` parameter in these formats:
// "-h", "--help", "help", or combine them as "-h|--help|help".
// "|" symbol is used as separator to define multiple variants for a single flag.

// Get boolean value of argument.
// It will parse flags like `--help`, `--debug <value>` or `--debug=<value>`
// where <value> is one of these (case insensitive):
// - true, false
// - on, off
// - yes, no
// - y, n
// - 1, 0
bool args_bool(const char *arg);

// Get integer value of argument.
// It will parse flags like `--port 8080` and `--port=8080`.
int args_int(const char *arg);

// Get floating point value of argument.
// It will parse flags like `--pi 3.14159` or `--pi=3.14159`.
double args_float(const char *arg);

// Get string value of argument.
// It will parse flags in formats:
//   --name John
//   --name=John
//   --name "John Smith"
//   --name="John Smith"
const char *args_string(const char *arg);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ARGS_H

#ifdef ARGS_IMPLEMENTATION

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int args__argc;
static char **args__argv;

void args_parse(int argc, char **argv) {
  args__argc = argc;
  args__argv = argv;
}

void args_print() {
  for (size_t i = 0; i < args__argc; ++i) printf("Argument %zu: %s\n", i, args__argv[i]);
}

bool args_bool(const char *arg) {
  static const char *true_values[] = {"true", "on", "yes", "y", "1"};
  static const char *false_values[] = {"false", "off", "no", "n", "0"};
  // Copy argument
  char arg_copy[strlen(arg) + 1];
  strcpy(arg_copy, arg);
  // Start parsing
  char *flag;
  flag = strtok(arg_copy, "|");
  while (flag) {
    for (size_t i = 1; i < args__argc; ++i) {
      // Check if arg is just a `--flag`
      if (!strcmp(args__argv[i], flag)) {
        // Check if next argument is in true_values or false_values
        if (i + 1 < args__argc) {
          // Check for true values
          const char *next_arg = args__argv[i + 1];
          for (size_t j = 0; j < sizeof(true_values) / sizeof(true_values[0]); ++j)
            if (!strcasecmp(next_arg, true_values[j])) return true;
          // Check for false values
          for (size_t j = 0; j < sizeof(false_values) / sizeof(false_values[0]); ++j)
            if (!strcasecmp(next_arg, false_values[j])) return false;
        }
        return true;
      }
      // Check if arg is `--flag=<value>`
      if (!strncmp(args__argv[i], flag, strlen(flag))) {
        const char *value = strchr(args__argv[i], '=');
        if (value) {
          value++; // Move past the '=' character
          // Check for true values
          for (size_t j = 0; j < sizeof(true_values) / sizeof(true_values[0]); j++)
            if (!strcmp(value, true_values[j])) return true;
          // Check for false values
          for (size_t j = 0; j < sizeof(false_values) / sizeof(false_values[0]); j++)
            if (!strcmp(value, false_values[j])) return false;
        }
      }
    }
    flag = strtok(NULL, "|");
  }
  return false;
}

int args_int(const char *arg) {
  // Copy argument
  char arg_copy[strlen(arg) + 1];
  strcpy(arg_copy, arg);
  // Start parsing
  int result = 0;
  char *flag;
  flag = strtok(arg_copy, "|");
  while (flag) {
    for (size_t i = 1; i < args__argc; ++i) {
      // Check if arg is `--flag`
      if (!strcmp(args__argv[i], flag)) {
        // Check if next argument is number
        if (i + 1 < args__argc && isdigit(args__argv[i + 1][0])) result = atoi(args__argv[i + 1]);
      } else {
        // Check if arg is `--flag=<value>`
        if (!strncmp(args__argv[i], flag, strlen(flag))) {
          const char *value = strchr(args__argv[i], '=');
          if (value) {
            value++; // Move past the '=' character
            result = atoi(value);
          }
        }
      }
    }
    flag = strtok(NULL, "|");
  }
  return result;
}

double args_float(const char *arg) {
  // Copy argument
  char arg_copy[strlen(arg) + 1];
  strcpy(arg_copy, arg);
  // Start parsing
  double result = 0;
  char *flag;
  flag = strtok(arg_copy, "|");
  while (flag) {
    for (size_t i = 1; i < args__argc; ++i)
      // Check if arg is `--flag`
      if (!strcmp(args__argv[i], flag)) {
        // Check if next argument is number
        if (i + 1 < args__argc && isdigit(args__argv[i + 1][0])) result = atof(args__argv[i + 1]);
      } else {
        // Check if arg is `--flag=<value>`
        if (!strncmp(args__argv[i], flag, strlen(flag))) {
          const char *value = strchr(args__argv[i], '=');
          if (value) {
            value++; // Move past the '=' character
            result = atof(value);
          }
        }
      }
    flag = strtok(NULL, "|");
  }
  return result;
}

const char *args_string(const char *arg) {
  // Copy argument
  char arg_copy[strlen(arg) + 1];
  strcpy(arg_copy, arg);
  // Start parsing
  const char *result = NULL;
  char *flag;
  flag = strtok(arg_copy, "|");
  while (flag) {
    for (size_t i = 1; i < args__argc; ++i) {
      // Check if arg is `--flag <value>` and has next arg
      if (!strcmp(args__argv[i], flag) && i + 1 < args__argc) result = args__argv[i + 1];
      else {
        // Check if arg is `--flag=value`
        if (!strncmp(args__argv[i], flag, strlen(flag))) {
          char *value = strchr(args__argv[i], '=');
          if (value) {
            value++; // Move past the '=' character
            // Value is `--flag="multi word value"`
            if (value[0] == '"') {
              value++; // Move past the '"' character
              const char *start = value;
              result = start;
              while (!(*value == '"' || *value == '\0')) value++;
              *value = '\0';
            }
            // Value is `--flag=value`
            else
              result = value;
          }
        }
      }
    }
    flag = strtok(NULL, "|");
  }
  return result;
}

#endif // ARGS_IMPLEMENTATION
