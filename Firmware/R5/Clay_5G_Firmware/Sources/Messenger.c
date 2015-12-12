/*
 * Interpreter.c
 *
 *  Created on: Sep 24, 2015
 *      Author: mokogobo
 */

#include "Messenger.h"

//Message *incomingMessageQueue = NULL;
//Message *outgoingMessageQueue = NULL;

char messageUuidBuffer[DEFAULT_UUID_LENGTH] = { 0 };
char grammarSymbolBuffer[MAXIMUM_GRAMMAR_SYMBOL_LENGTH] = { 0 };
char uuidBuffer[DEFAULT_UUID_LENGTH] = { 0 };
char behaviorDescriptionBuffer[MAXIMUM_GRAMMAR_SYMBOL_LENGTH] = { 0 };

uint8_t Initialize_Incoming_Message_Queue () {
	incomingMessageQueue = NULL;
	return TRUE;
}

Message* Create_Message (const char *content) {
	
	// Allocate memory for message structure.
	Message *message = (Message *) malloc (sizeof (Message));
	
	// Allocate memory for the message's content.
	(*message).content = (char *) malloc (strlen (content));
	
	// Copy message content
	strcpy ((*message).content, content);
	
	// Set up links for queue
	(*message).previous = NULL;
	(*message).next = NULL;
	
	return message;
}

int8_t Delete_Message (Message *message) {
	
	if (message != NULL) {
		
		// TODO: Remove references to the message in the queue.
		
		// Free the message's content from memory
		free ((*message).content);
		(*message).content = NULL;
		
		// Free the message from memory
		free (message);
		message = NULL;
	}
}

int16_t Queue_Incoming_Message (Message *message) {
	
	Message *lastMessage = NULL;
	uint16_t messageCount = 0;
	
	if (incomingMessageQueue == NULL) {
		
		// The queue is empty, so add it to the queue as the only element.
		
		incomingMessageQueue = message;
		
		(*message).previous = NULL;
		(*message).next = NULL;
		
		messageCount = 1;
		
	} else {
		
		// Search for the last element in the queue.
		lastMessage = incomingMessageQueue; // Get the front of the queue.
		messageCount++;
		while ((*lastMessage).previous != NULL) {
			lastMessage = (*lastMessage).previous;
			messageCount++;
		}
		
		// Update the linked list to add the message to the back of the queue.
		// (*message).previous = NULL; // NOTE: This should already be NULL at this point, so this is redundant, but adds some degree of robustness.
		(*message).next = lastMessage;
		(*lastMessage).previous = message;
		messageCount++;
		
	}
	
	return messageCount;
}

Message* Dequeue_Incoming_Message () {
	
	Message *message = NULL;
	
	if (incomingMessageQueue != NULL) {
		
		// Reference the message at the front of the queue.
		message = incomingMessageQueue;
		
		// Update the linked list to remove the message from the front of the queue.
		if ((*message).previous != NULL) {
			
			// There are additional messages on the queue. Set the previous element to the front of the queue.
			
			incomingMessageQueue = (*message).previous;
			// incomingMessageQueue = (*message).next; // NOTE: This should already be NULL at this point, so this is redundant, but adds some degree of robustness.
			(*incomingMessageQueue).next = NULL; // Set as the first element in the queue.
			
			// Unlink the message from linked list to finish dequeuing process.
			(*message).previous = NULL;
			(*message).next = NULL;
			
		} else {
			
			// There are no more messages in the queue, so remove links.
			
			incomingMessageQueue = NULL; // Remove the link to any message at the front of the queue.
			
			// Unlink the message from linked list to finish dequeuing process.
			(*message).previous = NULL;
			(*message).next = NULL;
			
		}
	}
	
	return message;
}

int8_t Has_Incoming_Message () {
	
	if (incomingMessageQueue != NULL) {
		if ((*incomingMessageQueue).content != NULL) {
			return TRUE;
		}
	}
	
	return FALSE;
}

