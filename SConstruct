from __future__ import print_function

env = Environment(
	LIBPATH = ['.', '/usr/lib', '/usr/local/lib'],
    CPPPATH = ['.', '/usr/include', '/usr/local/include'],
    CCFLAGS = ['-Wall'],
    OMP = 0,
    CRYPTO = 1,
    BUILD_TESTS = 0,
    RUN_TESTS = 0,
    LIBS = ['copri', 'pool', 'array', 'gmp']
)

AddOption("--test", action="store_true", dest="test", default=False, help="build tests")
AddOption("--no-omp", action="store_false", dest="omp", default=True, help="don't use OpenMP multithreading")
AddOption("--run-test", action="store_true", dest="runtest", default=False, help="run the tests")

env['BUILD_TESTS'] = GetOption('test')
if GetOption('runtest'):
	env['BUILD_TESTS'] = True
	env['RUN_TESTS'] = True

if env['PLATFORM'] == 'posix':
	if GetOption('omp'):
		env['OMP'] = 1
elif env['PLATFORM'] == 'darwin':
	#env.AppendUnique(CCFLAGS =['-Wno-deprecated-declarations'])
	env['CC'] = 'clang'
else:
	print('Platform %s is not supported' % env['PLATFORM'])

if env['OMP']:
	env.AppendUnique(LINKFLAGS = ['-fopenmp'])
	env.AppendUnique(CCFLAGS =['-fopenmp'])

conf = Configure(env)

if not conf.CheckCC():
	print('Did not find c compiler, exiting!')
	Exit(1);

if not conf.CheckCHeader('gmp.h'):
	print('Did not find gmp, exiting!')
	Exit(1)
	
if not conf.CheckCHeader('unistd.h'):
	print('Did not find POSIX unistd.h, exiting!')
	Exit(1);
	
if not conf.CheckCHeader('openssl/rsa.h') or not conf.CheckCHeader('openssl/sha.h'):
	print('WARNING: Did not find openssl!')
	conf.env['CRYPTO'] = 0

env = conf.Finish()

if env['BUILD_TESTS']:
	env.AppendUnique(CCFLAGS =['-g'])

env.Library('array', ['array.c'], LIBS = ['gmp'])

env.Library('pool', ['pool.c'], LIBS = ['gmp', 'array'])

env.Library('copri', ['copri.c'])

if env['CRYPTO']:
	env.Program('gen', ['gen.c'], LIBS = ['array', 'gmp', 'crypto'], CCFLAGS =['-Wno-deprecated-declarations'])

if env['BUILD_TESTS']:	
	prev_cmd = None
	for name in [
		'array',
		'arrayio',
		'prod',
		'twopower',
		'gcdppippo',
		'gcdppgpple',
		'appendcb',
		'split',
		'cbextend',
		'cbmerge',
		'cb',
		'findfactor',
		'pool'
		]:
		rel = 'test/test-'+name
		test = env.Program(rel, [rel+'.c'])
		if env['RUN_TESTS']:
			deps = [test]
			if prev_cmd:
				deps.append(prev_cmd)
			cmd = Command(rel+'_test', deps, test[0].abspath)			
			AlwaysBuild(cmd)
			prev_cmd = cmd
	
env.Program('app', ['app.c'])

env.Program('app-n2', ['app-n2.c'], LIBS = ['array', 'copri', 'gmp'])

env.Program('array-util', ['array-util.c'], LIBS = ['array', 'gmp'])

env.Program('csv2gmp', ['csv2gmp.c'], LIBS = ['gmp'])

def config_h_build(target, source, env):

	config_h_defines = {
		"install_prefix": "/usr/local",
		"version_str": "0.9",
		"openmp": env['OMP'],
		"crypto": env['CRYPTO'],
	}

	for a_target, a_source in zip(target, source):
		config_h = file(str(a_target), "w")
		config_h_in = file(str(a_source), "r")
		config_h.write(config_h_in.read() % config_h_defines)
		config_h_in.close()
		config_h.close()

env.AlwaysBuild(env.Command('config.h', 'config.h.in', config_h_build))

if not GetOption('clean'):
	import atexit
	
	if not env['CRYPTO']:
		atexit.register(lambda: print('WARNING: OpenSSL is not installed!\n\t The \'gen\' util has not been build.'))

	if not env['OMP']:
		atexit.register(lambda: print('WARNING: No OpenMP compiler found!\n\t This build does not support multithreading.'))


