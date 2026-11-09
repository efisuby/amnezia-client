sudo docker run -d \
  --log-driver none \
  --restart always \
  --ulimit nofile=65536:65536 \
  -p $TG_WS_PROXY_PORT:$TG_WS_PROXY_PORT/tcp \
  -v amnezia-tgwsproxy-data:/data \
  --name $CONTAINER_NAME \
  $CONTAINER_NAME
