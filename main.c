#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// the stuff below don't need to be a struct in the final form, just a map for the second is enough

struct Inventory {
    // TODO: a map of { item_name : count }
};

struct Place {
    char* name;

    // TODO: a map of everyone here
};

struct Person {
    char* name;
    struct Inventory* inventory;
};

struct ItemWithQuantity {
    int quantity;
    char* name;
};


struct actionArgs {

    /*
    * Multipurpose argument bundle for actions.
    * Only the variables that will be needed to call action will be initialized at the time of the action.
    */

    struct Person** personChain1;
    int personChain1Size;
    struct Person* personChain2;
    int personChain2Size;

    struct ItemWithQuantity** itemChain;
    int itemChainSize;
    struct Place* place;
};

/*
 * Possible action types : "buy", "buy from", "sell", "sell to", "go to"
 */
int actionEvaluator(char* actionType, struct actionArgs* args) {

    if (strcmp(actionType, "buy") == 0) {
        int numOfPeople = args->personChain1Size;
        int numOfItems = args->itemChainSize;

        for (int i=0; i<numOfPeople; i++) {
            struct Person* person_ptr = *( args->personChain1 + i * sizeof(struct Person*) );
            for (int j=0; j<numOfItems; j++) {
                struct ItemWithQuantity* item_ptr = *( args->itemChain + i * sizeof(struct ItemWithQuantity*) );

                // TODO: complete after map's are implemented
                // person_ptr->inventory[ item_ptr->name ] += item_ptr->quantity;

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
int buyEvaluator(struct Person* person, struct ItemWithQuantity* itemWithQuantity) {

    return 0;
}
int sellEvaluator(struct Person* person, struct ItemWithQuantity* itemWithQuantity) {

    return 0;
}
int buyFromEvaluator(struct Person* person1, struct Person* person2, struct ItemWithQuantity* itemWithQuantity) {

    return 0;
}
int sellToEvaluator(struct Person* person1, struct Person* person2, struct ItemWithQuantity* itemWithQuantity) {

    return 0;
}
int goToEvaluator(struct Person* person, struct Place* place) {

    return 0;
}

int main() {



    return 0;
}
