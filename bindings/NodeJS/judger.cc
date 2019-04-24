#include "node.h"
#include <cstdio>
#include <cstring>
#include <string>
#include "../../src/runner.h"

namespace demo {

    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Int32;
    using v8::Array;
    using v8::Integer;
    using v8::Uint32;
    using v8::Number;
    using v8::Object;
    using v8::String;
    using v8::Value;
    using v8::MaybeLocal;
    using v8::Exception;
    using v8::Context;

    /* 转成数字 */
    long ToNumber(Isolate * isolate,Local<Object> &args,char * key,long default_val){
        Local<Context> context= isolate->GetCurrentContext();
        Local<Value> val = args->Get(context,String::NewFromUtf8(isolate,key)).ToLocalChecked();
        if(val->IsNullOrUndefined())
            return default_val;
        else if( val->IsNumber()){
            Local<Integer> num = val->ToInteger(context).ToLocalChecked();
            return num->Value();
        }
        else {
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"typeof argument must be Object!")));
        }
    }

    char * _ToCharPTR(Local<Value> &val,Local<Context> &context,char * str_content){
            String::Utf8Value str_val( val->ToString(context).ToLocalChecked());
            strcpy(str_content,*str_val);
            return str_content;
    }

    /* 转成字符串 */
    char * ToCStr(Isolate * isolate,Local<Object> &args,char * key,char *default_val,char *str_content){

        Local<Context> context= isolate->GetCurrentContext();
        Local<Value> val= args->Get(context,String::NewFromUtf8(isolate,key)).ToLocalChecked();
        if(val->IsNullOrUndefined())
            return default_val;
        else if(val->IsString()){
            _ToCharPTR(val,context,str_content);
            return str_content;
        }
        else {
            char ret[100];
            sprintf(ret,"typeof %s must be String!",key);
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,ret)));
        }
    }


    /**
     * 参数:
     * {
     *      max_cpu_time
     *      max_real_time
     *      max_memory
     *      memory_limit_check_only ??
     *      max_stack
     *      max_process
     *      max_output_size
     *      exe_path
     *      input_path
     *      output_path
     *      error_path
     *      args:[]
     *      env:[]
     * }
     * 返回值
     * {
     *      cpu_time
     *      real_time
     *      memory
     *      signal
     *      exit_code
     *      result
     *      error:
     * }
     */

    void Method(const FunctionCallbackInfo<Value>& args)
    {
        Isolate* isolate = args.GetIsolate();
        Local<Context> context= isolate->GetCurrentContext();

        if( !args[0]->IsObject()){
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"typeof argument must be Object!")));
        }

        Local<Object> argument = args[0]->ToObject();

        /* 参数 */
        struct config _config;


        /* max_cpu_time */
        _config.max_cpu_time = ToNumber(isolate,argument,(char *)"max_cpu_time",UNLIMITED);

        /* max_real_time */
        _config.max_real_time = ToNumber(isolate,argument,(char *)"max_real_time",UNLIMITED);

        /* max_memory */
        _config.max_memory = ToNumber(isolate,argument,(char *)"max_memory",UNLIMITED);
    
        /* memory_limit_check_only */
        _config.memory_limit_check_only =
            ToNumber(isolate,argument,(char *)"memory_limit_check_only",0);

        if(_config.memory_limit_check_only)
            _config.memory_limit_check_only = 1;


        /* max_stack */
        _config.max_stack  = 
            ToNumber(isolate,argument,(char *)"max_stack",16*1024*1024); //默认16mb

        /* max_process_number */
        _config.max_process_number =
            ToNumber(isolate,argument,(char *)"max_process_number",UNLIMITED);

        /* max_output_size */
        _config.max_output_size =
            ToNumber(isolate,argument,(char *)"max_output_size",UNLIMITED);

        /* input_path */
        char input_path[255];
        _config.input_path = ToCStr(isolate,argument,(char *)"input_path",(char *)"/dev/stdin",input_path);

        /* output_path */
        char output_path[255];
        _config.output_path= ToCStr(isolate,argument,(char *)"output_path",(char *)"/dev/stdout",output_path);

        /* error_path */
        char error_path[255];
        _config.error_path= ToCStr(isolate,argument,(char *)"error_path",(char *)"/dev/stderr",error_path);

        /* exe_path */
        char exe_path[255];
        _config.exe_path = ToCStr(isolate,argument,(char *)"exe_path",(char *)"/bin/ls",exe_path);

        /* args */
        _config.args[0] = _config.exe_path;

        char _args[100][255];
        Local<Value> margs= argument->Get(context,String::NewFromUtf8(isolate,"args")).ToLocalChecked();
        if( margs->IsNullOrUndefined()){
            _config.args[1] = NULL;
        }
        else if( margs->IsArray()){
            Local<Array> args = margs.As<Array>();
            int len = args->Length();
            int i;
            for(i=0;i<len;i++){
                Local<Value> in = args->Get(i);
                if(in->IsString()){
                    _ToCharPTR(in,context,_args[i]);
                    _config.args[i+1] = _args[i];
                }
                else
                    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"the content of args must be string!")));
            }
            _config.args[len+1] = NULL;
        }
        else {  //not array
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"typeof argument.args must be Array!")));
        }

        char _env[100][255];
        Local<Value> menv= argument->Get(context,String::NewFromUtf8(isolate,"env")).ToLocalChecked();
        if( menv->IsNullOrUndefined()){
            _config.env[0] = NULL;
        }
        else if( margs->IsArray()){
            Local<Array> env = menv.As<Array>();
            int len = env->Length();
            int i;
            for(i=0;i<len;i++){
                Local<Value> in = env->Get(i);
                if(in->IsString()){
                    _ToCharPTR(in,context,_env[i]);
                    _config.env[i] = _env[i];
                }
                else
                    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"the content of env must be string!")));
            }
            _config.env[len] = NULL;
        }
        else {  //not array
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"typeof argument.env must be Array!")));
        }

        /* log_path */
        char log_path[255];
        _config.log_path= ToCStr(isolate,argument,(char *)"log_path",(char *)"judger.log",log_path);

        /* seccomp_rule_name */
        char seccomp_rule_name[255];
        _config.seccomp_rule_name = ToCStr(isolate,argument,(char *)"seccomp_rule_name",NULL,seccomp_rule_name);

        /* uid */
        _config.uid =
            ToNumber(isolate,argument,(char *)"uid",65534);
        /* gid */
        _config.gid =
            ToNumber(isolate,argument,(char *)"gid",65534);



