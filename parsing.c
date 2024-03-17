#include <stdio.h>
#include <string.h>
#include <ctype.h>

int is_valid_digit_number(const char *str) {
    if (strcmp(str, "") == 0)
        return 0;
    // Check if each character in the string is a digit
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0; // false
        }
    }
    return 1; // true
}

int main() {

    int MAX_TOKEN = 1024;
    while (1) {
        char inputStream[MAX_TOKEN];
        fgets(inputStream, MAX_TOKEN, stdin);

        char *tokens[MAX_TOKEN];
        char *ptr = inputStream;
        char *token;

        int numTokens = 0;
        while ((token = strsep(&ptr, " \n")) != NULL) {
            if (token[0] == '\0')
                continue;
            if (strcmp(token, "exit") == 0)
                return 0;
            tokens[numTokens++] = token;
        }

        char*** sentences[MAX_TOKEN][4];
        //pointer to... 0: start of actions 1: end of actions 2: start of conditions 3: end of conditions
        char** actions[MAX_TOKEN][6];
        //pointer to... 0: start of subjects 1: end of subjects 2: verb 3: from/to subject 4: start of objects 5: end of objects
        // verb --> "buy"|"sell"|"go"
        char** conditions[MAX_TOKEN][5];
        //pointer to... 0: start of subjects 1: end of subjects 2: verb 3: start of objects 4: end of objects
        // verb --> "has"|"more"|"less"|"at"
        char* subjects[MAX_TOKEN];
        //pointers to subjects
        char* objects[MAX_TOKEN][2];
        //pointers to objects
        char* actionFromTo[MAX_TOKEN];
        //subject that is bought from or sold to, accessed with actions[][3]

        int sentenceCount = 0;
        int actionCount = 0;
        int conditionCount = 0;
        int subjectCount = 0;
        int objectCount = 0;
        int taskCount = 0;


        int subjectState = 0;
        int verbState = 0;
        int objectState = 0;
        int sentenceState = 0;


        int j = 0;
        while (j < numTokens) {
            char *curr = tokens[j];
            if (sentenceState == 0) {
                sentences[sentenceCount][0] = (char ***) &actions[actionCount];
                sentenceState = 1;
                j--;
            }
            else if (sentenceState == 1) {
                if (subjectState == 0) {// a subject sequence is expected
                    subjects[subjectCount] = curr;
                    actions[actionCount][0] = &subjects[subjectCount];

                    subjectCount++;
                    subjectState = 1;
                } else if (subjectState == 1) { // "and" or a verb expected

                    if (strcmp(curr, "and") == 0)
                        subjectState = 2;

                    else if (strcmp(curr, "buy") == 0 || strcmp(curr, "sell") == 0 || strcmp(curr, "go") == 0) {
                        actions[actionCount][1] = &subjects[subjectCount-1];

                        if (strcmp(curr, "buy") == 0) {
                            char *str1 = curr;
                            actions[actionCount][2] = &str1;
                            verbState = 1;
                        }
                        if (strcmp(curr, "sell") == 0) {
                            char *str2 = curr;
                            actions[actionCount][2] = &str2;
                            verbState = 2;
                        }
                        if (strcmp(curr, "go") == 0) {//skip "to"
                            char *str3 = curr;
                            actions[actionCount][2] = &str3;
                            j++; //skipping "to"
                            verbState = 3;
                        }

                        subjectState = 3;
                    }
                }

                else if (subjectState == 2) { // subject expected, continued
                    subjects[subjectCount] = curr;
                    subjectCount++;
                    subjectState = 1;
                }

                else if (subjectState == 3) {
                    if (verbState == 1 || verbState == 2) { //object expected
                        objects[objectCount][0] = curr;
                        char *p = tokens[++j];
                        objects[objectCount][1] = p;
                        actions[actionCount][4] = (char **) &objects[objectCount];
                        objectCount++;
                        verbState *= 10;
                        if (j + 1 == numTokens) {
                            actions[actionCount][5] = (char **) &objects[objectCount - 1];
                            sentences[sentenceCount][1] = (char ***) &actions[actionCount];
                        }
                    }
                    else if (verbState == 3) {
                        objects[objectCount][0] = curr;
                        objects[objectCount][1] = curr;
                        actions[actionCount][4] = (char **) &objects[objectCount];
                        actions[actionCount][5] = (char **) &objects[objectCount];

                        objectCount++;
                        verbState *= 10;

                        if (j + 1 == numTokens) {
                            actions[actionCount][5] = (char **) &objects[objectCount - 1];
                            sentences[sentenceCount][1] = (char ***) &actions[actionCount];
                        }
                    }
                    else if (verbState == 10 || verbState == 20 || verbState == 30) {  //"and" or "from/to" or end of actions
                        if (strcmp(curr, "and") == 0) { //end of action or list of objects
                            int endOfActionContinuingWithAnd = 1;
                            while (strcmp(curr, "and") == 0) {
                                curr = tokens[++j];
                                if (is_valid_digit_number(curr)) {
                                    endOfActionContinuingWithAnd = 0;
                                    objects[objectCount][0] = curr;
                                    curr = tokens[++j];
                                    objects[objectCount][1] = curr;
                                    objectCount++;
                                    if (j == numTokens)
                                        break;
                                }
                            }
                            j--;
                            actions[actionCount][5] = (char **) &objects[objectCount - 1];
                            if (endOfActionContinuingWithAnd) {
                                subjectState = 0;
                                objectState = 0;
                                verbState = 0;
                                actionCount++;
                            }
                        } else if ((strcmp(curr, "from") == 0 && verbState == 10) ||
                                   (strcmp(curr, "to") == 0 && verbState == 20)) {
                            curr = tokens[++j]; //subject, bought from or sold to
                            actions[actionCount][2] = &actionFromTo[actionCount];
                            actionFromTo[actionCount] = curr;
                        }
                        else if (j + 1 == numTokens) {
                            actions[actionCount][5] = (char **) &objects[objectCount - 1];
                            sentences[sentenceCount][1] = (char ***) &actions[actionCount];
                        }
                        else if (strcmp(curr, "if") == 0) {
                            actions[actionCount][5] = (char **) &objects[objectCount - 1];
                            subjectState = 0;
                            objectState = 0;
                            verbState = 0;
                            sentences[sentenceCount][1] = (char ***) &actions[actionCount];
                            sentenceState = 2;
                            actionCount++;
                        }

                    }
                }
            }

            else if (sentenceState == 2) {
                sentences[sentenceCount][2] = (char ***) &conditions[conditionCount];
                sentenceState = 3;
                j--;
            }
            else if (sentenceState == 3) {
                if (subjectState == 0) { // beginning, a subject sequence is expected
                    subjects[subjectCount] = curr;
                    conditions[conditionCount][0] = &subjects[subjectCount];
                    subjectCount++;
                    subjectState = 1;
                }
                else if (subjectState == 1) { // "and" or a verb expected

                    if (strcmp(curr, "and") == 0)
                        subjectState = 2;
                    else if (strcmp(curr, "at") == 0) {
                        conditions[conditionCount][1] = &subjects[subjectCount-1];
                        char *str5 = curr;
                        conditions[conditionCount][2] = &str5;
                        subjectState = 3;
                        verbState = 1;
                    }
                    else if (strcmp(curr, "has") == 0) {
                        conditions[conditionCount][1] = &subjects[subjectCount-1];
                        curr = tokens[++j];
                        if (strcmp(curr, "more") == 0) { //check follow up word "than"
                            char *str6 = curr;
                            conditions[conditionCount][2] = &str6;
                            curr = tokens[++j]; //skipping than
                            subjectState = 3;
                            verbState = 2;
                        }
                        else if (strcmp(curr, "less") == 0) {
                            char *str7 = curr;
                            conditions[conditionCount][2] = &str7;
                            curr = tokens[++j]; //skipping than
                            subjectState = 3;
                            verbState = 2;
                        }
                        else if (is_valid_digit_number(curr)) {
                            char *str8 = "has";
                            conditions[conditionCount][2] = &str8;
                            subjectState = 3;
                            verbState = 2;
                            j--;
                        }
                    }
                }

                else if (subjectState == 2) {
                    subjects[subjectCount] = curr;
                    subjectCount++;
                    subjectState = 1;
                }

                else if (subjectState == 3) { //object sequence expected
                    if (verbState == 2) {
                        conditions[conditionCount][3] = (char **) &objects[objectCount];
                        objects[objectCount][0] = curr;
                        char *p = tokens[++j];
                        objects[objectCount][1] = p;
                        objectCount++;
                        if (j+1<numTokens) {
                            curr = tokens[++j];
                            while (strcmp(curr, "and") == 0 && j < numTokens) {
                                curr = tokens[++j];
                                if (is_valid_digit_number(curr)) {
                                    objects[objectCount][0] = curr;
                                    curr = tokens[++j];
                                    objects[objectCount][1] = curr;
                                    curr = tokens[++j];
                                    objectCount++;
                                } else {
                                    j--;
                                    break;
                                }
                            }
                        }

                        conditions[conditionCount][4] = (char **) &objects[objectCount - 1];
                        curr = tokens[j];
                    }
                    else if (verbState == 1) {

                        conditions[conditionCount][3] = (char **) &objects[objectCount];
                        conditions[conditionCount][4] = (char **) &objects[objectCount];

                        objects[objectCount][0] = curr;
                        objects[objectCount][1] = curr;

                        objectCount++;



                        curr = tokens[++j];//will be equal to "and" if possible
                    }

                    // if the curr is "and" determine if it is another condition or a new sentence
                    int i = j;
                    int isNewSentence = 0;
                    if (curr == NULL) {
                        sentences[sentenceCount][3] = (char ***) &conditions[conditionCount];
                        break;
                    }
                    if (strcmp(curr, "and") == 0) {
                        while ((curr = tokens[i++])) {
                            if (strcmp(curr, "has") == 0 || strcmp(curr, "at") == 0) {
                                break;
                            }
                            else if (strcmp(curr, "buy") == 0 || strcmp(curr, "sell") == 0 || strcmp(curr, "go") == 0) {
                                isNewSentence = 1;
                                break;
                            }
                        }
                    }

                    subjectState = 0;
                    verbState = 0;
                    conditionCount++;
                    if (isNewSentence) {
                        sentenceState = 0;
                        sentences[sentenceCount][3] = (char ***) &conditions[conditionCount - 1];
                        sentenceCount++;
                    }
                }
            }
            j++;

        }




    }
}
