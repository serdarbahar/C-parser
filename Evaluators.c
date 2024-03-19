#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Structs.c"
#include "parsing.c"


typedef int boolean;

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

/*
 * Possible action types : "buy", "buy from", "sell", "sell to", "go to"
 * returns 0 if action is successful, 1 if action is INVALID, -1 if action type is invalid
 */
int actionEvaluator(actionArgs* args, struct People* people, struct PlacesList* places) {

    char* actionType = args->actionType;

    // personChain1 buy itemChain
    if (strcmp(actionType, "buy") == 0) {
        int numOfPeople = args->personChain1Size;
        int numOfItems = args->itemChainSize;

        for (int i=0; i<numOfPeople; i++) {
            struct Person* person = people->getPerson(people, args->personChain1[i]);
            for (int j=0; j<numOfItems; j++) {
                ItemWithQuantity* item = args->itemChain[j];

                person->addItem(person, item->name, item->quantity);
            }
        }

        return 0;
    }

        // personChain1 sell itemChain
    else if (strcmp(actionType, "sell") == 0) {
        // check if all people have enough of everything to sell, then sell if everyone sells

        int numOfPeople = args->personChain1Size;
        int numOfItems = args->itemChainSize;

        bool everyoneCanSell = true;

        bool breakOuterLoop = false;
        for (int i=0; i<numOfPeople; i++) {
            if (breakOuterLoop) break;

            struct Person* person = people->getPerson(people, args->personChain1[i]);
            for (int j=0; j<numOfItems; j++) {
                ItemWithQuantity* item = args->itemChain[j];
                if (person->getItemQuantity(person, item->name) < item->quantity) {
                    everyoneCanSell = false;
                    breakOuterLoop = true;
                    break;
                }
            }
        }

        if (everyoneCanSell) {
            for (int i=0; i<numOfPeople; i++) {
                struct Person* person = people->getPerson(people, args->personChain1[i]);
                for (int j=0; j<numOfItems; j++) {
                    ItemWithQuantity* item = args->itemChain[j];
                    person->subtractItem(person, item->name, item->quantity);
                }
            }
        }

        return 0;
    }

        // personChain1 buy itemChain from personChain2
    else if (strcmp(actionType, "buy from") == 0) {

        // make sure that there is only 1 seller
        if (args->personChain2Size != 1) {
            printf("INVALID");
            return 1;
        }

        struct Person* seller = people->getPerson(people, args->personChain2[0]);

        // make sure that one of the buyers and seller is not the same
        boolean sellerIsBuyer = false;
        for (int i=0; i<args->personChain1Size; i++) {
            if (strcmp(args->personChain1[i], seller->name) == 0) {
                sellerIsBuyer = true;
                break;
            }
        }

        if (sellerIsBuyer) {
            printf("INVALID");
            return 1;
        }


        // check if seller has enough items to sell to everyone, and allow the seller to sell only then

        ItemWithQuantity* wantedItems[args->itemChainSize];
        boolean sellerHasEnough = true;

        for (int i=0; i<args->itemChainSize; i++) {

            // multiplying by num of people buying the item since the seller must sell the same quantity to all of them
            wantedItems[i]->quantity *= args->personChain1Size;

            if (seller->getItemQuantity(seller, wantedItems[i]->name) < wantedItems[i]->quantity) {
                sellerHasEnough = false;
                break;
            }

        }


        if (sellerHasEnough) {
            for (int i=0; i<args->personChain1Size; i++) {
                struct Person* buyer = people->getPerson(people, args->personChain1[i]);
                for (int j=0; j<args->itemChainSize; j++) {
                    ItemWithQuantity* item = args->itemChain[j];
                    buyer->addItem(buyer, item->name, item->quantity);
                    seller->subtractItem(seller, item->name, item->quantity);
                }
            }
        }

        return 0;
    }

        // personChain1 sell itemChain to personChain2
    else if (strcmp(actionType, "sell to") == 0) {

        // make sure there is only 1 buyer
        if (args->personChain2Size != 1) {
            printf("INVALID");
            return 1;
        }

        struct Person* buyer = people->getPerson(people, args->personChain2[0]);

        // make sure that one of the sellers and buyer is not the same
        boolean buyerIsSeller = false;
        for (int i=0; i<args->personChain1Size; i++) {
            if (strcmp(args->personChain1[i], buyer->name) == 0) {
                buyerIsSeller = true;
                break;
            }
        }

        if (buyerIsSeller) {
            printf("INVALID");
            return 1;
        }


        // check if all people have enough of everything to sell, then sell if everyone sells

        boolean everySellerHasEnough = true;
        for (int i=0; i<args->personChain1Size; i++) {
            struct Person* seller = people->getPerson(people, args->personChain1[i]);
            for (int j=0; j<args->itemChainSize; j++) {
                ItemWithQuantity* item = args->itemChain[j];
                if (seller->getItemQuantity(seller, item->name) < item->quantity) {
                    everySellerHasEnough = false;
                    break;
                }
            }
        }

        if (everySellerHasEnough) {
            for (int i=0; i<args->personChain1Size; i++) {
                struct Person* seller = people->getPerson(people, args->personChain1[i]);
                for (int j=0; j<args->itemChainSize; j++) {
                    ItemWithQuantity* item = args->itemChain[j];
                    seller->subtractItem(seller, item->name, item->quantity);
                    buyer->addItem(buyer, item->name, item->quantity);
                }
            }
        }

        return 0;
    }

        // personChain1 go to place
    else if (strcmp(actionType, "go to") == 0) {

        struct Place* place = places->getPlace(places, args->place);

        for (int i=0; i<args->personChain1Size; i++) {
            struct Person* person = people->getPerson(people, args->personChain1[i]);

            struct Place* previousLocation = person->location;
            previousLocation->removePerson(previousLocation, person);

            person->location = place;
            place->addPerson(place, person);
        }

        return 0;
    }

    else {
        printf("Invalid action type\n");
        return -1;
    }
}

