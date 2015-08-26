/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#define NO_FINGER 0xFFFFu

uint16 sliderPosition;

void iluminacion();
void alarma();
void alarmaApagada();

//Variables
int contalarma;     

/*Interrupcion para apagar la alarma.
Se activa cuando se cumple la cuenta de pulsaciones.*/
CY_ISR(AlarmOff){
    uint32 excep;
    
    LED_Write(0);
    CyDelay(500);
    LED_Write(1);
    
    XbeeE_UartPutChar('X');
    excep=Counter_GetInterruptSourceMasked();
    Counter_ClearInterrupt(excep);
}


int main()
{
    CyGlobalIntEnable;
    //Variables
    char mascara;
    
    XbeeE_Start();
    LCD_Start();
    LCD_Enable();
    Counter_Start();
    CapSense_Start();
    CapSense_InitializeAllBaselines();
    
    isr_apagoAlarma_ClearPending();
    isr_apagoAlarma_StartEx(AlarmOff);
    
    LCD_WriteControl(LCD_CLEAR_DISPLAY);
    
    for(;;)
    {
        
        LCD_WriteControl(LCD_CLEAR_DISPLAY); 
        
        //RECEPCION
        mascara=XbeeE_UartGetChar();
        
        switch(mascara){
            case 'I': iluminacion();
            break;
            
            case 'A': alarma();
            break;
            
            case 'Y': alarmaApagada();

            default: 
            break;
        }
        
        CapSense_UpdateEnabledBaselines();
        CapSense_ScanEnabledWidgets();
        while(CapSense_IsBusy());
        sliderPosition=CapSense_GetCentroidPos(5);
        
        //Cuando pulse, envio los datos de la pulsacion
        if(sliderPosition!=NO_FINGER){
            switch(sliderPosition){
                case 1: XbeeE_UartPutChar(1);
                break;
                
                case 2: XbeeE_UartPutChar(2);
                break;
                
                case 3: XbeeE_UartPutChar(3);
                break;
                
                case 4: XbeeE_UartPutChar(4);
                break;
                
                case 5: XbeeE_UartPutChar(5);
                break;
                
                default:
                break;
            }
        }

    }
}

void iluminacion(){
    LED_Write(0);
    CyDelay(500);
    LED_Write(1);
    LCD_Position(0,0);
    LCD_PrintString("Iluminacion");
    LCD_Position(1,0);
    LCD_PrintString("encendida");
    CyDelay(1500);
    LCD_WriteControl(LCD_CLEAR_DISPLAY);
}

void alarma(){
    Counter_WriteCounter(0);
    Counter_Start();
    
    for(contalarma=0; contalarma<3; contalarma++){
        LCD_DisplayOff();
        CyDelay(300);
        LCD_DisplayOn();
        LCD_Position(0,3);
        LCD_PrintString("Atencion!!");
        LCD_Position(1,0);
        LCD_PrintString("Hay un intruso");
        CyDelay(700);
    }
     
        //LCD_WriteControl(LCD_CLEAR_DISPLAY);
        LCD_Position(0,0);
        LCD_PrintString("Apagar alarma:");
        LCD_Position(1,0);
        LCD_PrintString("pulse SW 5 veces");
        CyDelay(1500);
        
        //Mantengo encendido el mensaje que me
        //dice como apagar la alarma.
        while(SW_Read()==1){
        }       
}//end alarma()

void alarmaApagada(){
    LCD_Position(0,0);
    LCD_PrintString("Alarma apagada");
    CyDelay(1500);
}

//------------------------------------

/* [] END OF FILE */