int8_t Process_Incoming_Message (Message *message) {
	
	int8_t status = NULL;
	int8_t result = NULL;
	char token[MAXIMUM_MESSAGE_LENGTH] = { 0 };
	int tokenInt = 0;
	int i;
	
	char *messageContent = NULL;
	messageContent = (*message).content;
	
//	getToken (messageContent, token, 0)
	// TODO: char* tokenX = tokenizeString (char *string, int stringLength, char separator) --OR-- tokenizeString (char *string, char separator, token1, token2, token3, ...)
	
	if ((status = getToken (messageContent, token, 0)) != NULL) { // status = getToken (message, token, 0);

		// create behavior
		// ^
		if (strncmp (token, "create", strlen ("create")) == 0) {
			
			if ((status = getToken (messageContent, token, 1)) != NULL) {
			
				// create behavior <uuid> "turn lights on"
				//        ^
				if (strncmp (token, "behavior", strlen ("behavior")) == 0) {
					
					// Get UUID (parameter index 2)
					status = getToken (messageContent, uuidBuffer, 2);

					// Get command (parenthesized string index 3)
					status = getToken (messageContent, behaviorDescriptionBuffer, 3);
					
					// TODO: Send the acknowledgment .
					// TODO: Queue_Outgoing_Message (i.e., "got <message-uuid> <message-content>")
					// TODO: Send_UDP_Message ();
					strncpy (token, "got ", 4);
					strncpy (token + 4, messageContent, strlen (messageContent));
					// TODO: Queue the outgoing UDP message!
					Broadcast_UDP_Message (token, DISCOVERY_BROADCAST_PORT); // Broadcast_UDP_Message (token, UDP_SERVER_PORT);
					// TODO: Queue the message rather than executing it immediately (unless specified)
					// TODO: Parse the message rather than brute force like this.
					
					// Delete the message
					if (message != NULL) {
						Delete_Message (message);
					}
					
					// Check if the behavior is already in the cache. If nay, cache it!
					if (Has_Cached_Behavior_By_UUID (uuidBuffer) == FALSE) {
						
						// TODO: Get available memory and only create the behavior if there's enough memory. Otherwise, move something out of memory to make room for it (and stream the moved thing back in when it's needed).
						
						// Parse the message content and perform the corresponding behavior operation
						Behavior *behavior = Create_Behavior (uuidBuffer, behaviorDescriptionBuffer);
						if (behavior != NULL) {
							// NOTE: The behavior was successfully created.
							// Add the behavior to the local cache!
							Cache_Behavior (behavior);
							result = TRUE;
						}
						
					} else {
						
						// The behavior construct already exists in the cache. There's no need to create it!
						return TRUE;
						
					}
					
					// TODO: Send the acknowledgment.
					// TODO: Queue_Outgoing_Message (i.e., "got <message-uuid> <message-content>")
					// TODO: Send_UDP_Message ();
					strncpy (token, "got ", 4);
					strncpy (token + 4, messageContent, strlen (messageContent));
					// TODO: Queue the outgoing UDP message!
					Broadcast_UDP_Message (token, DISCOVERY_BROADCAST_PORT); // Broadcast_UDP_Message (token, UDP_SERVER_PORT);
					
					// TODO: Queue the message rather than executing it immediately (unless specified)
					// TODO: Parse the message rather than brute force like this.
					
				}
				
				/*
				// create loop <uuid>
				//        ^
				else if (strncmp (token, "loop", strlen ("loop")) == 0) {
					
					// TODO:
					
				}
				*/
				
			}
			
		} else if (strncmp (token, "add", strlen ("add")) == 0) {
			
			if ((status = getToken (messageContent, token, 1)) != NULL) {
		
				// add behavior <uuid> [to loop <uuid>]
				//     ^
				if (strncmp (token, "behavior", strlen ("behavior")) == 0) {
					
					// TODO: Check for "add behavior <uuid> (to loop <loop-uuid>) after <other-uuid>"
				
					// Get UUID (parameter index 2)
					status = getToken (messageContent, uuidBuffer, 2);
					
					// TODO: Send the acknowledgment .
					// TODO: Queue_Outgoing_Message (i.e., "got <message-uuid> <message-content>")
					// TODO: Send_UDP_Message ();
					strncpy (token, "got ", 4);
					strncpy (token + 4, messageContent, strlen (messageContent));
					// TODO: Queue the outgoing UDP message!
					Broadcast_UDP_Message (token, DISCOVERY_BROADCAST_PORT); // Broadcast_UDP_Message (token, UDP_SERVER_PORT);
					// TODO: Queue the message rather than executing it immediately (unless specified)
					// TODO: Parse the message rather than brute force like this.
					
					// Delete the message
					if (message != NULL) {
						Delete_Message (message);
					}
					
					// Check if the behavior is already in the cache. If nay, cache it!
					if (Has_Cached_Behavior_By_UUID (uuidBuffer) == TRUE) {
						
						// TODO: Only call either Get_Cached_Behavior_By_UUID. Don't call both Has_Cached_Behavior_By_UUID and Get_Cached_Behavior_By_UUID. They do the same search work. Don't search multiple times for no reason during behavior construct recall!
						
						// Parse the message content and perform the corresponding behavior operation
						Behavior *behavior = Get_Cached_Behavior_By_UUID (uuidBuffer);
						if (behavior != NULL) {
							// NOTE: The behavior was successfully created.
							// Add the behavior to the local cache!
							Add_Behavior (behavior);
							result = TRUE;
						}
						
					} else {
						
						// TODO: The behavior is not in the cache! Return response indicating this! Or request it from the cloud!
						
						result = FALSE;
						
					}
					
				}
			
			}
			
		} else if (strncmp (token, "remove", strlen ("remove")) == 0) { 
			
			if ((status = getToken (messageContent, token, 1)) != NULL) {
		
				// add behavior <uuid> [to loop <uuid>]
				//     ^
				if (strncmp (token, "behavior", strlen ("behavior")) == 0) {
				
					// Get UUID (parameter index 2)
					status = getToken (messageContent, uuidBuffer, 2);
					
					// TODO: Send the acknowledgment .
					// TODO: Queue_Outgoing_Message (i.e., "got <message-uuid> <message-content>")
					// TODO: Send_UDP_Message ();
					strncpy (token, "got ", 4);
					strncpy (token + 4, messageContent, strlen (messageContent));
					// TODO: Queue the outgoing UDP message!
					Broadcast_UDP_Message (token, DISCOVERY_BROADCAST_PORT); // Broadcast_UDP_Message (token, UDP_SERVER_PORT);
					// TODO: Queue the message rather than executing it immediately (unless specified)
					// TODO: Parse the message rather than brute force like this.
					
					// Delete the message
					if (message != NULL) {
						Delete_Message (message);
					}
					
					// Check if the behavior is already in the cache. If nay, cache it!
					if (Has_Behavior_By_UUID (uuidBuffer) == TRUE) {
						
						// TODO: Only call either Get_Cached_Behavior_By_UUID. Don't call both Has_Cached_Behavior_By_UUID and Get_Cached_Behavior_By_UUID. They do the same search work. Don't search multiple times for no reason during behavior construct recall!
						
						// Parse the message content and perform the corresponding behavior operation
						Behavior *behavior = Get_Behavior_By_UUID (uuidBuffer);
						if (behavior != NULL) {
							// NOTE: The behavior was successfully created.
							// Add the behavior to the local cache!
							Remove_Behavior (behavior);
							result = TRUE;
						}
						
					} else {
						
						// TODO: The behavior is not in the cache! Return response indicating this! Or request it from the cloud!
						
						result = FALSE;
						
					}
					
				}
			
			}
			
		} else if (strncmp (token, "reset", strlen ("reset")) == 0) {
			
			// HACK: Remove the "reset" command. This is used for testing!
			
			// Remove behaviors constructs from loop
			// Delete behaviors
			// Set loop to null
			// Delete behaviors
			// Remove behavior constructs from cache
			// Set cache to null
			
			// Delete the message
			if (message != NULL) {
				Delete_Message (message);
			}
			
			// TODO: Send the acknowledgment .
			// TODO: Queue_Outgoing_Message (i.e., "got <message-uuid> <message-content>")
			// TODO: Send_UDP_Message ();
			strncpy (token, "got ", 4);
			strncpy (token + 4, messageContent, strlen (messageContent));
			// TODO: Queue the outgoing UDP message!
			Broadcast_UDP_Message (token, DISCOVERY_BROADCAST_PORT); // Broadcast_UDP_Message (token, UDP_SERVER_PORT);
			// TODO: Queue the message rather than executing it immediately (unless specified)
			// TODO: Parse the message rather than brute force like this.
			
			result = TRUE;
			
		}
			
	}
	
	// TODO: Store message UUID for use in message acknowledgment protocol. If it has been received, then don't apply it again, just send the acknowledgment packet.
	
	/*
	if ((status = getToken (messageContent, token, 0)) != NULL) { // status = getToken (message, token, 0);

		if (strncmp (token, "turn", strlen ("turn")) == 0) {
			
			if ((status = getToken (messageContent, token, 1)) != NULL) {
			
				// turn lights on
				//      ^
				if (strncmp (token, "lights", strlen ("lights")) == 0) {
					
					if ((status = getToken (messageContent, token, 2)) != NULL) { // TODO: Remove this! The parameter is used later.
						
						// turn lights on
						//             ^
						if (strncmp (token, "on", strlen ("on")) == 0) {
							
							// Turn all LEDs on
							for (i = 0; i < 12; i++) {
								Set_Channel_State (i, ON_CHANNEL);
							}
							
							result = TRUE;
							
						} else if (strncmp (token, "off", strlen ("off")) == 0) {
							
							// Turn all LEDs off
							for (i = 0; i < 12; i++) {
								Set_Channel_State (i, OFF_CHANNEL);
							}
							
							result = TRUE;
							
						}
					}
				
				// turn light 1 on
				//      ^
				} else if (strncmp (token, "light", strlen ("light")) == 0) {
					
					if ((status = getToken (messageContent, token, 2)) != NULL) { // TODO: Remove this! The parameter is used later.
						
						if ((status = getToken (messageContent, token, 3)) != NULL) {
							
							// turn light 1 on
							//              ^
							if (strncmp (token, "on", strlen ("on")) == 0) {
								
								// Turn the specified LED off
								status = getToken (messageContent, token, 2);
								tokenInt = atoi (token);
								Set_LED_Output ((RGB_LED) tokenInt,  &onColor);
								
								result = TRUE;
								
							} else if (strncmp (token, "off", strlen ("off")) == 0) {
								
								// Turn the specified LED off
								status = getToken (messageContent, token, 2);
								tokenInt = atoi (token);
								Set_LED_Output ((RGB_LED) tokenInt,  &offColor);
								
								result = TRUE;
								
							}
						}
					}
					
				// turn channel 1 on
				//      ^
				} else if (strncmp (token, "channel", strlen ("channel")) == 0) {
					
					if ((status = getToken (messageContent, token, 2)) != NULL) { // TODO: Remove this! The parameter is used later.
						
						// turn channel 1 on
						//              ^
						
						if ((status = getToken (messageContent, token, 3)) != NULL) {
							
							// turn channel 1 on
							//                ^
							if (strncmp (token, "on", strlen ("on")) == 0) {
								
								// Turn the specified LED off
								status = getToken (messageContent, token, 2);
								tokenInt = atoi (token);
								Set_Channel_State (tokenInt, ON_CHANNEL);
								
								result = TRUE;
								
							} else if (strncmp (token, "off", strlen ("off")) == 0) {
								
								// Turn the specified LED off
								status = getToken (messageContent, token, 2);
								tokenInt = atoi (token);
								Set_Channel_State (tokenInt, OFF_CHANNEL);
								
								result = TRUE;
								
							}
						}
					}
					
				}
				
			}
			
		} else if (strncmp (token, "wait", strlen ("wait")) == 0) {
			
			Wait (1000);
					
//			if ((status = getToken (behaviorContent, token, 1)) != NULL) {
//			
//				// turn lights on
//				//      ^
//				if (strncmp (token, "lights", strlen ("lights")) == 0) {
//					
//				}
//			}
			
		} else if (strncmp (token, "ping", strlen ("ping")) == 0) {
			
			if ((status = getToken (messageContent, token, 1)) != NULL) {
			
				// turn lights on
				//      ^
				if (strncmp (token, "lights", strlen ("lights")) == 0) {
				
					// Queue_Outgoing_Message ("pong <ip>"); // TODO: Send this via UDP datagram.
					// Send_UDP_Message()
//					printf("got ping\r\n");
					Broadcast_UDP_Message ("pong <ip>");
					
					result = TRUE;
					
				}
				
			}
			
		} else {
//			D(printf ("WTFping\r\n"));
		}
		
	} else {
//		D(printf ("status = %d\r\n", status));
	}
	*/
	
	return result;
}