VERSION='0.0.1'
APPNAME='mmgui'

top = '.'
out = 'build'

import waflib.Configure
waflib.Configure.autoconfig = True

def options(opt):
  opt.load('compiler_cxx qt4')

def configure(conf):
  conf.load('compiler_cxx qt4')
  conf.recurse('src')
  conf.env.append_value('CFLAGS',['-g'])
  conf.env.append_value('CXXFLAGS',['-g'])

def build(bld):
  bld.recurse('src')

