#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

/* Formats */
#define SECTION_FMT       "- \e[1;36m%s\e[0m\n"
#define SECTION_FMT_PLAIN "- %s\n"

#define PASS_STR       "\t\e[1;32m[PASSED]\e[0m"
#define PASS_STR_PLAIN "\t[PASSED]"
#define FAIL_STR       "\t\e[1;31m[FAILED]\e[0m"
#define FAIL_STR_PLAIN "\t[FAILED]"
#define DUMP_STR       "\t\e[1;33m[ DUMP ]\e[0m"
#define DUMP_STR_PLAIN "\t[ DUMP ]"

/* Globals */
unsigned TEST_FAILURES = 0;


/* Methods */

void log_section(const char* str)
{
  if (isatty(fileno(stdout)))
    printf(SECTION_FMT, str);
  else
    printf(SECTION_FMT_PLAIN, str);
}



void log_failure(const char* fmt, ...)
{
  if (isatty(fileno(stdout)))
    fputs(FAIL_STR, stdout);
  else
    fputs(FAIL_STR_PLAIN, stdout);
  
  fputc(' ', stdout);
  
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  
  fputc('\n', stdout);
}



void log_success(const char* fmt, ...)
{
  if (isatty(fileno(stdout)))
    fputs(PASS_STR, stdout);
  else
    fputs(PASS_STR_PLAIN, stdout);
  
  fputc(' ', stdout);
  
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  
  fputc('\n', stdout);
}



void log_dump(const char* fmt, ...)
{
  if (isatty(fileno(stdout)))
    fputs(DUMP_STR, stdout);
  else
    fputs(DUMP_STR_PLAIN, stdout);
  
  fputc(' ', stdout);
  
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  
  fputc('\n', stdout);
}
