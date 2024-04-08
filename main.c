#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Structs.h"
#include "parsing.h"
#include "Evaluators.h"

#define boolean int

// TODO: "if <condition>" (without a start) should not be invalid (?)


int main() {

    struct People *people = initializePeople(20);
    struct PlacesList *places = initializePlacesList(20);

    while (1) {

        struct Result* input = parsing();

        if (input->exit)
            break;

        char**** actions = input->actions;

        if (!input->isSentenceValid) {
            printf("INVALID\n");
            input->freeResult(input);
            continue;
        }

        // TODO: buyer and seller cannot be the same invalid check

        if (input->isQuestion) {
            questionArgs args;

            // loading input

            if (input->isWhoAt) {
                args.questionType = "who at";
                char* placeName = input->whoAtQuestion;
                args.place = places->getPlace(places,placeName);
            }
            else if (input->isTotal) {
                args.questionType = "total";
                char* personName = input->totalQuestion;
                args.person = people->getPerson(people,personName);
            }
            else if (input->isTotalItem) {
                args.questionType = "total item";
                args.itemName = *input->totalItemQuestion[2];

                struct Person** personList = calloc(1024, sizeof(struct Person*));
                char** subjects = input->subjectsForTotalItem;

                int personListSize = 0;
                for (int k = 0; subjects[k] != NULL; k++)
                    personListSize++;

                for (int k = 0; k < personListSize; k++)
                    personList[k] = people->getPerson(people, subjects[k]);

                args.personChain = personList;
                args.personChainSize = personListSize;
            }
            else if (input->isWhere) {
                args.questionType = "where";
                char* personName = input->whereQuestion;
                args.person = people->getPerson(people,personName);
            }

            // getting output

            char* outputString = questionEvaluator(&args);
            printf("%s\n",outputString);
            continue;
        }

        else { // input is not a question

            int sentenceCount = input->sentenceCount;

            int actionCount = 0;
            int conditionCount = 0;
            int objectCount = 0;

            for (int i = 0; i<sentenceCount; i++) {

                char**** currSentence = input->sentences[i];

                // ACTIONS

                boolean conditionResult = 1;

                // finding the number of actions for the current sentence
                int numOfActionsforCurrSentence = 0;
                while (1) {
                    if (actions[actionCount + numOfActionsforCurrSentence] == currSentence[1]) {
                        numOfActionsforCurrSentence++;
                        break;
                    }
                    numOfActionsforCurrSentence++;
                }



                actionArgs** actionArgumentsList = calloc(numOfActionsforCurrSentence, sizeof(actionArgs*));
                int actionArgumentIndex = 0;

                // LOADING INPUT

                while(1) {

                    actionArgs* actionArguments =  malloc(sizeof(actionArgs));

                    char ***currAction = actions[actionCount];

                    char *actionType;
                    if (strcmp(*currAction[2], "buy") == 0) {
                        if (**currAction[3] == '\0')
                            actionType = "buy";
                        else
                            actionType = "buy from";
                    } else if (strcmp(*currAction[2], "sell") == 0) {
                        if (**currAction[3] == '\0')
                            actionType = "sell";
                        else
                            actionType = "sell to";
                    } else
                        actionType = "go to";

                    actionArguments->actionType = actionType;

                    //subject chain
                    int subjectsChainSize = 0;
                    for (int k = 0; currAction[0][k] <= currAction[1][0] && currAction[0][k] != NULL; k++) {
                        subjectsChainSize++;
                    }

                    char** subjectChain = calloc(subjectsChainSize, sizeof(char*));
                    for (int k = 0; currAction[0][k] <= currAction[1][0] && currAction[0][k] != NULL; k++) {
                        subjectChain[k] = currAction[0][k];
                    }
                    actionArguments->personChain1 = subjectChain;
                    actionArguments->personChain1Size = subjectsChainSize;

                    int itemChainSize = 0;

                    if (strcmp(actionType, "sell to") == 0 || strcmp(actionType, "buy from") == 0) {
                        actionArguments->personChain2Size = 1;
                    }
                    actionArguments->personChain2 = currAction[3];

                    ItemWithQuantity **items = malloc(1024 * sizeof(ItemWithQuantity *));
                    //item chain
                    if (strcmp(actionType, "go to") == 0) {
                        actionArguments->place = currAction[4][0];
                        objectCount++;
                    } else {

                        int k = 0;
                        while (1) {
                            ItemWithQuantity *item = malloc(sizeof(ItemWithQuantity));
                            item->name = input->objects[objectCount][1];
                            item->quantity = atoi(input->objects[objectCount][0]);
                            items[k] = item;

                            if (input->objects[objectCount][0] == currAction[5][0] &&
                                input->objects[objectCount][1] == currAction[5][1]) {
                                objectCount++;
                                itemChainSize++;
                                break;

                            }

                            itemChainSize++;
                            k++;
                            objectCount++;

                        }
                        actionArguments->itemChainSize = itemChainSize;
                    }
                    actionArguments->itemChain = items;

                    actionArgumentsList[actionArgumentIndex++] = actionArguments;


                    if (actions[actionCount] == currSentence[1]) {
                        actionCount++;
                        break;
                    }

                    actionCount++;

                }
                // END OF LOADING INPUT
                // actions will be evaluated if the conditions (evaluated below) are met


                // CONDITIONS

                boolean conditionsExist = currSentence[2][0][0] != NULL;

                if (conditionsExist) { // true if conditions exist

                    // all conditions must be met for the actions to be executed
                    while (1) {

                        char ***currCondition = input->conditions[conditionCount];
                        conditionArgs* conditionArguments = calloc(1, sizeof(conditionArgs));

                        // LOADING INPUT

                        conditionArguments->conditionType = *currCondition[2];

                        //subject chain
                        int subjectsChainSize = 0;
                        for (int k = 0;
                             currCondition[0][k] <= currCondition[1][0] && currCondition[0][k] != NULL; k++) {
                            subjectsChainSize++;
                        }

                        char* subjectChain[subjectsChainSize];
                        for (int k = 0;
                             currCondition[0][k] <= currCondition[1][0] && currCondition[0][k] != NULL; k++) {
                            subjectChain[k] = currCondition[0][k];
                        }
                        conditionArguments->personChain = subjectChain;
                        conditionArguments->personChainSize = subjectsChainSize;

                        int itemChainSize = 0;
                        ItemWithQuantity* *items = malloc(1024 * sizeof(ItemWithQuantity));

                        if (strcmp(conditionArguments->conditionType, "at") == 0) {
                            conditionArguments->place = *currCondition[3];
                            objectCount++;
                        }
                        else {
                            int k = 0;
                            while (1) {

                                ItemWithQuantity* item = malloc(sizeof(ItemWithQuantity));
                                item->name = input->objects[objectCount][1];
                                item->quantity = atoi(input->objects[objectCount][0]);
                                items[k] = item;

                                if (input->objects[objectCount][0] == currCondition[4][0] &&
                                    input->objects[objectCount][1] == currCondition[4][1]) {
                                    objectCount++;
                                    itemChainSize++;
                                    break;
                                }

                                itemChainSize++;
                                k++;
                                objectCount++;

                            }
                            conditionArguments->itemChainSize = itemChainSize;
                        }
                        conditionArguments->itemChain = items;

                        // END OF LOADING INPUT

                        conditionResult = conditionEvaluator(conditionArguments, people, places);
                        if (conditionResult == 0) {
                            conditionCount++;
                            break;
                        }


                        if (input->conditions[conditionCount] == currSentence[3]) {
                            conditionCount++;
                            free(conditionArguments);
                            break;
                        }


//                        free(items);
                        free(conditionArguments);
                        conditionCount++;
                    }
                }

                if (conditionResult) {

                    for (int k = 0; k < numOfActionsforCurrSentence; k++)
                        actionEvaluator(actionArgumentsList[k], people, places);

                }

                if (i == sentenceCount - 1) {
                    printf("OK\n");
                }

                // TODO: write a function to free everything inside actionArguments
                for (int k = 0; k < numOfActionsforCurrSentence; k++) {
                    if (actionArgumentsList[k] != NULL)
                        free(actionArgumentsList[k]);
                }

            }

            printf("OK\n");

        }

        input->freeResult(input);
    }

    places->free(places);
    people->free(people);

    return 0;
}
