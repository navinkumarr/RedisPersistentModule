#include "stdlib.h"
#include "handler.h"
#include "FileHandler.h"


Handler* GetHandler(){
	Handler *handler = malloc( sizeof(Handler) );
	handler->get = GetFileHandler;
	handler->set = SetFileHandler;
	return handler;
}