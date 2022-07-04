import requests
import sys
import subprocess
import os 
import time
import urllib3

import socket, ssl

def run():
    n = len(sys.argv)
    print(n)
    CC_cmd = ["cat","/proc/sys/net/ipv4/tcp_congestion_control"]
    subprocess.call(CC_cmd)
    if n == 1:
        prepare_envs()
        filename = build_filename(BYTES_COUNT)
        ping_cmd = ["ping", "-c3", "192.168.44.101"]
        subprocess.call(ping_cmd)
        url = "https://192.168.44.101:443"
        out_filename = _generate_file_of_result_name(BYTES_COUNT)
        dir= "/result/"
        if ssl_mode == 1:
            dir = dir + "ssl/"
        elif ssl_mode == 2:
            dir = dir + "nossl/"
        else:
            return
        if not os.path.exists(dir):    
            os.makedirs(dir)
        f = open(dir+out_filename, "w")
        print("created ", out_filename)
        f.write("BYTES_COUNT\tP2P_DELAY\tERROR_RATE\tTIME\n")
        
        for i in range(ITERATIONS): 
            start = end = int(round(time.time() * 1000000))
            if ssl_mode==1:
                #A()
                res = sslRequest()
            elif ssl_mode==2:
                res = nosslRequest()
            else:
                return
            i = i + res
            print (i)
            if (res == -1): 
                continue
            
            end = int(round(time.time() * 1000000))
            f.write("{}\t{}\t{}\t{}\n".format(BYTES_COUNT,P2P_DELAY, ERROR_RATE, (end - start)))
        f.close()    
    else:
        while(1<3):
            time.sleep(100)
                    
def A():
    print("sslRequest A")
    HOST = '192.168.44.101'
    PORT = 443
    BUFFER_SIZE = 10240
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    s_sock = context.wrap_socket(s, server_hostname=HOST)
    #.settimeout(2.0)
    s_sock.connect((HOST, PORT))
    s_sock.close()
    print("connect")
    MESSAGE = "GET /resp_1024_b.txt HTTP/1.1\r\nHost:%s\r\n\r\n" % HOST    
    for i in range(ITERATIONS):
        s_sock.send(MESSAGE.encode())
        data = s_sock.recv(BUFFER_SIZE)
    s_sock.close()
    return 0

def sslRequest():
    try:
        print("sslRequest")
        HOST = '192.168.44.101'
        PORT = 443
        BUFFER_SIZE = 10240
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
        s_sock = context.wrap_socket(s, server_hostname=HOST)
        s_sock.settimeout(1.0)
        s_sock.connect((HOST, PORT))
        MESSAGE = "GET /resp_1024_b.txt HTTP/1.1\r\nHost:%s\r\n\r\n" % HOST    
        s_sock.send(MESSAGE.encode())
        data = s_sock.recv(BUFFER_SIZE)
        s_sock.close()
        return 0
    except Exception as e:
        print(e)
        return -1
  
def nosslRequest():
    try:
        print("nosslRequest")
        HOST = '192.168.44.101'
        PORT = 80
        BUFFER_SIZE = 10240
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1.0)
        s.connect((HOST, PORT))
        MESSAGE = "GET /resp_1024_b.txt HTTP/1.1\r\nHost:%s\r\n\r\n" % HOST    
        s.send(MESSAGE.encode())
        data = s.recv(BUFFER_SIZE)
        s.close()
        return 0
    except Exception as e:
        print(e)
        return -1
  
def prepare_envs():
    is_failed = False

    global BYTES_COUNT
    global ITERATIONS
    global P2P_DELAY
    global ERROR_RATE
    global ssl_mode
    global cc

    BYTES_COUNT = os.environ.get('BYTES_COUNT', 'none')
    ITERATIONS = int(os.environ.get('ITERATIONS', '1000'))
    P2P_DELAY = int(''.join(filter(str.isdigit, os.environ.get('P2P_DELAY', '1')))) 
    ERROR_RATE = float(os.environ.get('ERROR_RATE', '1'))
    ssl_mode = int(os.environ.get('ssl', 'none'))
    cc = os.environ.get('cc', 'none')
    
    print("cc-", cc, "ssl_mode-", ssl_mode, " BYTES_COUNT-",BYTES_COUNT," ITERATIONS-", ITERATIONS," P2P_DELAY-", P2P_DELAY," loss-", ERROR_RATE)
    
    
    if BYTES_COUNT == 'none':
        print("[error] BYTES_COUNT env is not set!")
        is_failed = True
    if ssl == 'none':
        print("[error] ssl env is not set!")
        is_failed = True

    if is_failed:
        exit(1)

def _generate_file_of_result_name(size):
    return "GET_res_with_"+str(size)+"_"+str(ERROR_RATE)+"_"+str(P2P_DELAY)+"_b.txt"
    return "{}_res_with_{}_b_{}ms_{}-loss.txt".format("GET", size, P2P_DELAY, ERROR_RATE)

def build_filename(size):
    return "resp_" + str(size) + "_b.txt"
    
if __name__ == "__main__":
    urllib3.disable_warnings()
    run()