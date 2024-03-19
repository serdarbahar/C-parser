
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//                                  INVENTORY STRUCT

struct Inventory {

    // possible improvement : make an {item_name : item_count} hashmap instead
    // the quantity of item with name itemNames[i] is stored at itemCounts[i]
    char** itemNames;
    int* itemCounts;

    int numOfItems;
    int maxNumOfItems; // this maximum will be determined by the allocated memory, it will be increased if it is full

    int (*getItemQuantity)(struct Inventory* self, char* itemName);
    void (*addItem)(struct Inventory* self, char* itemName, int quantity);
    void (*setCapacity)(struct Inventory* self, int numOfItemsToStore);
    int (*subtractItem)(struct Inventory* self, char* itemName, int quantity);
    void (*free)(struct Inventory* self);
};


//                                  PLACE STRUCT

struct Place {

    char* name;

    // possible improvement : make a {person_name_in_place : true or false} hashmap instead
    struct Person** peopleInPlace;
    int numOfPeople;
    int maxNumOfPeople;

    bool (*isPersonInPlace)(struct Place* self, char* name);
    void (*setCapacity)(struct Place* self, int newCapacity);
    void (*addPerson)(struct Place* self, struct Person* person);
    void (*removePerson)(struct Place* self, struct Person* person);
    void (*free)(struct Place* self);

};


//                                  PLACES LIST STRUCT

struct PlacesList {

    char** placeNames;
    struct Place** placePointers;

    int numOfPlaces;
    int maxNumOfPlaces;

    void (*addPlace)(struct PlacesList* self, struct Place* place);
    struct Place* (*getPlace)(struct PlacesList* self, char* name);
    void (*setCapacity)(struct PlacesList* self, int newCapacity);
    void (*free)(struct PlacesList* self);
};


//                                  PERSON STRUCT

struct Person {
    char* name;
    struct Inventory* inventory;
    struct Place* location;

    void (*goTo)(struct Person* self, struct Place* place);
    void (*addItem)(struct Person* self, char* itemName, int quantity);
    void (*subtractItem)(struct Person* self, char* itemName, int quantity);
    int (*getItemQuantity)(struct Person* self, char* itemName);
    void (*free)(struct Person* self);

};


//                                   PEOPLE STRUCT

struct People {

    // possible improvement : make a {person_name : person_pointer} hashmap instead
    char** personNames;
    struct Person** personPointers;

    int numOfPeople;
    int maxNumOfPeople;

    void (*addPerson)(struct People* self, struct Person* person_ptr);
    struct Person* (*getPerson)(struct People* self, char* name);
    void (*setCapacity)(struct People* self, int newCapacity);
    void (*free)(struct People* self);

};


//                                  INVENTORY FUNCTIONS

int getItemQuantityFromInventory(struct Inventory* inventory, char* itemName) {

    for (int i=0; i<inventory->numOfItems; i++)
        if (strcmp(inventory->itemNames[i], itemName) == 0)
            return inventory->itemCounts[i];

    return 0;
}
void setMemoryToInventory(struct Inventory* inventory, int newCapacity) {
    // newCapacity indicates capacity of distinct items

    char** tempItemNamesPtr = realloc(inventory->itemNames, newCapacity * sizeof(char*));
    int* tempItemCountsPtr = realloc(inventory->itemCounts, newCapacity * sizeof(int));
    if (tempItemNamesPtr == NULL || tempItemCountsPtr == NULL) {
        printf("problem with reallocating inventory space");
        exit(1);
    }
    inventory->itemNames = tempItemNamesPtr;
    inventory->itemCounts = tempItemCountsPtr;

    // setting newly allocating memory NULL to prevent undefined behaviour if that memory is accessed
    for (int i=inventory->numOfItems; i<newCapacity; i++) {
        inventory->itemNames[i] = NULL;
        inventory->itemCounts[i] = 0;
    }

}
void addItemToInventory(struct Inventory* inventory, char* itemName, int quantity) {

    // increment the item if it already exists in the inventory
    for (int i=0; i<inventory->numOfItems; i++) {
        if (strcmp(inventory->itemNames[i], itemName) == 0) {
            inventory->itemCounts[i] += quantity;
            return;
        }
    }

    // create a new item if it doesn't exist in the inventory

    inventory->itemNames[inventory->numOfItems] = itemName;
    inventory->itemCounts[inventory->numOfItems] = quantity;
    inventory->numOfItems++;

    //allocate more memory to inventory if the space became full
    if (inventory->numOfItems == inventory->maxNumOfItems) {
        int newCapacity = inventory->maxNumOfItems * 2;
        inventory->maxNumOfItems = newCapacity;
        inventory->setCapacity(inventory, newCapacity);
    }
}
int subtractItemFromInventory(struct Inventory* inventory, char* itemName, int quantity) {
    // This function should be used after checking the item quantity, but I wrote error messages just in case

    for (int i=0; i<inventory->numOfItems; i++) {
        if (strcmp(inventory->itemNames[i], itemName) == 0) {
            if (inventory->itemCounts[i] < quantity) {
                printf("Not enough of %s in the inventory\n", itemName);
                return -1;
            }
            inventory->itemCounts[i] -= quantity;
            return 0;
        }
    }
    printf("Item %s not found in the inventory\n", itemName);
    return -1;
}
void freeInventory(struct Inventory* inventory) {
    free(inventory->itemNames);
    free(inventory->itemCounts);
    free(inventory);
}
struct Inventory* initializeInventory(int initialItemCapacity) {
    struct Inventory *inventory = malloc(sizeof(struct Inventory));

