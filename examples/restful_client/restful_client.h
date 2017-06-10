#ifndef RESTFUL_CLIENT_H
#define RESTFUL_CLIENT_H 

#define ACUTE_SETUP   		 	0
#define ACUTE_MAINTAINENCE   	1
#define ACUTE_REGIME_EXCHANGE   2


uint8_t fifoHandler_acute_MaintainenceMessage(uint8_t *buffer);
uint8_t fifoHandler_acute_setup(uint8_t *buffer);
uint8_t fifoHandler_acute_RegimeExchange(uint8_t *buffer);

#endif 
