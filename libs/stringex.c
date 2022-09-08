#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void substring(char *original, char *newS, int start, int end) {
    if (end > strlen(original)) {
        end = strlen(original);
    }
    if (end > start) {
        char temp;
        for (int i = start ; i < end ; i++) {
            temp = original[i];
            strncat(newS, &temp, 1);
        }
    }
}

int findChar(char *str, char c) {
	for (int i = 0 ; i < strlen(str) ; i++){
		if (str[i] == c) {
			return i;
		}
	}
	return 0;
}

int countChar(char *str, char c) {
	int count = 0;

	for (int i = 0 ; i < strlen(str) ; i++){
		if (str[i] == c) {
			count ++;
		}
	}
	return count;

}