    inventory->itemNames = calloc(initialItemCapacity, sizeof(char*));
    inventory->itemCounts = calloc(initialItemCapacity, sizeof(int));

    inventory->numOfItems = 0;
    inventory->maxNumOfItems = initialItemCapacity;

    inventory->getItemQuantity = getItemQuantityFromInventory;
    inventory->addItem = addItemToInventory;
    inventory->setCapacity = setMemoryToInventory;
    inventory->subtractItem = subtractItemFromInventory;
    inventory->free = freeInventory;

    return inventory;
}


//                                  PLACE FUNCTIONS

bool isPersonInPlace(struct Place* place, char* name) {

    for (int i=0; i<place->numOfPeople; i++)
        if (strcmp(place->peopleInPlace[i]->name, name) == 0)
            return true;

    return false;

}
void setCapacityToPlace(struct Place* Place, int newCapacity) {
    // newCapacity indicates capacity of distinct people

    struct Person** tempPeopleInPlacePtr = realloc(Place->peopleInPlace, newCapacity * sizeof(struct Person*));
    if (tempPeopleInPlacePtr == NULL) {
        printf("problem with reallocating place space");
        exit(1);
    }
    Place->peopleInPlace = tempPeopleInPlacePtr;

    // setting newly allocating memory NULL to prevent undefined behaviour if that memory is accessed
    for (int i=Place->numOfPeople; i<newCapacity; i++) {
        Place->peopleInPlace[i] = NULL;
    }
}
void addPersonToPlace(struct Place* place, struct Person* person) {

    place->peopleInPlace[place->numOfPeople] = person;
    place->numOfPeople++;

    //allocate more memory to place if the space became full
    if (place->numOfPeople == place->maxNumOfPeople) {
        int newCapacity = place->maxNumOfPeople * 2;
        place->setCapacity(place, newCapacity);
        place->maxNumOfPeople = newCapacity;
    }

}
void removePersonFromPlace(struct Place* place, struct Person* person) {

    for (int i=0; i<place->numOfPeople; i++) {
        if (place->peopleInPlace[i] == person) {
            place->peopleInPlace[i] = place->peopleInPlace[place->numOfPeople-1];
            place->numOfPeople--;
            return;
        }
    }

}
void freePlace(struct Place* place) {
    free(place->peopleInPlace);
    free(place);
}
struct Place* initializePlace(char* name, int initialPersonCapacity) {

    struct Place *place = malloc(sizeof(struct Place));
    place->name = name;

    place->peopleInPlace = calloc(initialPersonCapacity, sizeof(struct Person*));

    place->numOfPeople = 0;
    place->maxNumOfPeople = initialPersonCapacity;

    place->isPersonInPlace = isPersonInPlace;
    place->setCapacity = setCapacityToPlace;
    place->addPerson = addPersonToPlace;
    place->removePerson = removePersonFromPlace;
    place->free = freePlace;

    return place;
}

struct Place NOWHERE = { "NOWHERE", NULL, 0 };


//                                  PLACES LIST FUNCTIONS

void addPlaceToPlacesList(struct PlacesList* placesList, struct Place* place) {

    placesList->placeNames[placesList->numOfPlaces] = place->name;
    placesList->placePointers[placesList->numOfPlaces] = place;
    placesList->numOfPlaces++;

    //allocate more memory to placesList if the space became full
    if (placesList->numOfPlaces == placesList->maxNumOfPlaces) {
        int newCapacity = placesList->maxNumOfPlaces * 2;
        placesList->setCapacity(placesList, newCapacity);
        placesList->maxNumOfPlaces = newCapacity;
    }

}
struct Place* getPlaceFromPlacesList(struct PlacesList* placesList, char* name) {

    for (int i=0; i<placesList->numOfPlaces; i++)
        if (strcmp(placesList->placeNames[i], name) == 0)
            return placesList->placePointers[i];

    return NULL;

}
void setMemoryToPlacesList(struct PlacesList* placesList, int newCapacity) {
    // newCapacity indicates capacity of distinct places

    char** tempPlaceNamesPtr = realloc(placesList->placeNames, newCapacity * sizeof(char*));
    struct Place** tempPlacePointersPtr = realloc(placesList->placePointers, newCapacity * sizeof(struct Place*));

    if (tempPlaceNamesPtr == NULL || tempPlacePointersPtr == NULL) {
        printf("problem with reallocating placesList space");
        exit(1);
    }

    placesList->placeNames = tempPlaceNamesPtr;
    placesList->placePointers = tempPlacePointersPtr;

    // setting newly allocating memory NULL to prevent undefined behaviour if that memory is accessed
    for (int i=placesList->numOfPlaces; i<newCapacity; i++) {
        placesList->placeNames[i] = NULL;
        placesList->placePointers[i] = NULL;
    }
}
/*
 * Frees the placesList struct and every place in it.

 */
