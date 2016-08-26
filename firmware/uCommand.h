/* _____CLASS DEFINITIONS____________________________________________________ */
/**

*/
class uCommand
{
public:
	bool setDebug(bool);
	int setSMSMode(char);
	int sendMessage(char*,char*,int);
	int checkMessages(int);	
	int deleteMessage(char,int);
	bool debugMode = false;
	unsigned char sendIndex = 0;
	unsigned char sendLength = 0;
	unsigned char rxIndex = 0;
	unsigned char rxLength = 0;
	char sendBuffer[100];
	char rxBuffer[100];
	struct smsStruct{
		char mess;
		char status[10];
		char phone[20];
		char sms[50];
	};
	struct smsStruct* smsPtr;
	struct smsStruct smsResults[10];
	char numMessages;
private:
};
