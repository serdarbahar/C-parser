#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Structs.c"
#include "parsing.c"

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
    struct Person** personChain1;
    int personChain1Size;
    struct Person* personChain2;
    int personChain2Size;

    ItemWithQuantity** itemChain;
    int itemChainSize;
    struct Place* place;
} actionArgs;

// Possible action types : "buy", "buy from", "sell", "sell to", "go to"
int actionEvaluator(char* actionType, actionArgs* args) {

    if (strcmp(actionType, "buy") == 0) {
        int numOfPeople = args->personChain1Size;
        int numOfItems = args->itemChainSize;

        for (int i=0; i<numOfPeople; i++) {
            struct Person* person_ptr = *( args->personChain1 + i * sizeof(struct Person*) );
            for (int j=0; j<numOfItems; j++) {
                struct ItemWithQuantity* item_ptr = *( args->itemChain + i * sizeof(struct ItemWithQuantity*) );

                // TODO: complete after map's are implemented

            }
        }

        return 0;
    }

    else if (strcmp(actionType, "sell") == 0) {
        // check if all people have enough of everything to sell, then sell if everyone sells

    }
    else if (strcmp(actionType, "buy from") == 0) {
        // call buyFromEvaluator with appropriate arguments
    }
    else if (strcmp(actionType, "sell to") == 0) {
        // call sellToEvaluator with appropriate arguments
    }
    else if (strcmp(actionType, "go to") == 0) {
        // call goToEvaluator with appropriate arguments
    }

    else {
        printf("Invalid action type\n");
        return -1;
    }
}


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
            places->free(places);
            people->free(people);
            continue;
        }

        if (input->isQuestion) {
            if (input->whoAtQuestion) {

            }
            else if (input->totalQuestion) {

            }
            else if (input->totalItemQuestion) {

            }
            else if (input->whereQuestion) {

            }
        }
        else {
            int sentenceCount = 0;
            while (input->sentences[sentenceCount] != NULL) {
                sentenceCount++;
            }

            for (int i = 0; i<sentenceCount; i++) {
                //
            }

        }




        input->freeResult(input);

    }
    
    places->free(places);
    people->free(people);
    return 0;
}
