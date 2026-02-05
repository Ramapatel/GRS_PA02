#!/bin/bash
# Roll No: MT25061
# PA02 – Part C Automated Experiment Script (FINAL FIXED)

set -e

OUTPUT="MT25061_Part_C_Results.csv"

# Always start fresh CSV
rm -f "$OUTPUT"

MSG_SIZES=(64 256 1024 4096)
THREADS=(1 2 4 8)
IMPLS=("a1" "a2" "a3")

echo "implementation,message_size,threads,total_time_sec,latency_us,throughput_mbps,cpu_cycles,context_switches,cache_misses" >> "$OUTPUT"

# Compile code
make clean
make

for impl in "${IMPLS[@]}"; do
  SERVER="./server_$impl"
  CLIENT="./client_$impl"

  for msg in "${MSG_SIZES[@]}"; do
    for th in "${THREADS[@]}"; do

      echo "Running $impl | msg=$msg | threads=$th"

      # Start server
      sudo ip netns exec ns_server $SERVER $msg $th &
      SERVER_PID=$!
      sleep 1

      # Run client with perf
      sudo perf stat \
        -e cycles,context-switches,cache-misses \
        -x, \
        ip netns exec ns_client $CLIENT $msg $th \
        > client_tmp.txt 2> perf_tmp.txt

      # Stop server
      sudo kill $SERVER_PID || true
      wait $SERVER_PID 2>/dev/null || true

      # =====================
      # PARSE CLIENT OUTPUT
      # =====================

      TOTAL_TIME=$(grep -o 'total_time=[^ ]*' client_tmp.txt | cut -d= -f2)
      LATENCY=$(grep -o 'latency_us=[^ ]*' client_tmp.txt | cut -d= -f2)
      BYTES=$(grep -o 'bytes=[^ ]*' client_tmp.txt | cut -d= -f2)

      [[ -z "$TOTAL_TIME" ]] && TOTAL_TIME="NA"
      [[ -z "$LATENCY" ]] && LATENCY="NA"
      [[ -z "$BYTES" ]] && BYTES="NA"

      # =====================
      # THROUGHPUT CALCULATION
      # =====================

      if [[ "$BYTES" != "NA" && "$TOTAL_TIME" != "NA" ]]; then
        THROUGHPUT=$(awk "BEGIN {printf \"%.2f\", ($BYTES*8)/(1000000*$TOTAL_TIME)}")
      else
        THROUGHPUT="NA"
      fi

      # =====================
      # PARSE PERF OUTPUT
      # =====================

      CYCLES=$(awk -F, '/cycles/ {print ($1=="<not counted>"?"NA":$1); exit}' perf_tmp.txt)
      CTX=$(awk -F, '/context-switches/ {print ($1=="<not counted>"?"NA":$1); exit}' perf_tmp.txt)
      CACHE=$(awk -F, '/cache-misses/ {print ($1=="<not counted>"?"NA":$1); exit}' perf_tmp.txt)

      [[ -z "$CYCLES" ]] && CYCLES="NA"
      [[ -z "$CTX" ]] && CTX="NA"
      [[ -z "$CACHE" ]] && CACHE="NA"

      # =====================
      # WRITE CLEAN CSV ROW
      # =====================

      printf "%s,%s,%s,%s,%s,%s,%s,%s,%s\n" \
      "$impl" "$msg" "$th" "$TOTAL_TIME" "$LATENCY" "$THROUGHPUT" "$CYCLES" "$CTX" "$CACHE" >> "$OUTPUT"

      sleep 1
    done
  done
done

rm -f client_tmp.txt perf_tmp.txt

