
#ifndef MBR_TEST_H
#define MBR_TEST_H
#include <assert.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
/*
    int devh = 1;
    if (devh == -1) {
        fprintf(stderr, "Device [ " RED "FAILED" RESET " ]\n");
    } else {
        fprintf(stdout, "Device [ " GREEN "OK" RESET " ]\n");
    }
 */
#include <stdio.h>
#define STR(x) #x
#define PRINT(expr) (fprintf(stdout, "%s ... ", STR(expr)))
#define check(expr) (expr)? (PRINT(expr) , fprintf(stdout, ""GREEN"%s\n"RESET"", "PASSED")): \
                            (PRINT(expr) , fprintf(stdout, ""RED"%s\n"RESET"",   "FAILED"), assert(expr))
#endif //MBR_TEST_H
