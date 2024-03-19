#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Structs.c"
#include "parsing.c"



int main() {

    static struct People *people = initializePeople(20);
    static struct PlacesList *places = initializePlacesList(20);


    // ...


    places->free(places);
    people->free(people);

    return 0;
}
