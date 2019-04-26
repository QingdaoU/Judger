const fs = require("fs")
const path = require("path")
const execSync = require("child_process").execSync

var out_base_path = '/tmp/judge'
var compile_base_path = ""
var compile_c = function(name){
  out_path =path.join(out_base_path,'main')
  execSync(`gcc -o  ${out_path} ${path.join(this.compile_base_path,name)}`)
  return out_path
}

var compile_cpp = function(name){
  out_path =path.join(out_base_path,'main')
  execSync(`g++ -o  ${out_path} ${path.join(this.compile_base_path,name)}`)
  return out_path
}

var make_input = content =>{
  fs.writeFileSync('/tmp/judge/input',content,{'encoding':'utf-8'})
    return '/tmp/judge/input'
}

var read_input = read_path => {
  return fs.readFileSync(read_path,{'encoding':'utf-8'})
}

var output_path = ()=>{
  return '/tmp/judge/output'
}

var error_path = ()=>{
  return '/tmp/judge/error'
}

var mkdir = _path => {
  execSync(`mkdir -p ${_path}`)
}

var rmdir = _path => {
  execSync(`rm -r ${_path}`)
}

var baseconfig = ()=>{
  return {
    max_cpu_time:1000,
    max_real_time:8000,
    max_memory:128*1024*1024,
    max_stack:128*1024*1024,
    max_process_number:8,
    max_output_size:128*1024*1024,
    exe_path:"/dev/null",
    input_path:"/dev/null",
    output_path:"/dev/null",
    error_path:"/dev/null",
    args:[],
    env:[],
    log_path:"/dev/null",
    seccomp_rule_name:null,
    gid:0,
    uid:0,
    info:false,
    debug:false,
  }
}

module.exports = {
  compile_c,compile_cpp,make_input,read_input,mkdir,rmdir,baseconfig,compile_base_path,
  output_path,
  error_path
}
