void webSocket_init(unsigned short portNumber, void (*newConnectionCB)());
void webSocket_service();
void webSocket_send(char *data, int len);
