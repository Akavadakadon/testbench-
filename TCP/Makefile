img:
	docker build -t tcp/clisrv -f Dockerfile-TCP .
	docker pull nginx
	docker build -t tcp/nginximg -f "Dockerfile-TCP NginxImg" .
tcpcs:
	docker build -t tcp/clics -f Dockerfile-tcpcli .
	docker build -t tcp/scvcs -f Dockerfile-servcs .