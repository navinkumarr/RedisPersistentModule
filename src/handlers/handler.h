#include "../../redismodule.h"

typedef struct HandlerResponse{
	int isSuccess;
	RedisModuleString* value;
} HandlerResponse;

typedef HandlerResponse* GetTypeHandler(RedisModuleString* key);
typedef HandlerResponse* SetTypeHandler(RedisModuleString* key, RedisModuleString* value);

typedef struct Handler {
	GetTypeHandler *get;
	SetTypeHandler *set;
} Handler;

Handler* GetHandler();