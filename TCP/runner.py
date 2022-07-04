import subprocess
import argparse

#python3 runner.py --iterations=100 --byte_sizes=1024 --delays=50ms --error_rates=0.0,0.01,0.1
#python3 runner.py --iterations=10 --byte_sizes=1024 --delays=1ms  --error_rates=0.0

def run():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--iterations',
        type=int,
        default=50,
        help='iterations count (default::100)'
    )
    parser.add_argument(
        '--ssl',
        type=int,
        default=1,
        help='enable ssl 1-true 2-false(default::True)'
    )
    parser.add_argument(
        '--cc',
        type=str,
        default="afk",
        help='cc type (default::afk)'
    )
    parser.add_argument(
        '--byte_sizes',
        type=str,
        #default='1024,2048',
        default='512,1024,2048',#512,1024,2048 1048576
        help='comma-separated byte sizes'
    )
    parser.add_argument(
        '--delays',
        type=str,
        default='1ms,50ms,100ms',
        help='comma-separated delays'
    )
    parser.add_argument(
        '--error_rates',
        type=str,
        default='0.05', #0.0,0.01,0.05,0.1
        help='comma-separated error rates'
    )
    parser.add_argument('--strict_iter', dest='strict_iter', action='store_true')
    parser.add_argument('--no_strict_iter', dest='strict_iter', action='store_false')
    parser.set_defaults(strict_iter=True)
    args = parser.parse_args()
    byte_sizes = [int(b) for b in args.byte_sizes.split(',')]
    delays = args.delays.split(',')
    error_rates = args.error_rates.split(',')
    cc = args.cc.split(',')
    print(args.ssl)
    execute_runner(byte_sizes, delays, error_rates, cc, args.ssl, args.iterations, args.strict_iter)
    return

def execute_runner(byte_sizes, delays, error_rates, cc, ssl, iterations, strict_iter):
    docker_compose_cmd_build = [
        "docker",
        "compose",
        "build"
    ]
    #subprocess.call(docker_compose_cmd_build)
    docker_compose_cmd_up = [
        "docker-compose",
        "up",
        "--abort-on-container-exit"
    ]
    docker_compose_cmd_down = ["docker-compose", "down"]
    i = 0
    for s in byte_sizes:
        for delay in delays:
            for er in error_rates:
                i=i+1
                prepare_env(delay, er, s, cc, ssl, iterations, strict_iter)
                print(delay, er, s, cc, ssl, iterations, strict_iter)
                subprocess.call(docker_compose_cmd_up)
                subprocess.call(docker_compose_cmd_down)
    print("Done:", i)
    return
    
def prepare_env(delay, er, bytes_count, cc, ssl, iterations, strict_iter):
    str_res = '''P2P_DELAY={}
ERROR_RATE={}
ENABLE_PCAP=1
BYTES_COUNT={}
cc={}
ssl={}
ITERATIONS={}
ITER_ONLY_OK={}'''.format(delay, er, bytes_count, cc, ssl, iterations, strict_iter)
    print("CURRENT ENV:\n", str_res)
    env_file = open(".env", "w")
    env_file.write(str_res)
    env_file.close()

if __name__ == "__main__":
    run()
