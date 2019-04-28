const assert = require("assert")
const path = require("path")
var judger = require("../../bindings/NodeJS")
var base = require('./base.js')
var signal = require("os").constants.signals


var compile_base_path = base.compile_base_path = path.join(__dirname,"../test_src","integration")



describe('#常用测试',function(){


  before( ()=>{
    //mkdir /tmp/judge
    base.mkdir('/tmp/judge')
  })
  after( ()=>{
    base.rmdir('/tmp/judge')
  })

  it('args must be a list 1',()=>{
      assert.throws(()=>judger.run({args:{},output_path:"/dev/null"}),/args must be a list$/)
  })

  it('args must be a list 2',()=>{
      assert.throws(()=>judger.run({args:"1234",output_path:"/dev/null"}),/args must be a list$/)
  })

  it('test_env_must_be_list',()=>{
      assert.throws(()=>judger.run({env:"1234",output_path:"/dev/null"}),/env must be a list$/)
  })
  it('test_seccomp_rule_can_be_none',()=>{
      let ret = judger.run({ output_path:"/dev/null",exe_path:"/bin/ls",args:["/"],env:["a=b"] })
      assert.strictEqual(ret.result,0)
  })

  it('test normal.c',()=>{
    let config = base.baseconfig()
      config["exe_path"]  = base.compile_c("normal.c")
      config["input_path"] = base.make_input("judger_test")
      config["output_path"] = config["error_path"] = base.output_path()
      let ret = judger.run(config)
      let output = "judger_test\nHello world"
      let content = base.read_input( base.output_path())
      assert.strictEqual(content,output)
  })

  it('test math.c' ,()=>{
      let config = base.baseconfig()
      config["exe_path"]  = base.compile_c("math.c")
      config["output_path"] = config["error_path"] = base.output_path()
      let ret = judger.run(config)
      assert.strictEqual(ret.result,judger.RESULT_SUCCESS)
      let content = base.read_input( base.output_path())
      assert.strictEqual(content,"abs 1024")
  })

  it('test_args',()=>{
      let config = base.baseconfig()
      config["exe_path"] = base.compile_c("args.c")
      config["args"] = ["test", "hehe", "000"]
      config["output_path"] = config["error_path"] = base.output_path()

      let result = judger.run(config)
      let output = "argv[0]: /tmp/judge/main\nargv[1]: test\nargv[2]: hehe\nargv[3]: 000\n"
      assert.strictEqual(result.result, judger.RESULT_SUCCESS)
      assert.strictEqual(output, base.read_input(config["output_path"]))

  })

  it('test env',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_c("env.c")
        config["output_path"] = config["error_path"] = base.output_path()
        config["env"] = ["env=judger_test","test=judger"]
        let result = judger.run(config)
        let output = "judger_test\njudger\n"
        assert.strictEqual(result["result"], judger.RESULT_SUCCESS)
        assert.strictEqual(output, base.read_input(config["output_path"]))

  })

  it('test real time',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_c("sleep.c")
        config["max_real_time"] = 3
        let result = judger.run(config)
        //assert.strictEqual(result["result"], judger.RESULT_REAL_TIME_LIMIT_EXCEEDED)
        assert.strictEqual(result["signal"], signal.SIGKILL)
        assert.ok(result["real_time"] >= config["max_real_time"])

  })

  it('test cpu time',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_c("while1.c")
        let result= judger.run(config)

        assert.strictEqual(result["result"], judger.RESULT_CPU_TIME_LIMIT_EXCEEDED)
        assert.strictEqual(result["signal"], signal.SIGKILL)
        assert.ok(result["cpu_time"] >= config["max_cpu_time"])

  })

  it('test memory1',()=>{
        let config = base.baseconfig()
        config["max_memory"] = 64 * 1024 * 1024
        config["exe_path"] = base.compile_c("memory1.c")
        let result= judger.run(config)
        //# malloc succeeded
        assert.ok(result["memory"] > 80 * 1024 * 1024)
        assert.strictEqual(result["result"], judger.RESULT_MEMORY_LIMIT_EXCEEDED)
  })

  it('test memory2',()=>{
        let config = base.baseconfig()
        config["max_memory"] = 64 * 1024 * 1024
        config["exe_path"] = base.compile_c("memory2.c")
        let result= judger.run(config)
        //# malloc failed, return 1
        assert.strictEqual(result["exit_code"], 1)
        //# malloc failed, so it should use a little memory
        assert.ok(result["memory"] < 12 * 1024 * 1024)
        assert.strictEqual(result["result"], judger.RESULT_RUNTIME_ERROR)
  })

  it('test memory3',()=>{
        let config = base.baseconfig()
        config["max_memory"] = 512 * 1024 * 1024
        config["exe_path"] = base.compile_c("memory3.c")
        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_SUCCESS)
        assert.ok(result["memory"] >= 102400000 * 4)
  })
  it('test re1',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_c("re1.c")
        let result= judger.run(config)
        //# re1.c return 25
        assert.strictEqual(result["exit_code"], 25)

  })
  it('test re2',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_c("re2.c")
        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_RUNTIME_ERROR)
        assert.strictEqual(result["signal"], signal.SIGSEGV)
  })
  it('test child proc cpu time limit',()=>{
        let config = base.baseconfig()

        config["exe_path"] = base.compile_c("child_proc_cpu_time_limit.c")
        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_CPU_TIME_LIMIT_EXCEEDED)

  })
  it('test child proc real time limit',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_c("child_proc_real_time_limit.c")
        config["max_real_time"] = 3
        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_REAL_TIME_LIMIT_EXCEEDED)
        assert.strictEqual(result["signal"], signal.SIGKILL)
  })
  it('test stdout and stderr',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_c("stdout_stderr.c")
        config["output_path"] = config["error_path"] = base.output_path()
        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_SUCCESS)
        let output = "stderr\n+++++++++++++++\n--------------\nstdout\n"
        assert.strictEqual(output, base.read_input(config["output_path"]))
  })
  it('test uid and gid',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_c("uid_gid.c")
        config["output_path"] = config["error_path"] = base.output_path()
        config["uid"] = 65534
        config["gid"] = 65534
        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_SUCCESS)
        output = "uid=65534(nobody) gid=65534(nogroup) groups=65534(nogroup)\nuid 65534\ngid 65534\n"
        assert.strictEqual(output, base.read_input(config["output_path"]))

  })
  it('test gcc random',()=>{
        let config = base.baseconfig()
        config["max_memory"] = judger.UNLIMITED
        config["exe_path"] = "/usr/bin/gcc"
        config["args"] = [path.join(compile_base_path,'gcc_random.c'),
                          "-o", '/tmp/judge/gcc_random']
        let result= judger.run(config)
        assert.ok(result["real_time"] >= 2000)

  })
  it('test cpp meta',()=>{
        let config = base.baseconfig()
        config["exe_path"] = "/usr/bin/g++"
        config["max_memory"] = 1024 * 1024 * 1024
        config["args"] = [path.join(compile_base_path,'cpp_meta.cpp'),
                          "-o",'/tmp/judge/cpp_meta']
        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_CPU_TIME_LIMIT_EXCEEDED)
        assert.ok(result["cpu_time"] > 1500)
        assert.ok(result["real_time"] >= 2000)
  })
  it('test output size',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_c("output_size.c")
        config["output_path"] = base.output_path()
        config["max_output_size"] = 1000 * 10
        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_RUNTIME_ERROR)
        assert.ok(base.read_input("/tmp/judge/fsize_test").length <= 1000*10)
  })
  it('test stack size',()=>{
        let config = base.baseconfig()
        config["max_memory"] = 256 * 1024 * 1024
        config["exe_path"] = base.compile_c("stack.c")
        config["max_stack"] = 16 * 1024 * 1024
        config["output_path"] = config["error_path"] = base.output_path()

        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_RUNTIME_ERROR)

        config["max_stack"] = 128 * 1024 * 1024
        result= judger.run(config).result
        assert.strictEqual(result["result"], judger.RESULT_SUCCESS)
        assert.strictEqual("big stack", base.read_input(config["output_path"]))

  })
  it('test writev',()=>{
        let config = base.baseconfig()
        config["exe_path"] = base.compile_cpp("writev.cpp")
        config["seccomp_rule_name"] = "c_cpp"
        config["input_path"] = base.make_input("111" * 10000 + "\n")
        config["output_path"] = config["error_path"] = base.output_path()

        let result= judger.run(config)
        assert.strictEqual(result["result"], judger.RESULT_SUCCESS)
  })
})
