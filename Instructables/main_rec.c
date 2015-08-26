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

int i;

void activoAlarma();

//Activar iluminacion
CY_ISR(Ilum_interrupt){
    Xbee_UartPutChar('I');
    PWM_ilum_WriteCompare(255);
}

//Activar aviso de alarma
CY_ISR(Aviso_interrupt){
    //Si solo se ha forzado por menos de 3,5 segundos una posible puerta
    //solo doy un aviso de presencia de alarma;
    uint32 excep;
    
    activoAlarma();
    CyDelay(300);
    activoAlarma();
    excep=Timer_GetInterruptSourceMasked();
    Timer_ClearInterrupt(excep);
}

//Activar alarma, y dejar activada hasta que el controlador la apague
CY_ISR(Alarm_interrupt){
    uint32 excep;
    char estado_alarma='0';
    //Si se intenta forzar la alarma por mucho tiempo,
    //se queda encendida hasta que el usuario la apague
    //enviando el caracter X;
    if(Acceso_Read()==0){
        
        Xbee_UartPutChar('A');
        
        while(estado_alarma!='X'){
                              
            activoAlarma(); 
            estado_alarma=Xbee_UartGetChar();
        }
        
        Xbee_UartPutChar('Y');
        excep=Timer_GetInterruptSourceMasked();
        Timer_ClearInterrupt(excep);
     }
}

//Funcion que activa alarma
void activoAlarma(){
    for(i=0; i<35000; i++){
        Alarma_WriteCompare(i*2);
    }
    if(i==35000){
        for(i=35000; i>0; i--){
            Alarma_WriteCompare(i*2);
        }
    }
}

int main()
{
    char nivel;
    
    //Interrupcion Sensor de iluminacion
    isr_ilum_ClearPending();
    isr_ilum_StartEx(Ilum_interrupt);
    
    //Interrupcion Alarma
    isr_acceso_ClearPending();
    isr_acceso_StartEx(Alarm_interrupt);
    
    //Interrupcion Aviso
    isr_aviso_ClearPending();
    isr_aviso_StartEx(Aviso_interrupt);
    
    
    Xbee_Start();
    Alarma_Start();
    Timer_Start();
    PWM_ilum_Start();
 
    Xbee_SpiUartClearRxBuffer();
    
    CyGlobalIntEnable; 
    
    for(;;)
    {
        //EMISION
        //Sensor de Iluminacion --> usar una interrupcion
        
        
        if(Ilu_sensor_Read()==0){
            PWM_ilum_WriteCompare(0);            
        }else{
            nivel=Xbee_UartGetChar();
            switch(nivel){
                case 1: PWM_ilum_WriteCompare(40);
                break;
                
                case 2: PWM_ilum_WriteCompare(70);
                break;
                
                case 3: PWM_ilum_WriteCompare(130);
                break;
                
                case 4: PWM_ilum_WriteCompare(190);
                break;
                
                case 5: PWM_ilum_WriteCompare(255);
                break;
                
                default:
                break;
                
            } //end switch
            
        }//end else
              
    }//end for
    
}//end main

/* [] END OF FILE */
