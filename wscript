VERSION='0.0.1'
APPNAME='mmgui'

top = '.'
out = 'build'

def options(opt):
  opt.load('compiler_cxx qt4')

def configure(conf):
  conf.load('compiler_cxx qt4')
  conf.recurse('src')

def build(bld):
  bld.recurse('src')

