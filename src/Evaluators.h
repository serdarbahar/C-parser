#ifndef INTERPRETER_PROJECT_EVALUATORS_H
#define INTERPRETER_PROJECT_EVALUATORS_H

#include "Structs.h"

typedef struct {
    int quantity;
    char* name;
} ItemWithQuantity;

/*
    * Multipurpose argument bundle for actions.
     * Only the variables that will be needed to call action will be initialized at the time of the action.
     * For example, if the action is "buy", only personChain1 and itemChain will be initialized.
     * Only if 2 people are involved, personChain2 will be initialized.
*/
typedef struct {
    char* actionType;

    char** personChain1; // list of names of people
    int personChain1Size;
    char** personChain2;
    int personChain2Size;

    ItemWithQuantity** itemChain;
    int itemChainSize;

    char* place; // name of place
} actionArgs;

typedef struct {
    char* conditionType;

    char** personChain; // list of names of people
    int personChainSize;

    ItemWithQuantity** itemChain;
    int itemChainSize;

    char* place; // name of place
} conditionArgs;

typedef struct {
    char* questionType;

    struct Person* person; // used in all questions except totalItem (because they only involve one person)

    struct Person** personChain; // only used in totalItem question
    int personChainSize;

    char* itemName;

    struct Place* place;
} questionArgs;

int actionEvaluator(actionArgs* args, struct People* people, struct PlacesList* places);
int conditionEvaluator(conditionArgs* args, struct People* people, struct PlacesList* places);
char* questionEvaluator(questionArgs* args);

#endif //INTERPRETER_PROJECT_EVALUATORS_H
