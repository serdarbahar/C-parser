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

                // TODO : serdar inputu diğerleri gibi verir misin aşkım :3


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
            int sentenceCount = 0;
            while (input->sentences[sentenceCount] != NULL) {
                sentenceCount++;
            }

            for (int i = 0; i<sentenceCount; i++) {
                char**** currSentence = input->sentences[i];
                //actions in current sentence
                for (int j = 0; &currSentence[0][j] <= &currSentence[1][0]; j++) {
                    char** currAction = currSentence[0][j];
                    char* actionType;
                    if (strcmp(currAction[2],"buy") == 0) {
                        if (currAction[3] == NULL)
                            actionType = "buy from";
                        else
                            actionType = "buy";
                    }
                    else if (strcmp(currAction[2],"sell") == 0) {
                        if (currAction[3] == NULL)
                            actionType = "sell to";
                        else
                            actionType = "sell";
                    }
                    else
                        actionType = "go";


                    actionArgs *arguments = (actionArgs*) calloc(1,sizeof(actionArgs));

                    int subjectsChainSize = 0;
                    for (int k = 0; &currAction[0]+k*sizeof(char*) <= &currAction[1]; k++) {
                        subjectsChainSize++;
                    }
                    char* subjectChain[subjectsChainSize];
                    for (int k = 0; &currAction[0]+k*sizeof(char*) <= &currAction[1]; k++) {
                        subjectChain[k] = *(&currAction[0] + k*sizeof(char*));
                    }
                    arguments->personChain1 = subjectChain;
                    arguments->personChain1Size = subjectsChainSize;

                    char* subjectChainFromTo;
                    if (strcmp(actionType,"sell to") == 0 || strcmp(actionType,"buy from") == 0) {
                        arguments->personChain2 = currAction[4];
                    }









                    free(arguments);







                }
            }

        }




        input->freeResult(input);
    }

    places->free(places);
    people->free(people);

    return 0;
}