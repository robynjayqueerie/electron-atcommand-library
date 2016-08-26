#include "application.h"
#include "uCommand.h"


struct _messStruct{
	char mess;
	char status[10];
	char phone[20];
	char sms[50];
};
static struct _messStruct* _messPtr;
static struct _messStruct _messResults[10];
static char _messN;
static char _u_debug;

/*
	The callback function for the Cellular.command() - unfortunately Particle don't seem
	to be able to allow this to be a public function within uCommand. This prevents me
	accessing any of the uCommand functions or variables directly hence the use of
	some local variables which survive between calls to this function
*/
int callback(int type, const char* buf, int len, char* result)
{
char* tp;
char i;
char* cp;
char* ep;

	_messPtr = _messResults;
	for(i=0;i<_messN;i++)_messPtr++;	// need to point at the right location
	tp = (char*)buf + len;
	*tp = 0;	// the buffer is not null terminated so we don't want leftover crap
	if(_u_debug)Serial.printlnf("Response in callback %x: %i : %s: \r\n",type,len,buf);
	switch(type) {
		case TYPE_UNKNOWN :
			// this could be the next part of a +CMGL response with the sms text in buf
			strcpy(_messPtr->sms,buf);
			_messN++;
			break;
		case TYPE_OK :
			break;
		case TYPE_ERROR :
			break;
		case TYPE_RING :
			break;
		case TYPE_CONNECT :
			break;
		case TYPE_NOCARRIER :
			break;
		case TYPE_NODIALTONE :
			break;
		case TYPE_NOANSWER :
			break;
		case TYPE_PROMPT :
			break;
		case TYPE_PLUS :
			if(cp = strstr(buf,"CMGL:")){
				while(!isdigit(*cp))cp++;
				_messPtr->mess = atoi(cp);
				while(*cp != '"')cp++;
				cp++;
				ep = cp;
				while(*ep != '"')ep++;
				strncpy(_messPtr->status,cp,(char)(ep-cp));	// read, etc status
				cp = ep+1;
				while(*cp != '"')cp++;
				cp++;
				ep = cp;
				while(*ep != '"')ep++;
				strncpy(_messPtr->phone,cp,(char)(ep-cp));	// phone number
			}
			if(strstr(buf,"CMGR:")){
			}
			break;
		case TYPE_TEXT :
			break;
		case TYPE_ABORTED :
			break;
	}
	return WAIT;
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */


/**
// sets a print out debug mode
*/

bool uCommand::setDebug(bool _debug)
{
	debugMode = _debug;
	_u_debug = _debug;
	return debugMode;
}


int uCommand::setSMSMode(char mode){
	if(mode)return Cellular.command(callback, rxBuffer, 10000, "AT+CMGF=1\r\n");
	else return Cellular.command(callback, rxBuffer, 10000, "AT+CMGF=0\r\n");
}


int uCommand::sendMessage(char* pMessage,char* phoneNumber,int timeout){
char CTRL_Z = 0x1a;
    sprintf(sendBuffer, "AT+CMGS=\"%s\"\r\n", phoneNumber);
	if(debugMode){
		Serial.printlnf("Sending command %s",sendBuffer);
	}

    Cellular.command(callback, rxBuffer, timeout, sendBuffer);
    Cellular.command(callback, rxBuffer, timeout, pMessage);

    sprintf(sendBuffer, "%c", CTRL_Z);

    int retVal = Cellular.command(callback, rxBuffer, timeout, sendBuffer);

    if(RESP_OK == retVal){
        if(debugMode)Serial.println("+OK, Message Send");
    }
    else{
        if(debugMode)Serial.println("+ERROR, error sending message");
    }

    return retVal;
}


int uCommand::checkMessages(int timeout){
int retVal;
char i;
	// check for any sms messages on device, results are returned in smsResults
	_messN = 0;	// need some local counters here to retrieve all the information
	sprintf(sendBuffer, "AT+CMGL\r\n");
	if(debugMode)Serial.printlnf("Request sent to modem is: %s",sendBuffer);
	retVal = Cellular.command(callback, rxBuffer, timeout, sendBuffer);
	_messPtr = _messResults;
	if(debugMode){
		Serial.println("Results of the list message command are");
		for(i=0;i<_messN;i++){
			Serial.printlnf("message number %d",_messPtr->mess);
			Serial.printlnf("message status %s",_messPtr->status);
			Serial.printlnf("message phone %s",_messPtr->phone);
			Serial.printlnf("message sms %s",_messPtr->sms);
			Serial.println("-------------------------------------");
			_messPtr++;
		}
	}
	// now put the results into a public structure
	_messPtr = _messResults;
	smsPtr = smsResults;
	numMessages = _messN;
	for(i=0;i<_messN;i++){
		smsPtr->mess = _messPtr->mess;
		strcpy(smsPtr->status,_messPtr->status);
		strcpy(smsPtr->phone,_messPtr->phone);
		strcpy(smsPtr->sms,_messPtr->sms);
		smsPtr++;
		_messPtr++;
	}
	return retVal;
}
/*
	Function deletes a message from the device
*/
int uCommand::deleteMessage(char mess,int timeout)
{
int retVal;
	_messN = 0;	// need some local counters here to retrieve all the information
	sprintf(sendBuffer, "AT+CMGD=%d\r\n",mess);
	if(debugMode)Serial.printlnf("Request sent to modem is: %s",sendBuffer);
	retVal = Cellular.command(callback, rxBuffer, timeout, sendBuffer);
	return retVal;
}