/*
 *        printf("max_cpu_time %d\n",_config.max_cpu_time);
 *        printf("max_real_time %d\n",_config.max_real_time);
 *        printf("max_memory %ld\n",_config.max_memory);
 *        printf("max_stack %ld\n",_config.max_stack);
 *        printf("max_process_number %d\n",_config.max_process_number);
 *        printf("max_output_size %ld\n",_config.max_output_size);
 *        printf("%s\n",_config.input_path);
 *        printf("%s\n",_config.output_path);
 *        printf("%s\n",_config.error_path);
 *
 *        int idx = 0;
 *        while( _config.args[idx]!= NULL){
 *            printf("args[%d] = %s\n",idx,_config.args[idx]);
 *            idx++;
 *        }
 *
 *        idx = 0;
 *        while( _config.env[idx]!= NULL){
 *            printf("env[%d] = %s\n",idx,_config.env[idx]);
 *            idx++;
 *        }
 *
 *        printf("log_path %s\n",_config.log_path);
 *        printf("seccomp_rule_name %s\n",_config.seccomp_rule_name);
 *        printf("uid %d\n",_config.uid);
 *        printf("gid %d\n",_config.gid);
 */

        struct result _result = {0, 0, 0, 0, 0, 0, 0};
        run(&_config, &_result);

        /*
         *printf("{\n"
         *   "    \"cpu_time\": %d,\n"
         *   "    \"real_time\": %d,\n"
         *   "    \"memory\": %ld,\n"
         *   "    \"signal\": %d,\n"
         *   "    \"exit_code\": %d,\n"
         *   "    \"error\": %d,\n"
         *   "    \"result\": %d\n"
         *   "}",
         *   _result.cpu_time,
         *   _result.real_time,
         *   _result.memory,
         *   _result.signal,
         *   _result.exit_code,
         *   _result.error,
         *   _result.result);
         */

        /* 返回值 */
        Local<Object> result = Object::New(isolate);
        result->Set(String::NewFromUtf8(isolate,"cpu_time"),Integer::New(isolate,_result.cpu_time));
        result->Set(String::NewFromUtf8(isolate,"real_time"),Integer::New(isolate,_result.real_time));
        result->Set(String::NewFromUtf8(isolate,"memory"),Integer::New(isolate,_result.memory));
        result->Set(String::NewFromUtf8(isolate,"signal"),Integer::New(isolate,_result.signal));
        result->Set(String::NewFromUtf8(isolate,"exit_code"),Integer::New(isolate,_result.exit_code));
        result->Set(String::NewFromUtf8(isolate,"result"),Integer::New(isolate,_result.result));
        result->Set(String::NewFromUtf8(isolate,"error"),Integer::New(isolate,_result.error));

        args.GetReturnValue().Set(result);
    }

    void init(Local<Object> exports)
    {
        NODE_SET_METHOD(exports, "run", Method);
    }

    NODE_MODULE(addon, init)

}
