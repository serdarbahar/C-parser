#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Structs.c"
#include "parsing.c"
#include "Evaluators.c"

int main() {

    struct People *people = initializePeople(20);
    struct PlacesList *places = initializePlacesList(20);

    while (1) {

        struct Result *input = parsing();
        if (input->exit)
            break;

        if (!input->isSentenceValid) {
            printf("INVALID");
            continue;
        }

        if (input->isQuestion) {
            questionArgs args;


            // input loading

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





        }
        else {
            int sentenceCount = 0;
            while (input->sentences[sentenceCount] != NULL) {
                sentenceCount++;
            }

            for (int i = 0; i<sentenceCount; i++) {
                char**** currSentence = input->sentences[i];
                //actions in current sentence
                for (int j = 0; currSentence[0][j] <= currSentence[1][0]; j++) {
                    char*** currAction = &currSentence[0][j];
                    char* actionType;
                    if (strcmp(*currAction[2],"buy") == 0) {
                        if (currAction[3] == NULL)
                            actionType = "buy from";
                        else
                            actionType = "buy";
                    }
                    else if (strcmp(*currAction[2],"sell") == 0) {
                        if (currAction[3] == NULL)
                            actionType = "sell to";
                        else
                            actionType = "sell";
                    }
                    else
                        actionType = "go";


                    actionArgs *actionArguments = (actionArgs*) calloc(1,sizeof(actionArgs));
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

                    char* subjectChainFromTo;
                    if (strcmp(actionType,"sell to") == 0 || strcmp(actionType,"buy from") == 0) {
                        actionArguments->personChain2 = currAction[3];
                        actionArguments->personChain2Size = 1;
                    }

                    ItemWithQuantity** items = (ItemWithQuantity**) malloc(1024*sizeof(ItemWithQuantity));
                    //item chain
                    if (strcmp(actionType, "go") == 0) {
                        actionArguments->place = currAction[4][0];
                    }
                    else {

                        for (int k = 0; currAction[4][0] + k * sizeof(char *) <= currAction[5][0] &&
                                        currAction[4][0] + k * sizeof(char *) != NULL &&
                                        currAction[4][1] + k * sizeof(char *) <= currAction[5][1] &&
                                        currAction[4][1] + k * sizeof(char *) != NULL; k++) {
                            printf("%d\n",1);
                            ItemWithQuantity *item = (ItemWithQuantity*) malloc(sizeof(ItemWithQuantity));
                            item->name = currAction[4][1] + k * sizeof(char *);
                            item->quantity = atoi(currAction[4][0] + k * sizeof(char *));
                            items[k] = item;
                        }
                        actionArguments->itemChain = items;
                    }

                    //EVALUATE ACTION HERE

                    free(items);
                    free(actionArguments);
                }
                //conditions
                for (int j = 0; currSentence[0][j] <= currSentence[1][0]; j++) {
                    char*** currCondition = &currSentence[0][j];
                    conditionArgs *conditionArguments = (conditionArgs*) calloc(1,sizeof(conditionArgs));

                    if (strcmp(*currCondition[2],"has") == 0) {
                        conditionArguments->conditionType = "has";
                    }
                    else if (strcmp(*currCondition[2],"more") == 0) {
                        conditionArguments->conditionType = "has more than";
                    }
                    else if (strcmp(*currCondition[2],"less") == 0) {
                        conditionArguments->conditionType = "has less than";
                    }
                    else if (strcmp(*currCondition[2],"at") == 0) {
                        conditionArguments->conditionType = "at";
                    }

                    //subject chain
                    int subjectsChainSize = 0;
                    for (int k = 0; currCondition[0][k] <= currCondition[1][0] && currCondition[0][k] != NULL; k++) {
                        subjectsChainSize++;
                    }
                    char* subjectChain[subjectsChainSize];
                    for (int k = 0; currCondition[0][k] <= currCondition[1][0] && currCondition[0][k] != NULL; k++) {
                        subjectChain[k] = currCondition[0][k];
                    }
                    conditionArguments->personChain = subjectChain;
                    conditionArguments->personChainSize = subjectsChainSize;

                    ItemWithQuantity** items = (ItemWithQuantity**) malloc(1024*sizeof(ItemWithQuantity));
                    if (strcmp(conditionArguments->conditionType,"at") == 0) {
                        conditionArguments->place = *currCondition[3];
                    }
                    else {
                        for (int k = 0; currCondition[3][0] + k * sizeof(char *) <= currCondition[4][0] &&
                                        currCondition[3][0] + k * sizeof(char *) != NULL &&
                                        currCondition[3][1] + k * sizeof(char *) <= currCondition[4][1] &&
                                        currCondition[3][1] + k * sizeof(char *) != NULL; k++) {
                            printf("%d\n",1);
                            ItemWithQuantity *item = (ItemWithQuantity*) malloc(sizeof(ItemWithQuantity));
                            item->name = currCondition[3][1] + k * sizeof(char *);
                            item->quantity = atoi(currCondition[3][0] + k * sizeof(char *));
                            items[k] = item;
                        }
                        conditionArguments->itemChain = items;
                    }
                    //EVALUATE CONDITION
                    free(conditionArguments);
                }


            }

        }


        input->freeResult(input);
    }

    places->free(places);
    people->free(people);

    return 0;
}