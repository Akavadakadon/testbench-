TCP клиент и сервер. Клиент написан на Python, а сервер - nginx с открытыми портами для SSL и noSSL.
Запускать через docker-compose.yml. Контейнер клиента в Dockerfile-TCP client, сервера - Dockerfile-TCP server.
Взаимодействие клиента и сервера такое же, как описано в рид ми в папке уровнем выше
Не забыть собрать первичые образы через make img или make tcpcs.