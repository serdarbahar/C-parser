#ifndef MY_HEADER_H
#define MY_HEADER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

const int MAX_TOKEN;

char***** sentence_allocator();
void sentence_free(char***** sentences);
char**** action_allocator();
void action_condition_free(char**** actions);
char**** condition_allocator();
char** subjects_allocator();
void subjects_free(char** subjects);
char*** objects_allocator();
void objects_free(char*** objects);

struct Result {
    int exit;
    int isSentenceValid;
    int isQuestion;
    int isWhoAt;
    int isWhere;
    int isTotalItem;
    int isTotal;
    char***** sentences;
    char**** actions;
    char**** conditions;
    char** subjects;
    char*** objects;
    char** actionFromTo;
    char*** totalItemQuestion;
    char** subjectsForTotalItem;
    char* totalQuestion;
    char* whereQuestion;
    char* whoAtQuestion;
    void (*freeResult)(struct Result*);

    int sentenceCount;
};

struct Result* parsing();
void freeResult(struct Result* result);
int is_valid_digit_number(const char *str);
int is_curr_keyword(const char *str);
int is_curr_question_word(const char *str);

#endif /* MY_HEADER_H */
