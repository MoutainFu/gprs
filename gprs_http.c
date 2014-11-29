#include <stdio.h>
#include <string.h>
 #include <stdlib.h>
#include "em_leuart.h"
#include "em_gpio.h"
#include "gprs.h"             
#include "leuart.h"
#include "systick.h"
#include "pulse.h"
#include "adc.h"

#define TCPIP_SERVER_IPADDR "115.29.140.120"
#define TCPIP_SERVER_PORT   "8082"


unsigned  char tcpip_config_ok=0;
unsigned  char http_conect_ok=0;
unsigned char  tcpip_config_step = 0;
unsigned char http_config_state=0; 

static  char tcpip_buf[500]={0};

char pGprs[] = "device-id=1&device-type=ring&data=";

void tcpip_config(void)
{  
       
    switch (tcpip_config_step)
    {
        case 0:
            free_rom_buf(rx_buf,sizeof(rx_buf));
            leuart_sent_string("AT\r\n");
            leuart_tick = msTicks;            
            tcpip_config_step = 1; 
            break;
        case 1:
            if((msTicks - leuart_tick) > 3000)
            {        
                if(strstr(rx_buf, "OK") == NULL)                   
                {
                    tcpip_config_step = 0;
                }
                else
                {
                    tcpip_config_step = 2; 
                                   
                } 
                free_rom_buf(rx_buf,sizeof(rx_buf));                         
            }                
            break;
                
//===================================================            
        case 2:
            leuart_sent_string("AT+CCID\r\n");
            tcpip_config_step = 3;
            leuart_tick = msTicks;
            break;        
        case 3:
            if((msTicks - leuart_tick) > 50)
            {
                if(strstr(rx_buf, "OK") == NULL)
                {
                    tcpip_config_step = 2;
                }
                else
               {
                tcpip_config_step = 4;
                                                                      
               }
               free_rom_buf(rx_buf,strlen(rx_buf));
            }            
            break;          
//========================================================            
        case 4:
            leuart_sent_string("AT+CSQ\r\n");
            tcpip_config_step = 5;
            leuart_tick = msTicks;
            break;
        case 5:
            if((msTicks - leuart_tick) > 50)
            {
                if(strstr(rx_buf, "OK") == NULL)
                {
                    tcpip_config_step = 4;
                }
                else
                {
                tcpip_config_step = 6;            
                
                }
                free_rom_buf(rx_buf,strlen(rx_buf));
            }            
            break;
             
//=====================================================            
        case 6:
            leuart_sent_string("AT+CREG?\r\n");
            tcpip_config_step = 7;
            leuart_tick = msTicks;
            break;
        case 7:
            if((msTicks - leuart_tick) > 50)
            {
                if(strstr(rx_buf, "OK") == NULL)
                {
                    tcpip_config_step = 6;
                }
                else
                {
                tcpip_config_step = 8;
                    
                }
                free_rom_buf(rx_buf,strlen(rx_buf));
            }            
            break;
            
//=================================================================
        case 8:
            leuart_sent_string("AT+CGDCONT=1,\"IP\",\"cmnet\"\r\n");
            tcpip_config_step = 9;
            leuart_tick = msTicks;
            break;
        case 9:
            if((msTicks - leuart_tick) > 50)
            {
                if(strstr(rx_buf, "OK") == NULL)
                {
                    tcpip_config_step = 8;
                }
                else
                {
                tcpip_config_step = 10;
                                     
                }
                free_rom_buf(rx_buf,strlen(rx_buf));
            }            
            break;

 //=================================================================
 // 前面10步校准SIM卡，并且写入网络参数
 //=================================================================           
        case 10:
            leuart_sent_string("AT+CGATT?\r\n");
            tcpip_config_step = 11;
            leuart_tick = msTicks;
            break;
        case 11:
            if((msTicks - leuart_tick) > 1000)
            {
                if(strstr(rx_buf, "+CGATT: 0") == NULL)
                {
                    tcpip_config_step = 14;
                }
                else
                {
                 tcpip_config_step = 12;                
                 //tcpip_config_ok = 1;                  
                }
                free_rom_buf(rx_buf,strlen(rx_buf));
            }            
            break; 


//==================================================================
        case 12:
            //sprintf(tcpip_buf, "AT+TCPSETUP=0,%s,%s\x0D\x0A", TCPIP_SERVER_IPADDR, TCPIP_SERVER_PORT);
            leuart_sent_string("AT+CGATT=1\r\n");
            //leuart_sent_string("AT+TCPSETUP=0,115.29.140.120,8082\r\n");
            tcpip_config_step = 13;
            leuart_tick = msTicks;
            break;
        case 13:
            if((msTicks - leuart_tick) > 500)
            {
                if(strstr(rx_buf, "OK") == NULL)
                {
                    tcpip_config_step = 8;//附着失败返回网络参数设置
                }
                else
                {
                    tcpip_config_step = 14;                                                          
                }
                free_rom_buf(rx_buf,strlen(rx_buf));
            }            
            break;             
//=================================================================
        case 14:
            leuart_sent_string("AT+XIIC=1\r\n");
            tcpip_config_step = 15;
            leuart_tick = msTicks;
            break;
        case 15:
            if((msTicks - leuart_tick) > 50)
            {
                if(strstr(rx_buf, "OK") == NULL)
                {
                    tcpip_config_step = 14;
                }
                else
                {
                tcpip_config_step = 16;
                                  
                }
                free_rom_buf(rx_buf,strlen(rx_buf));
            }            
            break;

//=================================================================
        case 16:
            leuart_sent_string("AT+XIIC?\r\n");
            tcpip_config_step = 17;
            leuart_tick = msTicks;
            break;
        case 17:
            if((msTicks - leuart_tick) > 1000)
            {
                if(strstr(rx_buf, "0,0.0.0.0") == NULL)
                {
                    tcpip_config_step = 0;
                    tcpip_config_ok = 1; 
                    
                }
                else
                {
                    tcpip_config_step = 14;
          
                }
                free_rom_buf(rx_buf,strlen(rx_buf));
            }            
            break;
                   
        default:
            break;
    }    
       
}

