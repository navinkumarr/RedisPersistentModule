#include "handler.h"

#include "../../redismodule.h"
#include "../../rmutil/util.h"
#include "../../rmutil/strings.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

char *join(int l, ...){

	va_list valist;
	size_t len = 1;
	size_t i ;

	va_start(valist, l);

	for(i=0; i < l; i++){
		len += strlen(va_arg(valist, char*));
	}

	va_end(valist);

	va_start(valist, l);

	char *temp = (char *) malloc(len);

	strcpy(temp, va_arg(valist, char*));
	
	for(i=0; i < l - 1; i++){
		strcat(temp, va_arg(valist, char*));
	}

	va_end(valist);

	return temp;
}

char *getDatastorePath(){
	
	char *dataDirectory = "/.datastore/";

	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL){
		return "";
	}

	return join(2, cwd, dataDirectory);
}

int makeDataStoreDirectory(){
	char *path = getDatastorePath();

	if(strlen(path) == 0){
		return -1;
	}

	struct stat st = {0};

	if (stat(path, &st) == -1) {
	    mkdir(path, 0700);
	}

	free(path);

	return 1;
}

HandlerResponse* createHandlerResponse(int isSuccess, RedisModuleString* value){
	HandlerResponse* response;
	response = malloc( sizeof(HandlerResponse) );
	response->isSuccess = isSuccess;
	response->value = value;
	return response;
}


HandlerResponse* GetFileHandler(RedisModuleCtx *ctx, RedisModuleString* key){

	size_t klen = 0;
	char buff[255];
	
	const char *keyString = RedisModule_StringPtrLen(key, &klen);

	char *basePath = getDatastorePath();

	char *finalPath = join(2, basePath, keyString);

	FILE *fptr;

	fptr = fopen(finalPath, "r");

	if(fptr == NULL){
	    return createHandlerResponse(0, key);
	}

	if(EOF == fscanf(fptr, "%s", buff)){
		return createHandlerResponse(0, key);	
	}

	RedisModuleString* response = RedisModule_CreateString(ctx, buff, strlen(buff));

	fclose(fptr);
	free(basePath);
	free(finalPath);

	return createHandlerResponse(1, response);
}

HandlerResponse* SetFileHandler(RedisModuleCtx *ctx, RedisModuleString* key, RedisModuleString* value){
	
	if(makeDataStoreDirectory() == -1){
		return createHandlerResponse(0, key);
	}

	size_t vlen = 0;
	size_t klen = 0;
	
	const char *keyString = RedisModule_StringPtrLen(key, &klen);
	const char *valueString = RedisModule_StringPtrLen(value, &vlen);

	char *basePath = getDatastorePath();

	char *finalPath = join(2, basePath, keyString);

	FILE *fptr;

	fptr = fopen(finalPath, "wb");

	if(fptr == NULL)
	{
	    return createHandlerResponse(0, key);
	}

	fprintf(fptr, "%s", valueString);

	fclose(fptr);
	free(basePath);
	free(finalPath);

	return createHandlerResponse(1, key);
}

