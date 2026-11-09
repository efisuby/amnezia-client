#!/bin/sh

echo "Container startup (tg-ws-proxy)"

SECRET=$(cat /data/secret)
PORT=$(cat /data/port)
FAKE_TLS_DOMAIN=$(cat /data/fake_tls_domain 2>/dev/null)

if [ -n "$FAKE_TLS_DOMAIN" ]; then
    exec /usr/local/bin/tg-ws-proxy --host 0.0.0.0 --port "$PORT" --secret "$SECRET" --listen-faketls-domain "$FAKE_TLS_DOMAIN"
fi
exec /usr/local/bin/tg-ws-proxy --host 0.0.0.0 --port "$PORT" --secret "$SECRET"
