
/*
Basics of the SMS messaging system
*/
#include "uCommand.h"


STARTUP(cellular_credentials_set("mdata.net.au", "", "", NULL));

uCommand uCmd;

void setup()
{
char res;
int atResult;
	//Cellular.off();
	delay(2000);
	Serial.println("Entering sms setup");
	pinMode(B0,OUTPUT);
	digitalWrite(B0, HIGH);	// turn power on
	uCmd.setDebug(false);
	// set up text mode for the sms
	atResult = uCmd.setSMSMode(1);
	if(atResult == RESP_OK){
		Serial.println("Text mode setup was OK");
	}
	else{
		Serial.println("Did not set up text mode");
	}
	delay(1000);

}


void loop()
{
int val,ret,res;
char c,i,str[80];

	while(1){
		Serial.println("Looking for an sms message");
		// read next available messages
		if(uCmd.checkMessages(10000) == RESP_OK){
			uCmd.smsPtr = uCmd.smsResults;
			for(i=0;i<uCmd.numMessages;i++){
				Serial.printlnf("message received %s",uCmd.smsPtr->sms);
				uCmd.smsPtr++;
			}
		}
		// return the last message back to the sender
		uCmd.smsPtr = uCmd.smsResults;
		if(uCmd.numMessages > 0){
			for(i=0;i<uCmd.numMessages-1;i++)uCmd.smsPtr++;
			sprintf(str, "Last message received: %s",uCmd.smsPtr->sms);
			if(uCmd.sendMessage(str,uCmd.smsPtr->phone,10000) == RESP_OK)
				Serial.println("Message sent successfully");
			else
				Serial.println("Message failed to send");
		}
		// delete all the messages
		uCmd.smsPtr = uCmd.smsResults;
		for(i=0;i<uCmd.numMessages;i++){
			if(uCmd.deleteMessage(uCmd.smsPtr->mess,10000) == RESP_OK){
				Serial.println("message deleted successfully");
			}
			else Serial.println("could not delete message");
			uCmd.smsPtr++;
		}
		delay(20000);
	}

}
