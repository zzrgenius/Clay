/*
 * String.c
 *
 *  Created on: Sep 22, 2015
 *      Author: mokogobo
 */

#include <cstring>
#include "String.h"
#include "PE_Types.h"

int getTokenCount (const char *string) {
	// TODO: Don't count the delimiters between tokens enclosed by the secondary delimiter (i.e., the " character).
	
	int i = 0;
	int tokenCount = 0;
	uint8_t escape = FALSE; // Is the current character within an "escaped" token, delimited by the secondary delimiter (i.e., the " character)?
	
	for (i = 0; i < std::strlen (string); i++) {
		
		// Check if the character is the secondary token delimiter, which escapes counting the primary delimiter.
		if (string[i] == DEFAULT_TOKEN_SECONDARY_DELIMIETER) {
			escape = (escape == FALSE ? TRUE : FALSE);
		}
		
		// Count the non-escaped primary delimiters.
		if (string[i] == DEFAULT_TOKEN_DELIMIETER) {
			if (escape == FALSE) {
				tokenCount++;
			}
		}
	}
	tokenCount++;
	
	return tokenCount;
	
}

int8_t getToken (const char *string, char *tokenBuffer, int tokenIndex) {
	
	int i = 0;
	const char *token = NULL;
	const char *tokenStop = NULL;
	
	if (string != NULL && tokenBuffer != NULL) {
		
		// Handle case when tokenIndex is 0
		if (tokenIndex == 0) {
			
			token = string;
			tokenStop = std::strchr (token, ' '); // i.e., the case for a multi-token message (e.g., "turn light 1 on")
			if (tokenStop == NULL) {
				tokenStop = token + std::strlen (token); // i.e., the case for a single-token message
			}
			
		} else {
		
			// Iterate to the specified token index
			token = string;
			for (i = 0; i < tokenIndex; i++) {
				token = std::strchr (token, DEFAULT_TOKEN_DELIMIETER);
				if (token != NULL) {
					token = token + 1; // Move past the delimiter to the first character of the token.
					if (token[0] == DEFAULT_TOKEN_SECONDARY_DELIMIETER) {
						tokenStop = std::strchr (token + 1, DEFAULT_TOKEN_SECONDARY_DELIMIETER) + 1;
					} else {
						tokenStop = std::strchr (token, ' ');
					}
					
					if (tokenStop == NULL) {
						tokenStop = string + std::strlen (string); // Go to the end of the string (i.e., to the terminating '\0' character for the string).
					}
					
				} else {
					tokenStop = NULL;
				}
			}
		}
		
		// Copy the string into the specified buffer
		if (token != NULL && tokenStop != NULL) {
			
			if (token[0] == DEFAULT_TOKEN_SECONDARY_DELIMIETER) {
			    std::strncpy (tokenBuffer, token + 1, tokenStop - token - 2);
			} else {
			    std::strncpy (tokenBuffer, token, tokenStop - token);
			}
			tokenBuffer[tokenStop - token] = '\0'; // Terminate the string in the token buffer.
			
		}
		
		return TRUE; // (tokenStop - token); // Return the length of the token
		
	}
	
	return NULL;
}
