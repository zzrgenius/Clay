/*
"Pixel" Firmware, Rendition 2
Authors: Michael Gubbels
*/

#include <Wire.h>
#include <SoftwareSerial.h>
#include <RadioBlock.h>
#include <SPI.h>

#include "Light.h"
#include "Gesture.h"
#include "Movement.h"
#include "Communication.h"

/**
 * Module configuration
 */

//boolean hasCounter = false;

//            _               
//           | |              
//   ___  ___| |_ _   _ _ __  
//  / __|/ _ \ __| | | | '_ \ 
//  \__ \  __/ |_| |_| | |_) |
//  |___/\___|\__|\__,_| .__/ 
//                     | |    
//                     |_|    

void setup() {
  
  // Initialize pseudorandom number generator
  randomSeed(analogRead(0));

  // Initialize module's color
  setModuleColor(random(256), random(256), random(256)); // Set the module's default color
  // setModuleColor(205, 205, 205); // Set the module's default color
  
  // Assign the module a unique color
  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);

  // Fade on the module to let people know it's alive!
//  fadeOn();
//  fadeOff();
  
  // Setup mesh networking peripherals (i.e., RadioBlocks)
  setupCommunication();
  
  //
  // Setup serial communication (for debugging)
  //
  
  Serial.begin(9600);
  Serial.println(F("Pixel 2014.04.01.23.54.14"));
  
  // Setup physical orientation sensing peripherals (i.e., IMU)
  setupOrientationSensing();
  
  // Flash RGB LEDs
  ledOn();  delay(100);
  ledOff(); delay(100);
  ledOn();  delay(100);
  ledOff(); delay(100);
  ledOn();  delay(100);
  ledOff();
}

//   _                   
//  | |                  
//  | | ___   ___  _ __  
//  | |/ _ \ / _ \| '_ \ 
//  | | (_) | (_) | |_) |
//  |_|\___/ \___/| .__/ 
//                | |    
//                |_|    

boolean awaitingNextModule = false;
boolean awaitingPreviousModule = false;
boolean awaitingNextModuleConfirm = false;
boolean awaitingPreviousModuleConfirm = false;
unsigned long awaitingNextModuleStartTime = 0;
unsigned long awaitingPreviousModuleStartTime = 0;

boolean hasGestureProcessed = false;

