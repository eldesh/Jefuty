
APPNAME = 'jefuty'
VERSION = '0.1.0'

top = '.'
out = 'build'

def options(opt):
	opt.load('compiler_cxx')

def configure(conf):
	conf.recurse('jefuty')
	conf.recurse('test')

def build(bld):
	bld.recurse('jefuty')
	bld.recurse('test')

def dist(ctx):
	ctx.base_name = 'JEFUTY 0.1.0'
	ctx.algo      = 'zip'
	# exclude file patterns
	ctx.excl      = '**/.waf-1* **/*~ **/*.swp **/.lock-w*'
	ctx.files     = ctx.path.ant_glob('**/wscript')


