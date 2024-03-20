#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Structs.h"
#include "parsing.h"
#include "Evaluators.h"

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
        if (input->isQuestion) {
            questionArgs args;

            // loading input

            if (input->whoAtQuestion) {
                args.questionType = "who at";
                char* placeName = input->whoAtQuestion;
                args.place = places->getPlace(places,placeName);
            }
            else if (input->totalQuestion) {
                args.questionType = "total";
                char* personName = input->totalQuestion;
                args.person = people->getPerson(people,personName);
            }
            else if (input->totalItemQuestion) {
                args.questionType = "total item";
                args.itemName = *input->totalItemQuestion[2];
                char** personList = calloc(1024, sizeof(char*));
                char** subjects = input->subjectsForTotalItem;
                int itemListSize = 0;
                for (int k = 0; subjects[k] != NULL; k++) {
                    itemListSize++;
                }
                for (int k = 0; k<itemListSize; k++) {
                    personList[k] = subjects[k];
                }
                args.personChain = personList;
                args.personChainSize = itemListSize;

            }
            else if (input->whereQuestion) {
                args.questionType = "where";
                char* personName = input->whereQuestion;
                args.person = people->getPerson(people,personName);
            }

            // getting output

            char* outputString = questionEvaluator(&args);
            printf("%s",outputString);
            continue;
        }
        else {
            int sentenceCount = input->sentenceCount;

            int actionCount = 0;
            int conditionCount = 0;
            int objectCount = 0;

            for (int i = 0; i<sentenceCount; i++) {

                char**** currSentence = input->sentences[i];

                //actions in current sentence
                for (; actions[actionCount] <= currSentence[1]; actionCount++) {

                    char*** currAction = actions[actionCount];


                    char* actionType;
                    if (strcmp(*currAction[2],"buy") == 0) {
                        if (**currAction[3] == '\0')
                            actionType = "buy";
                        else
                            actionType = "buy from";
                    }
                    else if (strcmp(*currAction[2],"sell") == 0) {
                        if (**currAction[3] == '\0')
                            actionType = "sell";
                        else
                            actionType = "sell to";
                    }
                    else
                        actionType = "go";

                    actionArgs *actionArguments = (actionArgs*) malloc(sizeof(actionArgs));
                    actionArguments->actionType = actionType;
                    //subject chain
                    int subjectsChainSize = 0;
                    for (int k = 0; currAction[0][k] <= currAction[1][0] && currAction[0][k] != NULL; k++) {
                        subjectsChainSize++;
                    }
                    char* subjectChain[subjectsChainSize];
                    for (int k = 0; currAction[0][k] <= currAction[1][0] && currAction[0][k] != NULL; k++) {
                        subjectChain[k] = currAction[0][k];
                    }
                    actionArguments->personChain1 = subjectChain;
                    actionArguments->personChain1Size = subjectsChainSize;

                    int itemChainSize = 0;

                    char* subjectChainFromTo;
                    if (strcmp(actionType,"sell to") == 0 || strcmp(actionType,"buy from") == 0) {
                        actionArguments->personChain2Size = 1;
                    }
                    actionArguments->personChain2 = currAction[3];

                    ItemWithQuantity** items = malloc(1024*sizeof(ItemWithQuantity*));
                    //item chain
                    if (strcmp(actionType, "go") == 0) {
                        actionArguments->place = currAction[4][0];
                    }
                    else {

                        int k = 0;
                        while (1) {
                            ItemWithQuantity* item = malloc(sizeof(ItemWithQuantity));
                            item->name = input->objects[objectCount][1];
                            item->quantity = atoi(input->objects[objectCount][0]);
                            items[k] = item;

                            if (input->objects[objectCount][0] == currAction[5][0] &&
                                input->objects[objectCount][1] == currAction[5][1]) {
                                objectCount++;
                                break;

                            }

                            itemChainSize++;
                            k++;
                            objectCount++;

                        }
                        actionArguments->itemChainSize = itemChainSize;
                    }
                    actionArguments->itemChain = items;

                    //EVALUATE SINGULAR ACTION HERE, USE actionArguments

                    free(items);
                    free(actionArguments);

                }
                //conditions
                if (currSentence[2][0][0] != NULL) {
                    for (; input->conditions[conditionCount] <= currSentence[3]; conditionCount++) {


                        char ***currCondition = input->conditions[conditionCount];
                        conditionArgs *conditionArguments = (conditionArgs *) calloc(1, sizeof(conditionArgs));


                        if (strcmp(*currCondition[2], "has") == 0) {
                            conditionArguments->conditionType = "has";
                        } else if (strcmp(*currCondition[2], "more") == 0) {
                            conditionArguments->conditionType = "has more than";
                        } else if (strcmp(*currCondition[2], "less") == 0) {
                            conditionArguments->conditionType = "has less than";
                        } else if (strcmp(*currCondition[2], "at") == 0) {
                            conditionArguments->conditionType = "at";
                        }

                        //subject chain
                        int subjectsChainSize = 0;
                        for (int k = 0;
                             currCondition[0][k] <= currCondition[1][0] && currCondition[0][k] != NULL; k++) {
                            subjectsChainSize++;
                        }
                        char *subjectChain[subjectsChainSize];
                        for (int k = 0;
                             currCondition[0][k] <= currCondition[1][0] && currCondition[0][k] != NULL; k++) {
                            subjectChain[k] = currCondition[0][k];
                        }
                        conditionArguments->personChain = subjectChain;
                        conditionArguments->personChainSize = subjectsChainSize;

                        int itemChainSize = 0;
                        ItemWithQuantity **items = (ItemWithQuantity **) malloc(1024 * sizeof(ItemWithQuantity));
                        if (strcmp(conditionArguments->conditionType, "at") == 0) {
                            conditionArguments->place = *currCondition[3];
                        } else {
                            int k = 0;
                            while (1) {

                                ItemWithQuantity* item = malloc(sizeof(ItemWithQuantity));
                                item->name = input->objects[objectCount][1];
                                item->quantity = atoi(input->objects[objectCount][0]);
                                items[k] = item;

                                if (input->objects[objectCount][0] == currCondition[4][0] &&
                                    input->objects[objectCount][1] == currCondition[4][1]) {
                                    objectCount++;
                                    break;
                                }

                                itemChainSize++;
                                k++;
                                objectCount++;

                            }
                            conditionArguments->itemChainSize = itemChainSize;
                        }
                        conditionArguments->itemChain = items;


                        printf("%s ", conditionArguments->personChain[0]);
                        printf("%s ", conditionArguments->conditionType);
                        printf("%d ", conditionArguments->itemChain[0]->quantity);
                        printf("%s ", conditionArguments->itemChain[0]->name);
                        printf("\n");


                        //EVALUATE SINGULAR CONDITION HERE, USE conditionArguments


                        free(items);
                        free(conditionArguments);
                    }
                }



            }

        }


        input->freeResult(input);









    }

    places->free(places);
    people->free(people);

    return 0;
}