void loop() {
  
  // TODO: Add "getPins" function to read the state of pins, store the state of the pins, and handle interfacing with the pins (reading, writing), based on the program running (in both Looper and Mover).
  // TODO: Write code to allow Processing sketch (or other software) to automatically connect to this serial port. Send a "waiting for connection" signal one per second (or thereabout).
  
//  if (getPreviousModuleCount()) {
//    Serial.print("Previous modules: ");
//    Serial.print(getPreviousModuleCount());
//    Serial.println();
//  }
//  
//  if (getNextModuleCount()) {
//    Serial.print("Next modules: ");
//    Serial.print(getNextModuleCount());
//    Serial.println();
//  }

  // Change color/light if needed
  if (crossfadeStep < 256) {
    applyColor();
  }
  
  //
  // Get data from mesh network
  //
  
  boolean hasReceivedMeshData = false;
  hasReceivedMeshData = receiveMeshData();
  
  //
  // Sense gesture (and phsyical orientation, generally)
  //
  
  boolean hasGestureChanged = false;
  if (senseOrientation()) {
    storeData();
    
    // Classify live gesture sample
    unsigned long currentTime = millis();
    if (currentTime - lastGestureClassificationTime >= gestureSustainDuration[classifiedGestureIndex]) { // Check if gesture duration has expired
      classifiedGestureIndex = classifyGestureFromTransitions(); // (gestureCandidate);
      
      lastGestureClassificationTime = millis(); // Update time of most recent gesture classification
    }
    
    // Update current gesture (if it has changed)
    if (classifiedGestureIndex != previousClassifiedGestureIndex) {
      Serial.print("Detected gesture: ");
      Serial.print(gestureName[classifiedGestureIndex]);
      Serial.println();
      
      // Update the previous gesture to the current gesture
      previousClassifiedGestureIndex = classifiedGestureIndex;
      
      // Indicate that the gesture has changed
      hasGestureChanged = true;
      hasGestureProcessed = false;
      
      // TODO: Process newly classified gesture
      // TODO: Make sure the transition can happen (with respect to timing, "transition cooldown")
    }
  }
  
  //
  // Gesture Interpreter:
  // Process gestures. Perform the action associated with the gesture.
  //
  
  // Process current gesture (if it hasn't been processed yet)
  if (hasGestureChanged) { // Only executed when the gesture has changed
    if (!hasGestureProcessed) { // Only executed when the gesture hasn't yet been processed
      
      // Handle gesture
      if (classifiedGestureIndex == 0) { // Check if gesture is "at rest, on table"
        handleGestureAtRestOnTable();
      } else if (classifiedGestureIndex == 1) { // Check if gesture is "at rest, in hand"
        handleGestureAtRestInHand();
      } else if (classifiedGestureIndex == 2) { // Check if gesture is "pick up"
        handleGesturePickUp();
      } else if (classifiedGestureIndex == 3) { // Check if gesture is "place down"
        handleGesturePlaceDown();
      } else if (classifiedGestureIndex == 4) { // Check if gesture is "tilt left"
        handleGestureTiltLeft();
      } else if (classifiedGestureIndex == 5) { // Check if gesture is "tilt right"
        handleGestureTiltRight();
      } else if (classifiedGestureIndex == 6) { // Check if gesture is "shake"
        handleGestureShake();
      } else if (classifiedGestureIndex == 7) { // Check if gesture is "tap to another, as left"
        handleGestureTapToAnotherAsLeft();    
      } else if (classifiedGestureIndex == 8) { // Check if gesture is "tap to another, as right"
        handleGestureTapToAnotherAsRight();
      }
      
      hasGestureProcessed = true; // Set flag indicating gesture has been processed
    }
  }
  
  //
  // Process incoming messages in queue (if any)
  //
  
  if (incomingMessageQueueSize > 0) {
    Message message = dequeueIncomingMeshMessage();
    
    Serial.print("Received ");
    Serial.print(message.message);
    Serial.print(" from module ");
    Serial.print(message.source);
    Serial.print(" (of ");
    Serial.print(incomingMessageQueueSize);
    Serial.print(")\n");
    
    //
    // Process received messages
    //
    
    if (message.message == ANNOUNCE_GESTURE_TAP_AS_LEFT) { // Sequence: Sequencing request (i.e., linking) confirmation, from "right" module
      // If receive "tap to another, as right", then check if this module performed "tap to another, as left" recently. If so, link the modules in a sequence, starting with this module first.
      
      if (awaitingNextModule) {
        
        // Update message state
        awaitingNextModule = false;
        awaitingNextModuleConfirm = true;
        
        // Send ACK message to message.source to confirm linking operation
//        addMessage(message.source, 13);
        addBroadcast(REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT);
      }
      
    } else if (message.message == REQUEST_CONFIRM_GESTURE_TAP_AS_LEFT) { // Sequence: Sequencing (i.e., linking) confirmation, from "right" module
    
      Serial.println(">>>> PROCESSING MESSAGE 14");
        
//        unsigned long currentTime = millis();
//        if (currentTime - awaitingNextModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
//          awaitingNextModuleConfirm = false;
//        }
        
      // Send ACK message to message.source to confirm linking operation (if not yet done)
      if (awaitingNextModuleConfirm) {
        addBroadcast(CONFIRM_GESTURE_TAP_AS_LEFT);
//      addMessage(message.source, 14);

        awaitingNextModule = false;
        awaitingNextModuleConfirm = false;
        
        addNextModule(message.source);
      
        setModuleColor(255, 255, 255);
        setColor(255, 255, 255);
      }
      
    } else if (message.message == CONFIRM_GESTURE_TAP_AS_LEFT) { // Sequence: Sequencing (i.e., linking) confirmation, from "right" module
    
      // TODO:
    
    } else if (message.message == ANNOUNCE_GESTURE_TAP_AS_RIGHT) { // Sequence: Sequencing request (i.e., linking) confirmation, from "left" module
      // If receive "tap to another, as left", then check if this module performed "tap to another, as right" recently. If so, link the modules in a sequence, starting with the other module first.
      
      if (awaitingPreviousModule) {
        
        // Update message state
        awaitingPreviousModule = false;
        awaitingPreviousModuleConfirm = true;
        
        // Send ACK message to message.source to confirm linking operation
        addBroadcast(REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT);
        // addMessage(message.source, 13);
      }
      
    } else if (message.message == REQUEST_CONFIRM_GESTURE_TAP_AS_RIGHT) { // Sequence: Sequencing (i.e., linking) confirmation, from "left" module
      
      Serial.println(">>>> PROCESSING MESSAGE 13");
      
//      unsigned long currentTime = millis();
//      if (currentTime - awaitingNextModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
//        awaitingNextModuleConfirm = false;
//      }
        
      // Send ACK message to message.source to confirm linking operation (if not yet done)
      if (awaitingPreviousModuleConfirm) {
        addBroadcast(CONFIRM_GESTURE_TAP_AS_RIGHT);
//      addMessage(message.source, 14);

        awaitingPreviousModule = false;
        awaitingPreviousModuleConfirm = false;
        
        addPreviousModule(message.source);
      
        setModuleColor(255, 255, 255);
        setColor(255, 255, 255);
      }

    } else if (message.message == 20) { // (Received) Sequence: Activate Module (because receiving a sequence iterator)
    
//      Serial.println(">>>> PROCESSING MESSAGE 20");
//        
////        unsigned long currentTime = millis();
////        if (currentTime - awaitingNextModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
////          awaitingNextModuleConfirm = false;
////        }
//        
//      // Send ACK message to message.source to confirm linking operation (if not yet done)
//      if (awaitingNextModuleConfirm) {
//        addBroadcast(13);
////      addMessage(message.source, 14);
//
//        awaitingNextModule = false;
//        awaitingNextModuleConfirm = false;
//        
//        addNextModule(message.source);
//      
//        setModuleColor(255, 255, 255);
//        setColor(255, 255, 255);
//      }
    }
    
    // TODO: Deactivate module (because it's passing a sequence iterator forward)
    // TODO: Module announces removal from sequence (previous and next)
  }
  
  //
  // Update state of current module
  //
  
//  //if (awaitingNextModuleConfirm) {
//  if (awaitingNextModule) {
//    unsigned long currentTime = millis();
//    if (currentTime - awaitingNextModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
//      awaitingNextModule = false;
//      awaitingNextModuleConfirm = false;
//    }
//  }
//  
//  //if (awaitingPreviousModuleConfirm) {
//  if (awaitingPreviousModule) {
//    unsigned long currentTime = millis();
//    if (currentTime - awaitingPreviousModuleStartTime > SEQUENCE_REQUEST_TIMEOUT) {
//      awaitingPreviousModule = false;
////      awaitingPreviousModuleConfirm = false;
//    }
//  }
  
  //
  // Send outgoing messages (e.g., this module's updated gesture)
  //
    
  unsigned long currentTime = millis();
  if (currentTime - lastMessageSendTime > RADIOBLOCK_PACKET_WRITE_TIMEOUT) {
  
    // Process mesh message queue  
    if (messageQueueSize > 0) {
      sendMessage();
    }
    
    // Update the time that a message was most-recently dispatched
    lastMessageSendTime = millis();
  }
}

