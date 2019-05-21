#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED
typedef enum {
    info,
    warning,
    fatal
} log_class_t;
int printl(log_class_t, const char *, ...);
#endif
