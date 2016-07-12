#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h>
#include "webSocket.h"
static int sendWebSocketData(struct libwebsocket *wsi, unsigned char *buf, size_t len);
static struct libwebsocket_context *context = NULL;
static struct libwebsocket *gWsi = NULL;
struct lws_context_creation_info info;
struct libwebsocket_protocols protocols[3];

static int httpCallback(struct libwebsocket_context *context,
  struct libwebsocket *wsi,
  enum libwebsocket_callback_reasons reason,
  void *user,
  void *in,
  size_t len) {
	printf("httpCallback! reason=%x\n", reason);
	int ret = 0;
	switch(reason) {
		case LWS_CALLBACK_ESTABLISHED:
			printf("LWS_CALLBACK_ESTABLISHED\n");
			gWsi = wsi;
			//sendWebSocketData(wsi, "Hello!", 6);
			break;		
		case LWS_CALLBACK_PROTOCOL_INIT:
			printf("LWS_CALLBACK_PROTOCOL_INIT\n");
			break;
		case LWS_CALLBACK_HTTP:
			printf("LWS_CALLBACK_HTTP - path is \"%s\"\n", (char *)in);
			libwebsockets_serve_http_file(context, wsi, (char *)in, "text/plain");
			ret = 1;
			break;
		case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
			printf("LWS_CALLBACK_FILTER_NETWORK_CONNECTION\n");
			break;			
		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			printf("LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION\n");
			break;	
		case LWS_CALLBACK_ADD_POLL_FD:
			printf("LWS_CALLBACK_ADD_POLL_FD\n");
			break;					
		case LWS_CALLBACK_DEL_POLL_FD:
			printf("LWS_CALLBACK_DEL_POLL_FD\n");
			break;				
	}
	return ret;
} 

static int dumbIncrementCallback(struct libwebsocket_context *context,
	struct libwebsocket *wsi,
   enum libwebsocket_callback_reasons reason,
   void *user,
   void *in,
   size_t len) {
	printf("Dumbincrementcallback! reason=%x\n", reason);
	switch(reason) {
		case LWS_CALLBACK_PROTOCOL_INIT:
			printf("LWS_CALLBACK_PROTOCOL_INIT\n");
			break;
	}	
	return 0;
}

static int sendWebSocketData(struct libwebsocket *wsi, unsigned char *buf, size_t len) {
	unsigned char *tmpBuf = malloc(LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING);
	memcpy(tmpBuf+LWS_SEND_BUFFER_PRE_PADDING, buf, len);
	int rc = libwebsocket_write(wsi, tmpBuf+LWS_SEND_BUFFER_PRE_PADDING, len, LWS_WRITE_TEXT);
	free(tmpBuf);
	return rc;
}
void websocket_init() {


	memset(protocols, 0, sizeof(protocols));
	protocols[0].name = "http-only";
	protocols[0].callback = httpCallback;
	protocols[1].name = "dumb-increment-protocol";
	protocols[1].callback = dumbIncrementCallback;
	

	memset(&info, 0, sizeof(info));
	info.port = 3000;
	info.gid = -1;
	info.uid = -1;
	info.protocols = protocols;
	

	context = libwebsocket_create_context(&info);
	if (context == NULL) {
		printf("Failed to create context!");
	}
}
void webSocket_send(char *data, int len) {
  if (gWsi != NULL) {
    sendWebSocketData(gWsi,data, len);
  }
}

void webSocket_service() {
  libwebsocket_service(context, 0);
}
