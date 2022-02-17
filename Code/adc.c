#include "NXP/iolpc2148.h"

/*-------------------------------------------------------------------------------
Function:   Initializes the ADC module

Returns:    None

Arguments: None

Notes:      Only channel 0 (AD0.0) of ADC0 is set to analog input. you can configure as much as 6 analog inputs
            on each of the ADC modules (ADC0 and ADC1 for a max of 12 analog channels)
            (refer to the manual (page 287) for more detailed description)
---------------------------------------------------------------------------------*/
void adc_init()
{
   PINSEL1_bit.P0_27 = 1; //set bit 28 PIO0 as analog input (channel-1)
         
   AD0CR_bit.SEL = 0x02;  //Conversion to be done on channel-1 only
   
   AD0CR_bit.CLKDIV = 11; //ADC_CLK = PCLK/12 = 48M/12 = 4M (Note: ADC_CLK must be <= 4.5MHz)
   
   AD0CR_bit.START = 0;   //don't start conversion yet
    
   AD0CR_bit.PDN = 1;     //AD converter is enabled and is ready 
   
}

/*-----------------------------------------------------------------------------------------------
Function:   Samples and measures the amplitude of an analog signal on channel-0 of the ADC0
          
Returns:    Digital equivalent of the input analog signal

Arguments:  One of the 6 analog channels from where analog input signal is to be sampled

Notes:      Analog voltage at the ADC input must be in the range 0 - 3.3V. ADC resolution
            is 10-bit [i.e digital value is from 0-1023]. 

            Smallest measurable voltage (resolution) = 1/1023 = 0.98mV (980uV). The reference
            voltage can be lowered for smaller resolution.

            digital value = analog value * (vref/1023), where vref = 3.3V -- see schematic
------------------------------------------------------------------------------------------------*/
unsigned short adc_read(char channel)
{
   unsigned short digital_value;
   
   AD0CR_bit.SEL = (1<<channel);    //Conversion to be done on selected channel
   
   AD0GDR_bit.DONE = 0;             //to be sure previous result was read (optional)
      
   AD0CR_bit.START = 1;                //start conversion now
      
   while(!AD0GDR_bit.DONE);            //wait for conversion to complete (DONE bit becomes 1 when conversion is done)
   
   AD0CR_bit.START = 0;                //clear the start bit
   
   digital_value = AD0GDR_bit.RESULT;  //read the 10-bit digital conversion result
   
   return digital_value;               //return digital value to the calling program
}
   
   