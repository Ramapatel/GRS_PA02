import pandas as pd
import matplotlib.pyplot as plt

# ===============================
# Load CSV
# ===============================
data = pd.read_csv("MT25061_Part_C_Results.csv")

# Clean implementation column (FIXES missing A3 bug)
data["implementation"] = data["implementation"].str.strip().str.lower()

# Convert numeric columns safely
numeric_cols = [
    "message_size", "threads", "total_time_sec",
    "latency_us", "throughput_mbps",
    "cpu_cycles", "context_switches", "cache_misses"
]

for col in numeric_cols:
    data[col] = pd.to_numeric(data[col], errors="coerce")

# ===============================
# Compute cycles per byte
# ===============================
data["bytes_transferred"] = (
    data["throughput_mbps"] * 1e6 / 8 * data["total_time_sec"]
)

data["cycles_per_byte"] = data["cpu_cycles"] / data["bytes_transferred"]

# ===============================
SYS_CONFIG = "System: Linux Namespace Setup | 10s Duration"
# ===============================


# =========================================================
# 1️⃣ Throughput vs Message Size (Threads = 4)
# =========================================================
plt.figure()

for impl in ["a1", "a2", "a3"]:
    subset = data[
        (data["implementation"] == impl) &
        (data["threads"] == 4)
    ].sort_values("message_size")

    if subset.empty:
        print(f"Missing data for {impl}")
        continue

    plt.plot(
        subset["message_size"],
        subset["throughput_mbps"],
        marker='o',
        label=impl.upper()
    )

plt.xlabel("Message Size (Bytes)")
plt.ylabel("Throughput (Mbps)")
plt.title("Throughput vs Message Size (Threads = 4)\n" + SYS_CONFIG)
plt.legend()
plt.grid(True)
plt.savefig("MT25061_Throughput_vs_MessageSize.png")
plt.show()


# =========================================================
# 2️⃣ Latency vs Thread Count (Message Size = 1024)
# =========================================================
plt.figure()

for impl in ["a1", "a2", "a3"]:
    subset = data[
        (data["implementation"] == impl) &
        (data["message_size"] == 1024)
    ].sort_values("threads")

    if subset.empty:
        print(f"Missing data for {impl}")
        continue

    plt.plot(
        subset["threads"],
        subset["latency_us"],
        marker='o',
        label=impl.upper()
    )

plt.xlabel("Thread Count")
plt.ylabel("Latency (µs)")
plt.title("Latency vs Threads (Message Size = 1024 Bytes)\n" + SYS_CONFIG)
plt.legend()
plt.grid(True)
plt.savefig("MT25061_Latency_vs_Threads.png")
plt.show()


# =========================================================
# 3️⃣ Cache Misses vs Message Size (Threads = 4)
# =========================================================
plt.figure()

for impl in ["a1", "a2", "a3"]:
    subset = data[
        (data["implementation"] == impl) &
        (data["threads"] == 4)
    ].sort_values("message_size")

    if subset.empty:
        print(f"Missing data for {impl}")
        continue

    plt.plot(
        subset["message_size"],
        subset["cache_misses"],
        marker='o',
        label=impl.upper()
    )

plt.xlabel("Message Size (Bytes)")
plt.ylabel("Cache Misses")
plt.title("Cache Misses vs Message Size (Threads = 4)\n" + SYS_CONFIG)
plt.legend()
plt.grid(True)
plt.savefig("MT25061_CacheMiss_vs_MessageSize.png")
plt.show()


# =========================================================
# 4️⃣ CPU Cycles vs Threads (Message Size = 1024)
# =========================================================
plt.figure()

for impl in ["a1", "a2", "a3"]:
    subset = data[
        (data["implementation"] == impl) &
        (data["message_size"] == 1024)
    ].sort_values("threads")

    if subset.empty:
        print(f"Missing data for {impl}")
        continue

    plt.plot(
        subset["threads"],
        subset["cpu_cycles"],
        marker='o',
        label=impl.upper()
    )

plt.xlabel("Thread Count")
plt.ylabel("CPU Cycles")
plt.title("CPU Cycles vs Threads (Message Size = 1024 Bytes)\n" + SYS_CONFIG)
plt.legend()
plt.grid(True)
plt.savefig("MT25061_CPUCycles_vs_Threads.png")
plt.show()


# =========================================================
# 5️⃣ CPU Cycles Per Byte vs Message Size (Threads = 4)
# =========================================================
plt.figure()

for impl in ["a1", "a2", "a3"]:
    subset = data[
        (data["implementation"] == impl) &
        (data["threads"] == 4)
    ].sort_values("message_size")

    if subset.empty:
        print(f"Missing data for {impl}")
        continue

    plt.plot(
        subset["message_size"],
        subset["cycles_per_byte"],
        marker='o',
        label=impl.upper()
    )

plt.xlabel("Message Size (Bytes)")
plt.ylabel("CPU Cycles Per Byte")
plt.title("CPU Cycles Per Byte vs Message Size (Threads = 4)\n" + SYS_CONFIG)
plt.legend()
plt.grid(True)
plt.savefig("MT25061_CyclesPerByte_vs_MessageSize.png")
plt.show()

print(" All plots generated successfully")
