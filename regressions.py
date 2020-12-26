import os
import shutil
import argparse
import multiprocessing

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--verbose", action="store_true", dest="verbose",default=False, help="Option to run tests with verbose output sent to stdout")
    args = parser.parse_args()

    build_dir = "build"
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)

    configure_retval =  os.system("cmake -S . -B %s -DBUILD_TESTING=ON" % (build_dir))
    if configure_retval != 0:
        print("Configure failed with code:" + str(configure_retval))
        exit(config_retval)

    build_retval = os.system("cmake --build %s" % (build_dir))
    if build_retval != 0:
        print("Build failed with code:" + str(build_retval))
        exit(build_retval)

    os.chdir("%s" % (build_dir))
    
    test_command = "ctest --parallel %d" % (multiprocessing.cpu_count())
    if args.verbose:
        test_command += " " + "-V"

    os.system(test_command)