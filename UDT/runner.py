import subprocess
import argparse

#python3 runner.py --iterations=10 --byte_sizes=1048576 --delays=1ms  --error_rates=0.0,0.05

def run():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--iterations',
        type=int,
        default=100,
        help='iterations count (default::100)'
    )
    parser.add_argument(
        '--byte_sizes',
        type=str,
        default='512,1024,2048',#
        help='comma-separated byte sizes'
    )
    parser.add_argument(
        '--delays',
        type=str,
        default='1ms,50ms,100ms',#
        help='comma-separated delays'
    )
    parser.add_argument(
        '--error_rates',
        type=str,
        default='0.0,0.01,0.05,0.1',#0.0,0.01,0.05,0.1
        help='comma-separated error rates'
    )
    parser.add_argument('--strict_iter', dest='strict_iter', action='store_true')
    parser.add_argument('--no_strict_iter', dest='strict_iter', action='store_false')
    parser.set_defaults(strict_iter=True)
    args = parser.parse_args()
    byte_sizes = [int(b) for b in args.byte_sizes.split(',')]
    delays = args.delays.split(',')
    error_rates = args.error_rates.split(',')
    execute_runner(byte_sizes, delays, error_rates, args.iterations, args.strict_iter)
    return

def execute_runner(byte_sizes, delays, error_rates, iterations, strict_iter):
    docker_compose_cmd_build = [
        "docker-compose",
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
                prepare_env(delay, er, s, iterations, strict_iter)
                subprocess.call(docker_compose_cmd_up)
                subprocess.call(docker_compose_cmd_down)
    print("Done:", i)
    return
    
def prepare_env(delay, er, bytes_count, iterations, strict_iter):
    str_res = '''P2P_DELAY={}
ERROR_RATE={}
ENABLE_PCAP=1
BYTES_COUNT={}
ITERATIONS={}
ITER_ONLY_OK={}'''.format(delay, er, bytes_count, iterations, strict_iter)
    print("CURRENT ENV:\n", str_res)
    env_file = open(".env", "w")
    env_file.write(str_res)
    env_file.close()

if __name__ == "__main__":
    run()
