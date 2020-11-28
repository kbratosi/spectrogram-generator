import platform

env = Environment()

if(platform.system() == "Linux"):
    env.Append( LINKFLAGS = '-Wall -Wextra -pedantic -std=c++11' )
    env.Append( LIBS = [ "libavcodec", "opencv_core", "boost_unit_test_framework" ] )
env.Program( target = 'a.out', source = 'main.cpp' )