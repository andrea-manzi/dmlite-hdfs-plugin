#ifndef UTILS_H
#define	UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned TEST_FAILURES;

/* Macros */

#define SECTION(msg) log_section(msg);

#define FILE_NAME (strrchr(__FILE__, '/') + 1)

#define TEST_ASSERT_WITH_LOG(condition, file, line)\


#define TEST_ASSERT(condition)\
if (!(condition)) {\
  ++TEST_FAILURES;\
  log_failure("Assert '%s' failed (%s:%d)", #condition, FILE_NAME, __LINE__);\
} else {\
  log_success("%s", #condition);\
}

#define TEST_ASSERT_NOT_EQUAL(a, b)\
if (a == b) {\
  ++TEST_FAILURES;\
  log_failure("Expected %s != %ld (%s:%d)", #b, (long)a, FILE_NAME, __LINE__);\
} else {\
  log_success("%s != %ld", #b, (long)a);\
}

#define TEST_ASSERT_EQUAL(a, b)\
if (a != b) {\
  ++TEST_FAILURES;\
  log_failure("Expected %s == %ld, got %ld (%s:%d)", #b, (long)a, (long)b,\
              FILE_NAME, __LINE__);\
} else {\
  log_success("%s == %ld", #b, (long)a);\
}

#define TEST_ASSERT_STR_EQUAL(a, b)\
if (strcmp(a, b) != 0) {\
  ++TEST_FAILURES;\
  log_failure("Expected %s == '%s', got '%s' (%s:%d)", #b, a, b,\
              FILE_NAME, __LINE__);\
} else {\
  log_success("%s == '%s'", #b, a);\
}

#define TEST_MANAGER_CALL(manager, call, ...)\
if (call(manager, __VA_ARGS__) != 0){\
  ++TEST_FAILURES;\
  log_failure("%s with %d - %s (%s:%d)",\
              #call, dmlite_manager_errno(manager), dmlite_manager_error(manager),\
              FILE_NAME, __LINE__);\
}\
else {\
  log_success("%s", #call);\
}

#define TEST_CONTEXT_CALL(ctx, call, ...)\
if (call(ctx, __VA_ARGS__) != 0){\
  ++TEST_FAILURES;\
  log_failure("%s with %d - %s (%s:%d)",\
              #call, dmlite_errno(ctx), dmlite_error(ctx),\
              FILE_NAME, __LINE__);\
}\
else {\
  log_success("%s", #call);\
}

#define TEST_CONTEXT_CALL_PTR(dst, ctx, call, ...)\
dst = call(ctx, __VA_ARGS__);\
if (dst == NULL){\
  ++TEST_FAILURES;\
  log_failure("%s with %d - %s (%s:%d)",\
              #call, dmlite_errno(ctx), dmlite_error(ctx),\
              FILE_NAME, __LINE__);\
}\
else {\
  log_success("%s", #call);\
}

/* Extern methods */
#ifdef __cplusplus
extern "C" {
#endif
void log_section(const char*);
void log_failure(const char* fmt, ...);
void log_success(const char* fmt, ...);
void log_dump(const char* fmt, ...);
#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */
