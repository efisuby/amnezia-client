#!/bin/sh

mkdir -p /data
echo "$TG_WS_PROXY_SECRET" > /data/secret
echo "$TG_WS_PROXY_PORT" > /data/port
echo "$TG_WS_PROXY_FAKE_TLS_DOMAIN" > /data/fake_tls_domain
chmod 600 /data/secret