/**
 * Possible condition types : "at", "has", "has less than", "has more than"
 * returns 1 if conditionChain is true, returns 0 if conditionChain is false, -1 is conditionType is invalid
 */
int conditionEvaluator(conditionArgs* args, struct People* people, struct PlacesList* places) {

    char* conditionType = args->conditionType;

    // personChain at place
    if (strcmp(conditionType, "at") == 0) {
        char* placeName = args->place;
        struct Place* place = places->getPlace(places, placeName);

        boolean everyoneAtPlace = 1;
        for (int i=0; i<args->personChainSize; i++) {

            char* personName = args->personChain[i];
            struct Person* person = people->getPerson(people, personName);

            if (person->location != place) {
                everyoneAtPlace = 0;
                break;
            }
        }

        return everyoneAtPlace;
    }

    // personChain has itemChain
    if (strcmp(conditionType, "has") == 0) {

        boolean everyoneHasExactQuantity = 1;

        for (int i=0; i<args->personChainSize; i++) {

            char* personName = args->personChain[i];
            struct Person* person = people->getPerson(people, personName);

            for (int j=0; j<args->itemChainSize; j++) {

                ItemWithQuantity* item = args->itemChain[j];

                if (person->getItemQuantity(person, item->name) != item->quantity) {
                    everyoneHasExactQuantity = 0;
                    return everyoneHasExactQuantity;
                }

            }
        }

        return everyoneHasExactQuantity;
    }

    if (strcmp(conditionType, "has less than") == 0) {

        boolean everyoneHasLessThanQuantity = 1;

        for (int i=0; i<args->personChainSize; i++) {

            char* personName = args->personChain[i];
            struct Person* person = people->getPerson(people, personName);

            for (int j=0; j<args->itemChainSize; j++) {
                ItemWithQuantity* item = args->itemChain[j];

                if (person->getItemQuantity(person, item->name) >= item->quantity) {
                    everyoneHasLessThanQuantity = 0;
                    return everyoneHasLessThanQuantity;
                }

            }
        }

        return everyoneHasLessThanQuantity;
    }

    if (strcmp(conditionType, "has more than") == 0) {

        boolean everyoneHasMoreThanQuantity = 1;

        for (int i=0; i<args->personChainSize; i++) {

            char* personName = args->personChain[i];
            struct Person* person = people->getPerson(people, personName);

            for (int j=0; j<args->itemChainSize; j++) {
                ItemWithQuantity* item = args->itemChain[j];

                if (person->getItemQuantity(person, item->name) <= item->quantity) {
                    everyoneHasMoreThanQuantity = 0;
                    return everyoneHasMoreThanQuantity;
                }

            }
        }

        return everyoneHasMoreThanQuantity;
    }

    else {
        printf("Invalid condition type\n");
        return -1;
    }

}

/**
 * evaluates the conditions and does the action if the conditions are true
 * returns -10 if the conditions are false, returns the return value of the actionEvaluator otherwise
 */
int ifEvaluator(actionArgs* actionArgs, conditionArgs* conditionArgs, struct People* people, struct PlacesList* places) {

    int conditionResult = conditionEvaluator(conditionArgs, people, places);

    if (conditionResult)
        return actionEvaluator(actionArgs, people, places);
    else
        return -10;

}

