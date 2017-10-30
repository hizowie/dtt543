

#define TOTAL_PACKETS 20000
#define MAX_UDP_PAYLOAD 1472



const char* error_ACKTimeout;
const char* msg_ACKSend;
const char* msg_ACKResend;
const char* msg_packetTime;
const char* instructions;
const char * error_invalidInput;
const char * error_notEnoughArguments;
const char * error_portNumber;
const char * error_machineName;
const char * error_testID;
const char * error_windowSize;
const char * error_timeoutLength;
const char * error_requiredOptions_Server;
const char * error_requiredOptions_Stop;
const char * error_requiredOptions_Sliding;




int getTestID(const char* input);
int getTimeoutLength(const char* input);
int getWindowSize(const char* input);
