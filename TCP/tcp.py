import socket, ssl, sys
HOST = '127.0.0.1'
TCP_PORT = 443
BUFFER_SIZE = 10240

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
s_sock = context.wrap_socket(s, server_hostname=HOST)
s_sock.connect((HOST, 443))
MESSAGE = "GET /resp_1024_b.txt HTTP/1.1\r\nHost:%s\r\n\r\n" % HOST                          
print(MESSAGE)
s_sock.send(MESSAGE.encode())
data = s_sock.recv(BUFFER_SIZE)
s_sock.close()

print ("received data:{} \n {}".format( sys.getsizeof(data), data))