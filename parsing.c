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

int is_curr_keyword(const char *str) {
    if (strcmp(str,"and") == 0 || strcmp(str,"buy") == 0 || strcmp(str,"from") == 0 || strcmp(str,"from") == 0 ||
            strcmp(str,"sell") == 0 || strcmp(str,"to") == 0 || strcmp(str,"go") == 0 || strcmp(str,"if") == 0 ||
            strcmp(str,"at") == 0 || strcmp(str,"has") == 0 || strcmp(str,"less") == 0 || strcmp(str,"than") == 0 ||
            strcmp(str,"more") == 0 || strcmp(str,"total") == 0 || strcmp(str,"where") == 0 || strcmp(str,"who") == 0) {
        return 1;
    }
    return 0;
}

int is_curr_question_word(const char *str) {
    if (strcmp(str,"total") == 0 || strcmp(str,"where") == 0 || strcmp(str,"who") == 0) {
        return 1;
    }
    return 0;
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
        while ((token = strsep(&ptr, " \n?")) != NULL) {
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

        //Arrays for different type of questions
        char** totalItemQuestion[3];
        //pointer to 0: start of subjects 1: end of subjects 2: item
        char* subjectsForTotalItem[MAX_TOKEN];
        char* totalQuestion;
        char* whereQuestion;
        char* whoAtQuestion;

        int sentenceCount = 0;
        int actionCount = 0;
        int conditionCount = 0;
        int subjectCount = 0;
        int objectCount = 0;
        int subjectCountforQ = 0;

        //input is read from left to right, states give information about the state of the sentence
        //at the current word (e.g. if subjects are all read --> subjectState = 3)
        int subjectState = 0;
        int verbState = 0;
        int sentenceState = 0;
        int questionState = 0;

        int isSentenceValid = 0;
        int isQuestion = 0;
        int isWhoAt = 0;
        int isWhere = 0;
        int isTotalItem = 0;
        int isTotal = 0;
        int questionCheck = 0;

        int j = 0;
        while (j < numTokens) {
            char *curr = tokens[j];
            if (isQuestion) {

                if (isTotal + isTotalItem + isWhere + isWhoAt != 1) {
                    isSentenceValid = 0;
                    break;
                }

                if (isWhoAt) {

                    if (j == 0) {
                    }
                    else if (j==1) {
                        if (strcmp(curr, "at") != 0) {
                            isSentenceValid = 0;
                            break;
                        }
                    }
                    else if (j==2) {
                        if (is_curr_keyword(curr)) {
                            isSentenceValid = 0;
                            break;
                        }

                        whoAtQuestion = curr;
                    }
                    else {
                        isSentenceValid = 0;
                        break;
                    }
                }

                else if (isWhere) {
                    if (j == 0) {
                        if (is_curr_keyword(curr)) {
                            isSentenceValid = 0;
                            break;
                        }
                        whereQuestion = curr;
                    }
                    else if (j == 1) {
                        if (strcmp(curr, "where") != 0) {
                            isSentenceValid = 0;
                            break;
                        }
                    }
                    else {
                        isSentenceValid = 0;
                        break;
                    }
                }

                else if (isTotal) {
                    if (j == 0) {
                        if (is_curr_keyword(curr)) {
                            isSentenceValid = 0;
                            break;
                        }
                        totalQuestion = curr;
                    }
                    else if (j == 1) {
                        if (strcmp(curr, "total") != 0) {
                            isSentenceValid = 0;
                            break;
                        }
                    }
                    else {
                        isSentenceValid = 0;
                        break;
                    }
                }

                else if (isTotalItem) {
                    if (questionState == 0) { //subject sequence expected
                        if (is_curr_keyword(curr)) {
                            isSentenceValid = 0;
                            break;
                        }
                        totalItemQuestion[0] = (char **) &subjectsForTotalItem;
                        subjectsForTotalItem[0] = curr;
                        questionState = 1;
                        subjectCountforQ++;
                    }
                    else if (questionState == 1) { //"and" or "total" expected
                        if (strcmp(curr, "total") == 0) {
                            totalItemQuestion[1] = (char **) &subjectsForTotalItem[subjectCountforQ-1];
                            questionState = 3;
                        }
                        else if (strcmp(curr, "and") == 0) {
                            questionState = 2;
                        }
                    }
                    else if (questionState == 2) {
                        if (is_curr_keyword(curr)) {
                            isSentenceValid = 0;
                            break;
                        }
                        subjectsForTotalItem[subjectCountforQ] = curr;
                        subjectCountforQ++;
                        questionState = 1;
                    }
                    else if (questionState == 3) {
                        if (is_curr_keyword(curr)) {
                            isSentenceValid = 0;
                            break;
                        }
                        char* p = curr;
                        totalItemQuestion[2] = &p;
                        if (++j != numTokens) {
                            isSentenceValid = 0;
                            break;
                        }
                    }
                }
            }
            else if (sentenceState == 0) {
                //check if question
                int i = 0;
                char *p;
                if (!questionCheck) {
                    while (i < numTokens) {
                        p = tokens[i];
                        if (strcmp(p, "total") == 0 && i + 1 == numTokens) {
                            isTotal = 1;
                            isQuestion = 1;
                            isSentenceValid = 1;
                        } else if (strcmp(p, "total") == 0 && i + 1 < numTokens) {
                            isTotalItem = 1;
                            isQuestion = 1;
                            isSentenceValid = 1;
                        } else if (strcmp(p, "where") == 0 && i + 1 == numTokens) {
                            isWhere = 1;
                            isQuestion = 1;
                            isSentenceValid = 1;
                        } else if (strcmp(p, "who") == 0 && i == 0) {
                            isWhoAt = 1;
                            isQuestion = 1;
                            isSentenceValid = 1;
                        } else if (is_curr_question_word(p)) {
                            isSentenceValid = 0;
                            break;
                        }
                        i++;
                    }
                    questionCheck = 1;
                }

                if (isQuestion)
                    continue;

                sentences[sentenceCount][0] = (char ***) &actions[actionCount];
                sentenceState = 1;
                j--;

                if (is_curr_keyword(curr)) {
                    isSentenceValid = 0;
                    break;
                }
            }
            else if (sentenceState == 1) {
                if (subjectState == 0) {// a subject sequence is expected
                    isSentenceValid = 0;
                    if (is_curr_keyword(curr)) {
                        isSentenceValid = 0;
                        break;
                    }

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
                        if (strcmp(curr, "go") == 0) {
                            char *str3 = curr;
                            actions[actionCount][2] = &str3;
                            j++; //skipping "to"
                            char *p = tokens[j];
                            if (strcmp(p, "to") != 0) {
                                isSentenceValid = 0;
                                break;
                            }
                            verbState = 3;
                        }

                        subjectState = 3;
                    }
                    else {
                        isSentenceValid = 0;
                        break;
                    }
                }

                else if (subjectState == 2) { // subject expected, continued

                    if (is_curr_keyword(curr)) {
                        isSentenceValid = 0;
                        break;
                    }

                    subjects[subjectCount] = curr;
                    subjectCount++;
                    subjectState = 1;
                }

                else if (subjectState == 3) {
                    if (verbState == 1 || verbState == 2) { //object expected

                        if (!is_valid_digit_number(curr)){
                            isSentenceValid = 0;
                            break;
                        }

                        objects[objectCount][0] = curr;
                        char *p = tokens[++j];

                        if (is_valid_digit_number(p)){
                            isSentenceValid = 0;
                            break;
                        }
                        if (is_curr_keyword(p)) {
                            isSentenceValid = 0;
                            break;
                        }

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

                        if (is_curr_keyword(curr)) {
                            isSentenceValid = 0;
                            break;
                        }

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
                        isSentenceValid = 1;
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
                            if (endOfActionContinuingWithAnd || verbState == 30) {
                                subjectState = 0;
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
                            verbState = 0;
                            sentences[sentenceCount][1] = (char ***) &actions[actionCount];
                            sentenceState = 2;
                            actionCount++;
                        }

                        else {
                            isSentenceValid = 0;
                            break;
                        }

                    }
                }
            }

            else if (sentenceState == 2) {
                sentences[sentenceCount][2] = (char ***) &conditions[conditionCount];
                sentenceState = 3;
                j--;

                if (is_curr_keyword(curr)) {
                    isSentenceValid = 0;
                    break;
                }

            }
            else if (sentenceState == 3) {
                if (subjectState == 0) { // beginning, a subject sequence is expected
                    isSentenceValid = 0;
                    if (is_curr_keyword(curr)) {
                        isSentenceValid = 0;
                        break;
                    }

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
                        else {
                            isSentenceValid = 0;
                            break;
                        }
                    }
                }

                else if (subjectState == 2) {

                    if (is_curr_keyword(curr)) {
                        isSentenceValid = 0;
                        break;
                    }

                    subjects[subjectCount] = curr;
                    subjectCount++;
                    subjectState = 1;
                }

                else if (subjectState == 3) { //object sequence expected
                    if (verbState == 2) {
                        conditions[conditionCount][3] = (char **) &objects[objectCount];

                        if (!is_valid_digit_number(curr)) {
                            isSentenceValid = 0;
                            break;
                        }


                        objects[objectCount][0] = curr;
                        char *p = tokens[++j];

                        if (is_curr_keyword(p)) {
                            isSentenceValid = 0;
                            break;
                        }

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

                        if (is_valid_digit_number(curr)) {
                            isSentenceValid = 0;
                            break;
                        }

                        conditions[conditionCount][3] = (char **) &objects[objectCount];
                        conditions[conditionCount][4] = (char **) &objects[objectCount];

                        objects[objectCount][0] = curr;
                        objects[objectCount][1] = curr;

                        objectCount++;


                        curr = tokens[++j];//will be equal to "and" if possible
                    }

                    isSentenceValid = 1;
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
                    else {
                        isSentenceValid = 0;
                        break;
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

        //use isSentenceValid to check if the input is correct
        //use isQuestion to check if it is a question
        //      use isWhoAt|... to check question type, then use the according the question array (e.g. totalItemQuestion[3])
        //if not question, use sentences[][] to retrieve terminals
        
    }
}