void freePlacesList(struct PlacesList* placesList) {
    for (int i=0; i<placesList->numOfPlaces; i++)
        placesList->placePointers[i]->free(placesList->placePointers[i]);

    free(placesList->placeNames);
    free(placesList->placePointers);
    free(placesList);
}
struct PlacesList* initializePlacesList(int initialPlaceCapacity) {
    struct PlacesList *placesList = malloc(sizeof(struct PlacesList));

    placesList->placeNames = calloc(initialPlaceCapacity, sizeof(char*));
    placesList->placePointers = calloc(initialPlaceCapacity, sizeof(struct Place*));

    placesList->numOfPlaces = 0;
    placesList->maxNumOfPlaces = initialPlaceCapacity;

    placesList->addPlace = addPlaceToPlacesList;
    placesList->getPlace = getPlaceFromPlacesList;
    placesList->setCapacity = setMemoryToPlacesList;
    placesList->free = freePlacesList;

    return placesList;
}


//                                  PERSON FUNCTIONS


void personGoTo(struct Person* person, struct Place* place) {

    if (person->location != &NOWHERE)
        person->location->removePerson(person->location, person);

    person->location = place;
    place->addPerson(place, person);

}
void personAddItem(struct Person* person, char* itemName, int quantity) {
    person->inventory->addItem(person->inventory, itemName, quantity);
}
void personSubtractItem(struct Person* person, char* itemName, int quantity) {
    person->inventory->subtractItem(person->inventory, itemName, quantity);
}
int personGetItemQuantity(struct Person* person, char* itemName) {
    return person->inventory->getItemQuantity(person->inventory, itemName);
}
void freePerson(struct Person* person) {
    free(person->name);

    person->inventory->free(person->inventory);
    free(person->inventory);

    person->location->free(person->location);
    free(person->location);

    free(person);
}
struct Person* initializePerson(char* name, struct People* people) {

    struct Person* person = malloc(sizeof(struct Person));

    person->name = name;
    person->inventory = initializeInventory(10);
    person->location = &NOWHERE;

    person->goTo = personGoTo;
    person->addItem = personAddItem;
    person->subtractItem = personSubtractItem;
    person->getItemQuantity = personGetItemQuantity;
    person->free = freePerson;

    people->addPerson(people, person);


    return person;
}

//                                  PEOPLE FUNCTIONS

void addPersonToPeople(struct People* people, struct Person* person) {

    people->personNames[people->numOfPeople] = person->name;
    people->personPointers[people->numOfPeople] = person;
    people->numOfPeople++;

    //allocate more memory to people if the space became full
    if (people->numOfPeople == people->maxNumOfPeople) {
        int newCapacity = people->maxNumOfPeople * 2;
        people->setCapacity(people, newCapacity);
        people->maxNumOfPeople = newCapacity;
    }

}
struct Person* getPersonFromPeople(struct People* people, char* name) {

    for (int i=0; i<people->numOfPeople; i++)
        if (strcmp(people->personNames[i], name) == 0)
            return people->personPointers[i];

    return NULL;

}
void setMemoryToPeople(struct People* people, int newCapacity) {
    // newCapacity indicates capacity of distinct people

    char** tempPersonNamesPtr = realloc(people->personNames, newCapacity * sizeof(char*));
    struct Person** tempPersonPointersPtr = realloc(people->personPointers, newCapacity * sizeof(struct Person*));
    if (tempPersonNamesPtr == NULL || tempPersonPointersPtr == NULL) {
        printf("problem with reallocating people space");
        exit(1);
    }
    people->personNames = tempPersonNamesPtr;
    people->personPointers = tempPersonPointersPtr;

    // setting newly allocating memory NULL to prevent undefined behaviour if that memory is accessed
    for (int i=people->numOfPeople; i<newCapacity; i++) {
        people->personNames[i] = NULL;
        people->personPointers[i] = NULL;
    }
}
/**
 * Frees the people struct and every person in it.
 */
void freePeople(struct People* people) {
    for (int i=0; i<people->numOfPeople; i++)
        people->personPointers[i]->free(people->personPointers[i]);

    free(people->personNames);
    free(people->personPointers);
    free(people);
}
struct People* initializePeople(int initialPersonCapacity) {
    struct People *people = malloc(sizeof(struct People));

    people->personNames = calloc(initialPersonCapacity, sizeof(char*));
    people->personPointers = calloc(initialPersonCapacity, sizeof(struct Person*));

    people->numOfPeople = 0;
    people->maxNumOfPeople = initialPersonCapacity;

    people->addPerson = addPersonToPeople;
    people->getPerson = getPersonFromPeople;
    people->setCapacity = setMemoryToPeople;
    people->free = freePeople;

    return people;
}