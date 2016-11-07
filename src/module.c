#include "../redismodule.h"
#include "../rmutil/util.h"
#include "../rmutil/strings.h"
#include "../rmutil/test_util.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>

#include "handlers/handler.h"

/* PARSE.GET <name>
*/
int PersistentGetCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

	// we must have at least 4 args
	if (argc < 1) {
		return RedisModule_WrongArity(ctx);
	}

	// init auto memory for created strings
	RedisModule_AutoMemory(ctx);
	
	RedisModuleString *key = argv[1];

	Handler* handler = GetHandler();

	RedisModuleCallReply *rep =
			RedisModule_Call(ctx, "GET", "s", key);
	RMUTIL_ASSERT_NOERROR(rep);

	// if the value was null before - we just return null
	if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_NULL) {
		RedisModule_ReplyWithNull(ctx);
		free(handler);
		return REDISMODULE_OK;
	}

	RedisModuleString *value = RedisModule_CreateStringFromCallReply(rep);

	HandlerResponse* response = handler->get(value);

	free(response);
	free(handler);

	RedisModule_ReplyWithString(ctx, response->value);
	return REDISMODULE_OK;
}

/* EXAMPLE.SET <name> <value>
*/
int PersistentSetCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

	// we must have at least 4 args
	if (argc < 2) {
		return RedisModule_WrongArity(ctx);
	}

	// init auto memory for created strings
	RedisModule_AutoMemory(ctx);
	
	RedisModuleString *key = argv[1];
	RedisModuleString *value = argv[2];

	Handler* handler = GetHandler();

	HandlerResponse* response = handler->set(key, value);

	if(response->isSuccess == 0){
		free(response);
		free(handler);
		return REDISMODULE_ERR;
	}

	RedisModuleCallReply *rep =
			RedisModule_Call(ctx, "SET", "ss", key, response->value);
	RMUTIL_ASSERT_NOERROR(rep);

	free(response);
	free(handler);
	
	// if the value was null before - we just return null
	if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_NULL) {
		RedisModule_ReplyWithNull(ctx);
		return REDISMODULE_OK;
	}

	// forward the HGET reply to the client
	RedisModule_ReplyWithCallReply(ctx, rep);
	return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx) {
	// Register the module itself
	if (RedisModule_Init(ctx, "persist", 1, REDISMODULE_APIVER_1) ==
			REDISMODULE_ERR) {
		return REDISMODULE_ERR;
	}

	RMUtil_RegisterReadCmd(ctx, "persist.get", PersistentGetCommand);

	RMUtil_RegisterReadCmd(ctx, "persist.set", PersistentSetCommand);

	return REDISMODULE_OK;
}