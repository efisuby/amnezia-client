#!/bin/sh

BLOCKLIST_URL="https://adguardteam.github.io/AdGuardSDNSFilter/Filters/filter.txt"
BLOCKLIST_CONF="/opt/unbound/etc/unbound/a-records.conf"
UPDATE_INTERVAL=86400
RETRY_INTERVAL=3600

update_blocklist() {
  tmp_list="$(mktemp)"
  tmp_conf="$(mktemp)"
  if curl -fsSL --retry 3 --connect-timeout 10 --max-time 300 -o "$tmp_list" "$BLOCKLIST_URL" \
    && awk '
      /^@@\|\|[a-z0-9._-]+\^(\|)?$/ { d = substr($0, 5); sub(/\^\|?$/, "", d); allowed[d] = 1; next }
      /^\|\|[a-z0-9._-]+\^$/ { d = substr($0, 3); sub(/\^$/, "", d); if (d !~ /^[0-9.]+$/) blocked[d] = 1 }
      END {
        for (d in blocked) if (!(d in allowed)) printf "local-zone: \"%s.\" always_nxdomain\n", d
        for (d in allowed) printf "local-zone: \"%s.\" transparent\n", d
      }
    ' "$tmp_list" > "$tmp_conf" \
    && [ -s "$tmp_conf" ]; then
    chmod 644 "$tmp_conf"
    mv -f "$tmp_conf" "$BLOCKLIST_CONF"
    rm -f "$tmp_list"
    return 0
  fi
  rm -f "$tmp_list" "$tmp_conf"
  return 1
}

update_loop() {
  while true; do
    if update_blocklist; then
      unbound_pid="$(pidof unbound)"
      [ -n "$unbound_pid" ] && kill -HUP $unbound_pid
      sleep $UPDATE_INTERVAL
    else
      sleep $RETRY_INTERVAL
    fi
  done
}

update_loop > /dev/null 2>&1 &

if [ "$$" = "1" ]; then
  exec /unbound.sh
fi
