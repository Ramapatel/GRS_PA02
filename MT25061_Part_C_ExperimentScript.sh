#!/bin/bash
# Roll No: MT25061
# PA02 – Part C Automated Experiment Script (FINAL, STABLE)

set -e

OUTPUT="MT25061_Part_C_Results.csv"

# Remove old output
rm -f "$OUTPUT"

MSG_SIZES=(64 256 1024 4096)
THREADS=(1 2 4 8)
IMPLS=("a1" "a2" "a3")

# CSV header
echo "implementation,message_size,threads,total_time_sec,latency_us,throughput_mbps,cpu_cycles,context_switches,cache_misses" \
> "$OUTPUT"

# Compile everything cleanly
make clean
make

for impl in "${IMPLS[@]}"; do
  SERVER="./server_$impl"
  CLIENT="./client_$impl"

  for msg in "${MSG_SIZES[@]}"; do
    for th in "${THREADS[@]}"; do

      echo "Running $impl | msg=$msg | threads=$th"

      # Start server in server namespace
      sudo ip netns exec ns_server $SERVER $msg $th &
      SERVER_PID=$!
      sleep 1

      # Run client with perf (CLIENT-SIDE PERF)
      sudo perf stat \
        -e cycles,context-switches,cache-misses \
        -x, \
        ip netns exec ns_client $CLIENT $msg $th \
        > client_tmp.txt 2> perf_tmp.txt

      # Stop server (best-effort)
      sudo kill $SERVER_PID 2>/dev/null || true
      wait $SERVER_PID 2>/dev/null || true

      # -----------------------------
      # Parse client output
      # -----------------------------
      TOTAL_TIME=$(grep -o 'total_time=[^ ]*' client_tmp.txt | cut -d= -f2)
      LATENCY=$(grep -o 'latency_us=[^ ]*' client_tmp.txt | cut -d= -f2)
      BYTES=$(grep -o 'bytes=[^ ]*' client_tmp.txt | cut -d= -f2)

      [[ -z "$TOTAL_TIME" ]] && TOTAL_TIME="NA"
      [[ -z "$LATENCY" ]] && LATENCY="NA"
      [[ -z "$BYTES" ]] && BYTES="NA"

      # Throughput (Mbps)
      if [[ "$BYTES" != "NA" && "$TOTAL_TIME" != "NA" ]]; then
        THROUGHPUT=$(awk "BEGIN {printf \"%.2f\", ($BYTES*8)/(1000000*$TOTAL_TIME)}")
      else
        THROUGHPUT="NA"
      fi

      # -----------------------------
      # Parse perf output (CLIENT)
      # -----------------------------
      CYCLES=$(awk -F, '/cycles/ {print ($1=="<not counted>"?"NA":$1); exit}' perf_tmp.txt)
      CTX=$(awk -F, '/context-switches/ {print ($1=="<not counted>"?"NA":$1); exit}' perf_tmp.txt)
      CACHE=$(awk -F, '/cache-misses/ {print ($1=="<not counted>"?"NA":$1); exit}' perf_tmp.txt)

      [[ -z "$CYCLES" ]] && CYCLES="NA"
      [[ -z "$CTX" ]] && CTX="NA"
      [[ -z "$CACHE" ]] && CACHE="NA"

      # Write CSV row
      printf "%s,%s,%s,%s,%s,%s,%s,%s,%s\n" \
        "$impl" "$msg" "$th" "$TOTAL_TIME" "$LATENCY" "$THROUGHPUT" \
        "$CYCLES" "$CTX" "$CACHE" >> "$OUTPUT"

      sleep 1
    done
  done
done

rm -f client_tmp.txt perf_tmp.txt

echo "Part C completed successfully."
