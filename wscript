srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "timedout"
  obj.source = "src/timedout.cc"
  obj.includes = ["."]
  obj.defines = "__STDC_LIMIT_MACROS __STDC_CONSTANT_MACROS"

