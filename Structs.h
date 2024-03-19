#ifndef INTERPRETER_PROJECT_STRUCTS_H
#define INTERPRETER_PROJECT_STRUCTS_H

#include <stdbool.h>

// Forward declaration of structs
struct Inventory;
struct Place;
struct PlacesList;
struct Person;
struct People;

// Inventory struct and its functions
struct Inventory {
    char** itemNames;
    int* itemCounts;
    int numOfItems;
    int maxNumOfItems;

    int (*getItemQuantity)(struct Inventory* self, char* itemName);
    void (*addItem)(struct Inventory* self, char* itemName, int quantity);
    void (*setCapacity)(struct Inventory* self, int numOfItemsToStore);
    int (*subtractItem)(struct Inventory* self, char* itemName, int quantity);
    void (*free)(struct Inventory* self);
};

struct Inventory* initializeInventory(int initialItemCapacity);

// Place struct and its functions
struct Place {
    char* name;
    struct Person** peopleInPlace;
    int numOfPeople;
    int maxNumOfPeople;

    bool (*isPersonInPlace)(struct Place* self, char* name);
    void (*setCapacity)(struct Place* self, int newCapacity);
    void (*addPerson)(struct Place* self, struct Person* person);
    void (*removePerson)(struct Place* self, struct Person* person);
    void (*free)(struct Place* self);
};

struct Place* initializePlace(char* name, int initialPersonCapacity);

// PlacesList struct and its functions
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

struct PlacesList* initializePlacesList(int initialPlaceCapacity);

// Person struct and its functions
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

struct Person* initializePerson(char* name, struct People* people);

// People struct and its functions
struct People {
    char** personNames;
    struct Person** personPointers;
    int numOfPeople;
    int maxNumOfPeople;

    void (*addPerson)(struct People* self, struct Person* person_ptr);
    struct Person* (*getPerson)(struct People* self, char* name);
    void (*setCapacity)(struct People* self, int newCapacity);
    void (*free)(struct People* self);
};

struct People* initializePeople(int initialPersonCapacity);

#endif //INTERPRETER_PROJECT_STRUCTS_H
