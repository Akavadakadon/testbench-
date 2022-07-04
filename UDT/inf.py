import sys
import subprocess
import os 

def run():
    prepare_envs()
    ping_cmd = ["ping", "-c6", "192.168.44.101"]
    subprocess.call(ping_cmd)
    args = ("/app/appclient", "192.168.44.101", "6121", str(P2P_DELAY), str(ERROR_RATE), str(BYTES_COUNT), str(ITERATIONS))
    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    popen.wait()
    output = popen.stdout.read()


def prepare_envs():
    is_failed = False

    global BYTES_COUNT
    global ITERATIONS
    global P2P_DELAY
    global ERROR_RATE

    BYTES_COUNT = os.environ.get('BYTES_COUNT', 'none')
    ITERATIONS = int(os.environ.get('ITERATIONS', '1000'))
    P2P_DELAY = int(''.join(filter(str.isdigit, os.environ.get('P2P_DELAY', '1')))) 
    ERROR_RATE = float(os.environ.get('ERROR_RATE', '1'))
    
    print("BYTES_COUNT-",BYTES_COUNT," ITERATIONS-", ITERATIONS," P2P_DELAY-", P2P_DELAY," loss-", ERROR_RATE)
    
    
    if BYTES_COUNT == 'none':
        print("[error] BYTES_COUNT env is not set!")
        is_failed = True

    if is_failed:
        exit(1)
        
if __name__ == "__main__":
    run()
