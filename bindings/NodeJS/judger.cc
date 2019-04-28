#include "node.h"
#include <cstdio> 
#include <cstring> 
#include <string> 
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>


namespace demo {

    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::JSON;
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
    bool ToNumber(Isolate * isolate,Local<Value> &args,char * key,std::string &str){
        Local<Context> context= isolate->GetCurrentContext();
        Local<Value> val = args->ToObject()->Get(context,String::NewFromUtf8(isolate,key)).ToLocalChecked();
        if(val->IsNullOrUndefined())
            return true;
        else if( val->IsNumber()){
            Local<Integer> num = val->ToInteger(context).ToLocalChecked();
            str+=" --";
            str+=std::string(key);
            str+="=";
            str+=std::to_string(num->Value());
            return true;
        }
        else {
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"typeof argument must be Object!")));
            return false;
        }
    }

    char * _ToCharPTR(Local<Value> &val,Local<Context> &context,char * str_content){
        String::Utf8Value str_val( val->ToString(context).ToLocalChecked());
        strcpy(str_content,*str_val);
        return str_content;
    }

    /* 转成字符串 */
    bool  ToCStr(Isolate * isolate,Local<Value> &args,char * key,std::string &str){

        char str_content[255]={0};
        Local<Context> context= isolate->GetCurrentContext();
        Local<Value> val;
        if(args->IsObject())
            val = args->ToObject()->Get(context,String::NewFromUtf8(isolate,key)).ToLocalChecked();
        else
            val = args;
        if(val->IsNullOrUndefined())
            return true;
        else if(val->IsString()){
            _ToCharPTR(val,context,str_content);
            str+=" --";
            str+=std::string(key);
            str+="=";
            str+=std::string(str_content);
            return true;
        }
        else {
            char ret[100];
            sprintf(ret,"typeof %s must be String!",key);
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,ret)));
            return false;
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

        std::string _args = "/usr/lib/judger/libjudger.so";

        if( !args[0]->IsObject()){
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"typeof argument must be Object!")));
        }

        Local<Value> argument = args[0];

        std::string int_vars[] = { "max_cpu_time","max_real_time","max_memory","memory_limit_check_only","max_stack","max_process_number","max_output_size","uid","gid"};
        std::string str_vars[] = { "input_path","output_path","error_path","exe_path","log_path","seccomp_rule_name"};

        for( auto var :int_vars){
            if( ! ToNumber(isolate,argument,(char *)var.c_str(),_args))
                return;
        }

        for( auto var : str_vars){
            if( !ToCStr(isolate,argument,(char *)var.c_str(),_args))
             return;
        }

        /* args */
        Local<Value> margs= argument->ToObject()->Get(context,String::NewFromUtf8(isolate,"args")).ToLocalChecked();
        if( margs->IsNullOrUndefined()){
            ;
        }
        else if( margs->IsArray()){
            Local<Array> args = margs.As<Array>();
            int len = args->Length();
            int i;
            for(i=0;i<len;i++){
                Local<Value> in = args->Get(i);
                if( !ToCStr(isolate,in,(char *)"args",_args))
                    return;
            }
        }
        else {  //not array
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"args must be a list")));
            return;
        }

        Local<Value> menv= argument->ToObject()->Get(context,String::NewFromUtf8(isolate,"env")).ToLocalChecked();
        if( menv->IsNullOrUndefined()){
            ;
        }
        else if( margs->IsArray()){
            Local<Array> env = menv.As<Array>();
            int len = env->Length();
            int i;
            for(i=0;i<len;i++){
                Local<Value> in = env->Get(i);
                if(!ToCStr(isolate,in,(char *)"env",_args))
                    return;

            }
        }
        else {  //not array
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"env must be a list")));
            return;
        }



        char buf[255];
        FILE *_result_f;
        std::string result;
        if(( _result_f = popen(_args.c_str(),"r"))==NULL)
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,"run /usr/lib/judger/libjudger.so failed!")));
        while(fgets(buf,sizeof(buf),_result_f)){
            result += std::string(buf);
        }
        pclose(_result_f);

        const char *pr = result.c_str();
        int len = strlen(pr);
        for(len--;len>=0;len--){
            if( pr[len] == '{')
                break;
        }

        if(len)
            printf("%*.*s",len,len,pr); //输出 程序的输出

        MaybeLocal<Value> mres = JSON::Parse(isolate,String::NewFromUtf8(isolate,pr+len));
        args.GetReturnValue().Set(mres.ToLocalChecked());
    }

    void init(Local<Object> exports)
    {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        NODE_SET_METHOD(exports, "run", Method);
        exports->Set( String::NewFromUtf8(isolate,"UNLIMITED"),Integer::New(isolate, -1));
        exports->Set(String::NewFromUtf8(isolate,"VERSION"),Integer::New(isolate, 0x020101));

        exports->Set(String::NewFromUtf8(isolate,"RESULT_SUCCESS"),Integer::New(isolate, 0));
        exports->Set(String::NewFromUtf8(isolate,"RESULT_WRONG_ANSWER"),Integer::New(isolate, -1));
        exports->Set(String::NewFromUtf8(isolate,"RESULT_CPU_TIME_LIMIT_EXCEEDED"),Integer::New(isolate, 1));
        exports->Set(String::NewFromUtf8(isolate,"RESULT_REAL_TIME_LIMIT_EXCEEDED"),Integer::New(isolate, 2));
        exports->Set(String::NewFromUtf8(isolate,"RESULT_MEMORY_LIMIT_EXCEEDED"),Integer::New(isolate, 3));
        exports->Set(String::NewFromUtf8(isolate,"RESULT_RUNTIME_ERROR"),Integer::New(isolate, 4));
        exports->Set(String::NewFromUtf8(isolate,"RESULT_SYSTEM_ERROR"),Integer::New(isolate, 5));

        exports->Set(String::NewFromUtf8(isolate,"ERROR_INVALID_CONFIG"),Integer::New(isolate, -1));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_FORK_FAILED"),Integer::New(isolate, -2));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_PTHREAD_FAILED"),Integer::New(isolate, -3));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_WAIT_FAILED"),Integer::New(isolate, -4));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_ROOT_REQUIRED"),Integer::New(isolate, -5));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_LOAD_SECCOMP_FAILED"),Integer::New(isolate, -6));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_SETRLIMIT_FAILED"),Integer::New(isolate, -7));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_DUP2_FAILED"),Integer::New(isolate, -8));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_SETUID_FAILED"),Integer::New(isolate, -9));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_EXECVE_FAILED"),Integer::New(isolate, -10));
        exports->Set(String::NewFromUtf8(isolate,"ERROR_SPJ_ERROR"),Integer::New(isolate, -11));
    }

    NODE_MODULE(addon, init)

}
