

#include <stdio.h>
#include <stdlib.h>

// need this for false declaration
#include <stdbool.h>
#include "ftd2xx.h"

int main()
{
  
  printf("Start");
  
  FT_HANDLE ftHandle;            // Handle of the FTDI device
  FT_STATUS ftStatus;            // Result of each D2XX call
  DWORD dwNumDevs;               // The number of devices
  unsigned int uiDevIndex = 0xF; // The device in the list that we'll use
  BYTE byOutputBuffer[8];        // Buffer to hold MPSSE commands and data
                                 // to be sent to the FT2232H
  BYTE byInputBuffer[8];         // Buffer to hold data read from the FT2232H
  DWORD dwCount = 0;             // General loop index
  DWORD dwNumBytesToSend = 0;    // Index to the output buffer
  DWORD dwNumBytesSent = 0;      // Count of the actual bytes send - used with the FT_Write
  DWORD dwNumBytesToRead = 0;    // Number of bytes available to read in the driver's input buffer
  DWORD dwNumBytesRead = 0;      // Count of actual bytes read 0 used with FT_Read
  DWORD dwClockDivisor = 0x05DB; // Value of clock divisor, SCL Frequency =
                                 //     60/((1+0x05DB*2) (MHz) = 1Mhz

  // Does an FTDI device exist?

  printf("Checking for FTDI devices...\n");

  ftStatus = FT_CreateDeviceInfoList(&dwNumDevs);
                                 // Get the number of FTDI devices
  if (ftStatus != FT_OK)         // Did the command execute OK?
    {
      printf("Error in getting the number of devices\n");
      return 1;                  // Exit with error
    }

  if (dwNumDevs < 1)             // Exit if we don't see any
    {
      printf("There are no FTDI devices installed\n");
      return 1;                  // Exit with error
    }

  printf("%d FTDI devices found \
         - the count includes individual ports on a single chip\n", dwNumDevs);

  // Open the port we assume port 0

  printf("\nAssume first device has the MPSSE and open it...\n");

  ftStatus = FT_Open(0, &ftHandle);

  if (ftStatus != FT_OK)
    {
      printf("Open Failed with error %d\n", ftStatus);
      return 1;                  // Exit with error
    }

  // Configure port parameters

  printf("\nConfiguring port for MPSSE use...\n");

  ftStatus |= FT_ResetDevice(ftHandle); // Reset USB Device

  if (ftStatus != FT_OK)         // Did the command execute OK?
    {
      printf("ResetDevice did not return properly\n");
      return 1;                  // Exit with error
    }


  // Purge USB receive buffer first by reading out all old data from FT2232H recieve buffer
  ftStatus |= FT_GetQueueStatus(ftHandle, &dwNumBytesToRead);
  // Get the number of bytes in the FT2232H recieve buffer

  if ((ftStatus == FT_OK) && (dwNumBytesToRead > 0))
    FT_Read(ftHandle, &byInputBuffer, dwNumBytesToRead, &dwNumBytesRead);
  // Read out the data from FT2232H receive buffer

  ftStatus |= FT_SetUSBParameters(ftHandle, 65536, 65535);
  // set the USB request transfer sizes to 64k

  ftStatus |= FT_SetChars(ftHandle, false, 0, false, 0);
  // Disable event and error characters

  ftStatus |= FT_SetTimeouts(ftHandle, 0, 5000);
  // Sets the read and write timeouts in milliseconds

  ftStatus |= FT_SetLatencyTimer(ftHandle, 1);
  // Set the latency timer to 1mS (default is 16mS)

  ftStatus |= FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0x00, 0x00);
  // Turn on flow control to synchronize IN requests

  ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x00);
  // Reset controller

  ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x02);
  // Enable MPSSE mode

  if (ftStatus != FT_OK)
    {
      printf("Error in initializing the MPSSE %d\n", ftStatus);
      FT_Close(ftHandle);
      return 1;                   // Exit with error
    }

  // FIX sleep not Sleep
  // this actually fails sometimes - move to 60 sec instead of 50 to see if helps
  // printf("sleep(60)");
  // sleep(50); // Wait for all the USB stuff to complete and work

  printf("sleep(1)\n");
  sleep(1);

  // Enable internal loop-back

  byOutputBuffer[dwNumBytesToSend++] = 0x84; // Enable Loopback

  ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  // send off the loopback command

  dwNumBytesToSend = 0; // Reset output buffer pointer

  printf("sleep(1)\n");
  sleep(1); // Wait for all the USB stuff to complete and work

  // Check the receive buffer - it should be empty
  ftStatus = FT_GetQueueStatus(ftHandle, &dwNumBytesToRead);

  // TODO : missing line in example?
  // ftStatus = FT_Read(ftHandle, &byInputBuffer, dwNumBytesToRead, &dwNumBytesRead);
  // Read out the data from the input buffer

  if (dwNumBytesToRead != 0)
    {
      printf("Error - MPSSE receive buffer should be empty %d - dwNumBytesToRead %d\n", ftStatus, dwNumBytesToRead);
      FT_SetBitMode(ftHandle, 0x0, 0x00);
      // Reset the port to disable MPSSE
      FT_Close(ftHandle); // Close the USB port
      return 1;  // exit with error
    }

  // synchronize the MPSSE by sending a bogus opcode (0xAB),
  // the MPSSE will respond with a "Bad Command" (0xFA) followed by
  // the bogus opcode itself
  
  byOutputBuffer[dwNumBytesToSend++] = 0xAB;
  // Add bogus command 0xAB to the queue

  ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  // Send off the BAD command
  dwNumBytesToSend = 0; // Reset output buffer pointer

  do
    {
      ftStatus = FT_GetQueueStatus(ftHandle, &dwNumBytesToRead);
      // Get the number of bytes in the device input buffer
    } while ((dwNumBytesToRead == 0) && (ftStatus == FT_OK));
  // or Timeout

  bool bCommandEchod = false;

  ftStatus = FT_Read(ftHandle, &byInputBuffer, dwNumBytesToRead, &dwNumBytesRead);
  // Read out the data from the input buffer

  for (dwCount = 0; dwCount < dwNumBytesRead - 1; dwCount++)
    // check if bad command and echo command are received
    {
      if ((byInputBuffer[dwCount] == 0xFA) && (byInputBuffer[dwCount+1] == 0xAB))
	{ bCommandEchod = true;
	  break;
	}
    }
  if (bCommandEchod == false)
    {
      printf("Error in synchronizing the MPSSE\n");
      FT_Close(ftHandle);
      return 1;  // Exit with error
    }

  // Disable internal loop-back

  byOutputBuffer[dwNumBytesToSend++] = 0x85;
  // disable the loopback command 0x85
  ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  // send off the loopback command

  dwNumBytesToSend = 0; // reset output buffer pointer

  // check the receive buffer - it should be empty
  ftStatus = FT_GetQueueStatus(ftHandle, &dwNumBytesToRead);
  // get the number of bytes in the FT2232H receive buffer

  if (dwNumBytesToRead != 0)
    {
      printf("Error - MPSSE receive buffer should be empty\n", ftStatus);
      FT_SetBitMode(ftHandle, 0x0, 0x00);
      // Reset the port to disable MPSSE
      FT_Close(ftHandle); // Close the USB Port
      return 1;
    }

  // Configure the MPSSE settings for JTAG
  // multiple commands can be sent to the MPSSE with one FT_Write

  dwNumBytesToSend = 0; // start with a fresh index

  // set up the hi-speed specific commands for the FTx232h?

  byOutputBuffer[dwNumBytesToSend++] = 0x8A;
  // use a 60Mhz master clock (disable divide by 5)
 
  byOutputBuffer[dwNumBytesToSend++] = 0x97;
  // turn off adaptive clocking (may be needed for ARM)

  byOutputBuffer[dwNumBytesToSend++] = 0x8D;
  // disable three-phase clocking

  ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  // send off the HS-specific commands

  dwNumBytesToSend = 0;

  // Set TCK frequency
  // TCK = 60MHz / ((1 + [(1 + 0xValueH*256) or (0xValueL])*2)
  
  byOutputBuffer[dwNumBytesToSend++] = '\x86';
  // command to set clock divisor

  byOutputBuffer[dwNumBytesToSend++] = dwClockDivisor & 0xFF;
  // set 0xValueL of clock divisor

  byOutputBuffer[dwNumBytesToSend++] = (dwClockDivisor >> 8) & 0xFF;
  // set 0xValueH of clock divisor

  ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  // send off the clock divisor commands

  dwNumBytesToSend = 0; // reset output buffer pointer

  // Set initial states of the MPSSE interface
  // - low byte, both pin directions and output values
  // Pin name Signal Direction Config Initial State Config
  // ADBUS0 TCK/SK output 1 high 1
  // ADBUS1 TDI/DO output 1 low 0
  // ADBUS2 TDO/DI input  0     0
  // ADBUS3 TMS/CS output 1 high 1
  // ADBUS4 GPIOL0 output 1 low 0
  // ADBUS5 GPIOL1 output 1 low 0
  // ADBUS6 GPIOL2 output 1 high 1
  // ADBUS7 GPIOL3 output 1 high 1

  byOutputBuffer[dwNumBytesToSend++] = 0x80;
  // configure data bits low-byte of MPSSE port
  
  byOutputBuffer[dwNumBytesToSend++] = 0xC9;
  // initial state config above

  byOutputBuffer[dwNumBytesToSend++] = 0xFB;
  // direction config above

  ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  // send off the low GPIO config commands

  dwNumBytesToSend = 0;
  // reset output buffer pointer

  // Note that since the data in subsequent sections will be clocked on the rising edge, the
  // inital clock state of high is selected. Clocks will be generated as high-low-high.
  // For example, in this case, data changes on the rising edge to give it enough time
  // to have it available at the device, which will accept data *into* the target device
  // on the falling edge.
  // Set initial states of the MPSSE interface
  // - high byte, both pin directions and output values
  // Pin name Signal Direction Config Initial State Config
  // ACBUS0 GPIOH0 input 0 0
  // ACBUS1 GPIOH1 input 0 0
  // ACBUS2 GPIOH2 input 0 0
  // ACBUS3 GPIOH3 input 0 0
  // ACBUS4 GPIOH4 input 0 0
  // ACBUS5 GPIOH5 input 0 0
  // ACBUS6 GPIOH6 input 0 0
  // ACBUS7 GPIOH7 input 0 0

  byOutputBuffer[dwNumBytesToSend++] = 0x82;
  // configure data bits low-byte of MPSSE port

  byOutputBuffer[dwNumBytesToSend++] = 0x00;
  // initial state config above

  byOutputBuffer[dwNumBytesToSend++] = 0x00;
  // direction config above;
  
  ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  // send off the high GPIO config commands
  
  dwNumBytesToSend = 0;
  
  for(dwCount = 0; dwCount < 8; dwCount++)
    {
      byInputBuffer[dwCount] = 0x00;
      byOutputBuffer[dwCount] = 0x00;
    }


  // Data Transmit, no receive

  byOutputBuffer[dwNumBytesToSend++] = 0x10;
  // output on rising clock, no input
  // MSB first, clock a number of bytes out

  byOutputBuffer[dwNumBytesToSend++] = 0x01; // Length L
  byOutputBuffer[dwNumBytesToSend++] = 0x00; // Length H
  // Length = 0x0001 + 1
  byOutputBuffer[dwNumBytesToSend++] = 0xA5;
  byOutputBuffer[dwNumBytesToSend++] = 0x0F;
  // Data = 0xA50F

  ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  // send off the command
  dwNumBytesToSend = 0; // reset the buffer pointer
  sleep(2); // Wait for the data to be transmitted and status to be returned by the
            // device driver - see latency timer above

  // check the receive buffer - it should be empty
  ftStatus = FT_GetQueueStatus(ftHandle, &dwNumBytesToRead);
  // get the number of bytes in the FT2232H receive buffer
  // it should be zero since there was no data clock *in*

  FT_Read(ftHandle, &byInputBuffer, dwNumBytesToRead, &dwNumBytesRead);

  if (dwNumBytesToRead != 0)
    {
      printf("Error - MPSSE receive buffer should be empty\n", ftStatus);
      FT_SetBitMode(ftHandle, 0x0, 0x00); // reset the port to disable MPSSE
      FT_Close(ftHandle); // close the USB port
      return 1; // Exit with error
    }

  printf("Press <Enter> to continue\n");
  getchar(); // wait for a carriage return

  // Now repeat the transmission with the send and receive op-code in place
  // of transmit-only Data Transmit, with receive

  byOutputBuffer[dwNumBytesToSend++] = 0x34;
  // output on rising clock, input on falling clock
  // MSB first, clock a number of bytes out
  byOutputBuffer[dwNumBytesToSend++] = 0x01; // Length L
  byOutputBuffer[dwNumBytesToSend++] = 0x00; // Length H
  // Length = 0x0001 + 1
  byOutputBuffer[dwNumBytesToSend++] = 0xA5;
  byOutputBuffer[dwNumBytesToSend++] = 0x0F;
  // Data = 0xA50F

  ftStatus = FT_Write(ftHandle, byOutputBuffer, dwNumBytesToSend, &dwNumBytesSent);
  // send off the command

  dwNumBytesToSend = 0;
  // reset output buffer pointer

  printf("sleep(2)\n");
  sleep(2);
  // wait for data to be transmitted and status to be returned by the device driver
  // see latency timer above

  // Check the receive buffer - it should contain the looped-back data
  ftStatus = FT_GetQueueStatus(ftHandle, &dwNumBytesToRead);
  // get the number of bytes in the FT2232H receive buffer
  // it should be zero since there was no data clock *in*

  FT_Read(ftHandle, &byInputBuffer, dwNumBytesToRead, &dwNumBytesRead);

  // the input buffer should contain the same number of bytes as those output

  if (dwNumBytesToRead != 2)
    {
      printf("Error - MPSSE receive buffer should have the looped-back data\n");
      FT_SetBitMode(ftHandle, 0x0, 0x00);
      // reset the port to disable MPSSE
      FT_Close(ftHandle);
      // close the USB port
      return 1;
    }
  printf("The correct number of bytes have been recieved\n");

  // check to be sure its the same

  for (dwCount = 0; dwCount <= dwNumBytesRead - 1; dwCount++)
    {
      printf("DEBUG BYTE NUMBER %d INPUT: %02x OUTPUT: %02x\n", dwCount, byInputBuffer[dwCount], byOutputBuffer[dwCount + 3]);
	  

      if (byInputBuffer[dwCount] != byOutputBuffer[dwCount + 3])
	// output data begins at location 3 after the opcode and length
	{
	  printf("Error - Data received does not match data output input %02x output %02x\n", byInputBuffer[dwCount], byOutputBuffer[dwCount + 3]);
	  //FT_SetBitMode(ftHandle, 0x0, 0x00);
	  // reset the port to disable MPSSE
	  //FT_Close(ftHandle);
	  // close the USB port
	  //return 1; // exit with error
	}
    }

  printf("The input data matches the output data\n");
  printf("Press <Enter> to continue\n");
  getchar();

  // clear the buffers
  for (dwCount = 0; dwCount < 8; dwCount++)
    {
      byInputBuffer[dwCount] = 0x00;
      byInputBuffer[dwCount] = 0x00;
    }

  printf("End");

 return 0;
}