void http_config(void)
{
 
    switch (http_config_state)
    {

        case 0:
             leuart_sent_string("AT+HTTPPARA=url,115.29.140.120/app/pulse-wave/commit\r\n");
             //leuart_sent_string("AT+HTTPPARA=url,115.29.140.120:8082/app/user/login?username=mzx&password=mzx\r\n");
       
             http_config_state = 1;
            leuart_tick = msTicks;
            break;                

        case 1:
                if((msTicks - leuart_tick) > 50)
                {
                    if(strstr(rx_buf, "OK") == NULL)
                    {
                        http_config_state = 0;
                    }
                    else{
                    http_config_state = 2;
                      
                    }
                    free_rom_buf(rx_buf,strlen(rx_buf));
                }           
            break; 

        case 2:
             sprintf(tcpip_buf, "AT+HTTPPARA=port,%s\x0D\x0A",TCPIP_SERVER_PORT);
             leuart_sent_string(tcpip_buf); 
             http_config_state = 3; 
             leuart_tick = msTicks;
            break;                

        case 3:
                if((msTicks - leuart_tick) > 50)
                {
                    if(strstr(rx_buf, "OK") == NULL)
                    {
                        http_config_state = 2;
                    }
                    else{
                    http_config_state = 0; 
                    http_conect_ok = 1;    
                                
                    }
                    free_rom_buf(rx_buf,strlen(rx_buf));
                }           
            break; 


                
        default:
            break;
    }     
}

void http_Send_test(void)
{
    static unsigned char http_send_step = 0;
    unsigned int k;
    
    switch (http_send_step)
    {
        
    case 0:
         leuart_sent_string("AT+XIIC?\r\n");  
         http_send_step = 1;
         leuart_tick = msTicks;
        break;            
    
    case 1:
            if((msTicks - leuart_tick) > 1000)
            {
                if(strstr(rx_buf, "0,0.0.0.0") == NULL)
                {                                    
                    http_send_step = 2;                                         
                }
                else
                {
                   http_send_step = 0; 
                   tcpip_config_ok = 0;         
                }
                free_rom_buf(rx_buf,strlen(rx_buf));
            } 
        break;
            
        case 2:
             leuart_sent_string("AT+HTTPSETUP\r\n"); 
             http_send_step = 3;
             leuart_tick = msTicks;
            break;
        
        case 3:
                if((msTicks - leuart_tick) > 1000)
                {
                    if(strstr(rx_buf, "OK") == NULL)
                    {                        
                       http_send_step = 0;
                       http_conect_ok = 0;        
                    }
                   else
                   {                        
                        http_send_step = 4; 
                    }
                    free_rom_buf(rx_buf,strlen(rx_buf));
                }            
            break;            
    
    case 4:
         leuart_sent_string("AT+HTTPACTION=2,2038\r\n"); //34+ x +1
         http_send_step = 5;
        leuart_tick = msTicks;
        break;
    
    case 5:
            if((msTicks - leuart_tick) > 1000)
            {
                if(strstr(rx_buf, ">") == NULL)
                {                                                  
                    http_send_step = 2; 
                    //http_conect_ok = 0;
                }
                else{
                                       
                http_send_step = 6;
                }
                free_rom_buf(rx_buf,strlen(rx_buf));
            }                
        break;
    case 6:               
       leuart_sent_string(pGprs);
        
        
        for(k= 0;k<500;k++)
        {              
            sprintf(tcpip_buf, "%04d", adc_buf[k]);

            leuart_sent_string(tcpip_buf);
            
        }
        
        sprintf(tcpip_buf, "%04d", disp_filt_pulse);
        leuart_sent_string(tcpip_buf);
        
        http_send_step = 7;
        leuart_tick = msTicks;
        
        break;
    
    case 7:
            //发数据没有检测成功或失败，因为不管成功还是失败都要继续发送
            if((msTicks - leuart_tick) > 1000)
            {
                http_send_step = 0;
                free_rom_buf(rx_buf,strlen(rx_buf));
            } 
   
        break; 
   
    default:
        break;
               
    }
            
}
    
    
void gprs_monit_thread(void)
{
   if((gprs_is_ok)&&(!tcpip_config_ok))                      { tcpip_config();                   }
   if((tcpip_config_ok) && (!http_conect_ok) )               { http_config();                    }
   if((tcpip_config_ok) && (http_conect_ok)&&(data_is_ok))   { http_Send_test(); data_is_ok = 0; }    
}