/**
 * Handle "at rest, on table" gesture.
 */
boolean handleGestureAtRestOnTable() {
  setColor(0.3 * defaultModuleColor[0], 0.3 * defaultModuleColor[1], 0.3 * defaultModuleColor[2]);
  
  addBroadcast(1);
}

/**
 * Handle "at rest, in hand" gesture.
 */
boolean handleGestureAtRestInHand() {
  setColor(defaultModuleColor[0], defaultModuleColor[1], defaultModuleColor[2]);
  
  addBroadcast(2);
}

/**
 * Handle "pick up" gesture.
 */
boolean handleGesturePickUp() {
  addBroadcast(3);
}

/**
 * Handle "place down" gesture.
 */
boolean handleGesturePlaceDown() {
  addBroadcast(4);
}

/**
 * Handle "tilt left" gesture.
 */
boolean handleGestureTiltLeft() {
  setColor(0, 0, 255);
  
  addBroadcast(ANNOUNCE_GESTURE_TILT_LEFT);
}

/**
 * Handle "tilt right" gesture.
 */
boolean handleGestureTiltRight() {
  setColor(0, 255, 0);
  
  addBroadcast(ANNOUNCE_GESTURE_TILT_RIGHT);
}

/**
 * Handle "shake" gesture.
 */
boolean handleGestureShake() {
  setColor(255, 0, 0);
  
  // TODO: Message next modules, say this module is leaving the sequence
  // TODO: Message previous module, say this module is leaving the sequence
  
  addBroadcast(ANNOUNCE_GESTURE_SHAKE);
  
  removePreviousModules();
  removeNextModules();
}

/**
 * Handle "tap to another, as left" gesture.
 */
boolean handleGestureTapToAnotherAsLeft() {
  setColor(255, 0, 0);
  
  addBroadcast(ANNOUNCE_GESTURE_TAP_AS_LEFT);
  
  awaitingNextModule = true;
  awaitingNextModuleConfirm = false;
  awaitingNextModuleStartTime = millis();
}

/**
 * Handle "tap to another, as right" gesture.
 */
boolean handleGestureTapToAnotherAsRight() {
  setColor(255, 0, 0);
  
  // Send to all linked devices
//      for (int i = 0; i < 1; i++) {
//          // Set the destination address
//          interface.setupMessage(next[i]);
//  
//          // Package the data payload for transmission
//          interface.addData(1, (byte) 0x1F); // TYPE_INT8
//          interface.sendMessage(); // Send data OTA
//  
//          // Wait for confirmation
//          // delayUntilConfirmation();
//      }

  addBroadcast(ANNOUNCE_GESTURE_TAP_AS_RIGHT);
  
  awaitingPreviousModule = true;
  awaitingPreviousModuleConfirm = false;
  awaitingPreviousModuleStartTime = millis();
}
