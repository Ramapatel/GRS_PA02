import matplotlib.pyplot as plt

# System configuration (for titles)
SYS_CONFIG = "Linux Namespaces | 10s Duration"

# Common X-axis values
message_sizes = [64, 256, 1024, 4096]
threads = [1, 2, 4, 8]

# 1️⃣ Throughput vs Message Size (Threads = 4)

throughput_a1 = [123.28, 503.82, 2115.33, 3881.90]
throughput_a2 = [993.22, 2501.13, 5168.30, 12235.23]
throughput_a3 = [208.33, 1057.22, 3299.61, 8187.14]

plt.figure()
plt.plot(message_sizes, throughput_a1, marker='o', label="A1")
plt.plot(message_sizes, throughput_a2, marker='o', label="A2")
plt.plot(message_sizes, throughput_a3, marker='o', label="A3")
plt.xlabel("Message Size (Bytes)")
plt.ylabel("Throughput (Mbps)")
plt.title("Throughput vs Message Size (Threads = 4)\n" + SYS_CONFIG)
plt.legend()
plt.grid(True)
plt.savefig("MT25061_Throughput_vs_MessageSize.png")
plt.show()

# 2️⃣ Latency vs Threads (Message Size = 1024)

latency_a1 = [2.58, 2.77, 2.57, 2.55]
latency_a2 = [1.63, 1.63, 1.46, 1.46]
latency_a3 = [2.01, 2.20, 1.93, 2.07]

plt.figure()
plt.plot(threads, latency_a1, marker='o', label="A1")
plt.plot(threads, latency_a2, marker='o', label="A2")
plt.plot(threads, latency_a3, marker='o', label="A3")
plt.xlabel("Thread Count")
plt.ylabel("Latency (µs)")
plt.title("Latency vs Threads (Message Size = 1024 Bytes)\n" + SYS_CONFIG)
plt.legend()
plt.grid(True)
plt.savefig("MT25061_Latency_vs_Threads.png")
plt.show()

# 3️⃣ Cache Misses vs Message Size (Threads = 4)

cache_a1 = [29309579, 7288769, 5292956, 60480210]
cache_a2 = [5189150, 6889248, 2696670, 38019823]
cache_a3 = [13818890, 28191637, 13422531, 12196501]

plt.figure()
plt.plot(message_sizes, cache_a1, marker='o', label="A1")
plt.plot(message_sizes, cache_a2, marker='o', label="A2")
plt.plot(message_sizes, cache_a3, marker='o', label="A3")
plt.xlabel("Message Size (Bytes)")
plt.ylabel("Cache Misses")
plt.title("Cache Misses vs Message Size (Threads = 4)\n" + SYS_CONFIG)
plt.legend()
plt.grid(True)
plt.savefig("MT25061_CacheMiss_vs_MessageSize.png")
plt.show()

# 4️⃣ CPU Cycles per Byte vs Message Size (Threads = 4)

cycles_per_byte_a1 = [0.020, 0.006, 0.0022, 0.0010]
cycles_per_byte_a2 = [0.006, 0.002, 0.0009, 0.0005]
cycles_per_byte_a3 = [0.011, 0.004, 0.0014, 0.0008]

plt.figure()
plt.plot(message_sizes, cycles_per_byte_a1, marker='o', label="A1")
plt.plot(message_sizes, cycles_per_byte_a2, marker='o', label="A2")
plt.plot(message_sizes, cycles_per_byte_a3, marker='o', label="A3")
plt.xlabel("Message Size (Bytes)")
plt.ylabel("CPU Cycles per Byte")
plt.title("CPU Cycles per Byte vs Message Size (Threads = 4)\n" + SYS_CONFIG)
plt.legend()
plt.grid(True)
plt.savefig("MT25061_CyclesPerByte_vs_MessageSize.png")
plt.show()

print("All plots generated successfully (hardcoded values).")
