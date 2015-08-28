/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : RxBuf.c
**     Project     : clay_firmware
**     Processor   : MK20DX256VLL7
**     Component   : RingBufferUInt8
**     Version     : Component 01.003, Driver 01.00, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-08-21, 02:14, # CodeGen: 1
**     Abstract    :
**
**     Settings    :
**
**     Contents    :
**         Put             - byte RxBuf_Put(byte elem);
**         Get             - byte RxBuf_Get(byte *elemP);
**         NofElements     - byte RxBuf_NofElements(void);
**         NofFreeElements - byte RxBuf_NofFreeElements(void);
**         Init            - void RxBuf_Init(void);
**
**     License   :  Open Source (LGPL)
**     Copyright : (c) Copyright Erich Styger, 2012, all rights reserved.
**     This an open source software of an embedded component for Processor Expert.
**     This is a free software and is opened for education,  research  and commercial developments under license policy of following terms:
**     * This is a free software and there is NO WARRANTY.
**     * No restriction on use. You can use, modify and redistribute it for personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
**     * Redistributions of source code must retain the above copyright notice.
** ###################################################################*/
/*!
** @file RxBuf.c
** @version 01.00
** @brief
**
*/         
/*!
**  @addtogroup RxBuf_module RxBuf module documentation
**  @{
*/         

/* MODULE RxBuf. */

#include "RxBuf.h"

static uint8_t RxBuf_buffer[RxBuf_BUF_SIZE]; /* ring buffer */
static uint8_t RxBuf_inIdx;  /* input index */
static uint8_t RxBuf_outIdx; /* output index */
static uint8_t RxBuf_inSize; /* size of input data */
/*
** ===================================================================
**     Method      :  RxBuf_Put (component RingBufferUInt8)
**     Description :
**         Puts a new element into the buffer
**     Parameters  :
**         NAME            - DESCRIPTION
**         elem            - New element to be put into the buffer
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
byte RxBuf_Put(byte elem)
{
  byte res = ERR_OK;

  EnterCritical();
  if (RxBuf_inSize==RxBuf_BUF_SIZE) {
    res = ERR_TXFULL;
  } else {
    RxBuf_buffer[RxBuf_inIdx] = elem;
    RxBuf_inSize++;
    RxBuf_inIdx++;
    if (RxBuf_inIdx==RxBuf_BUF_SIZE) {
      RxBuf_inIdx = 0;
    }
  }
  ExitCritical();
  return res;
}

/*
** ===================================================================
**     Method      :  RxBuf_Get (component RingBufferUInt8)
**     Description :
**         Removes an element from the buffer
**     Parameters  :
**         NAME            - DESCRIPTION
**       * elemP           - Pointer to where to store the received
**                           element
**     Returns     :
**         ---             - Error code
** ===================================================================
*/
byte RxBuf_Get(byte *elemP)
{
  byte res = ERR_OK;

  EnterCritical();
  if (RxBuf_inSize==0) {
    res = ERR_RXEMPTY;
  } else {
    *elemP = RxBuf_buffer[RxBuf_outIdx];
    RxBuf_inSize--;
    RxBuf_outIdx++;
    if (RxBuf_outIdx==RxBuf_BUF_SIZE) {
      RxBuf_outIdx = 0;
    }
  }
  ExitCritical();
  return res;
}

/*
** ===================================================================
**     Method      :  RxBuf_NofElements (component RingBufferUInt8)
**     Description :
**         Returns the actual number of elements in the buffer.
**     Parameters  : None
**     Returns     :
**         ---             - Number of elements in the buffer.
** ===================================================================
*/
byte RxBuf_NofElements(void)
{
  return RxBuf_inSize;
}

/*
** ===================================================================
**     Method      :  RxBuf_NofFreeElements (component RingBufferUInt8)
**     Description :
**         Returns the actual number of free elements/space in the
**         buffer.
**     Parameters  : None
**     Returns     :
**         ---             - Number of elements in the buffer.
** ===================================================================
*/
byte RxBuf_NofFreeElements(void)
{
  return (byte)(RxBuf_BUF_SIZE-RxBuf_inSize);
}

/*
** ===================================================================
**     Method      :  RxBuf_Init (component RingBufferUInt8)
**     Description :
**         Initializes the data structure
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void RxBuf_Init(void)
{
  RxBuf_inIdx = 0;
  RxBuf_outIdx = 0;
  RxBuf_inSize = 0;
}

/* END RxBuf. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/