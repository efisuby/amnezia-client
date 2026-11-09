FROM valnesfjord/tg-ws-proxy-rs:latest AS upstream

FROM alpine:3.20

LABEL maintainer="AmneziaVPN"

RUN apk add --no-cache ca-certificates
COPY --from=upstream /usr/local/bin/tg-ws-proxy /usr/local/bin/tg-ws-proxy

RUN mkdir -p /opt/amnezia /data
RUN printf '#!/bin/sh\ntail -f /dev/null\n' > /opt/amnezia/start.sh && \
    chmod a+x /opt/amnezia/start.sh

VOLUME /data
ENTRYPOINT ["/bin/sh", "/opt/amnezia/start.sh"]
CMD [""]
