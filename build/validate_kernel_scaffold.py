from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
header = (root / "include" / "winagent.h").read_text()
source = (root / "src" / "winagent.c").read_text()
inf = (root / "winagent.inf").read_text()
project = (root / "winagent.vcxproj").read_text()
readme = (root / "README.md").read_text()

required_header_tokens = [
    "#define PRIORITY_BOOSTER_DEVICE 0x8000",
    "METHOD_NEITHER",
    "FILE_ANY_ACCESS",
    "PRIORITY_BOOSTER_DEVICE_NAME L\"\\\\Device\\\\PriorityBooster\"",
    "PRIORITY_BOOSTER_SYMBOLIC_LINK_NAME L\"\\\\??\\\\PriorityBooster\"",
    "DRIVER_INITIALIZE DriverEntry",
]

for token in required_header_tokens:
    if token not in header:
        raise SystemExit(f"missing header token: {token}")

required_source_tokens = [
    "IoCreateDevice(",
    "IoCreateSymbolicLink",
    "IRP_MJ_DEVICE_CONTROL",
    "IOCTL_PRIORITY_BOOSTER_SET_PRIORITY",
    "ProbeForRead",
    "STATUS_NOT_SUPPORTED",
    "RTL_CONSTANT_STRING(PRIORITY_BOOSTER_DEVICE_NAME)",
    "FILE_DEVICE_UNKNOWN",
    "IoDeleteSymbolicLink",
    "IoDeleteDevice",
    "KdPrint((\"PriorityBooster: Failed to create device object",
]

for token in required_source_tokens:
    if token not in source:
        raise SystemExit(f"missing source token: {token}")

if "ServiceType=1" not in inf or "AddService=PriorityBooster" not in inf or "PriorityBooster.sys" not in inf:
    raise SystemExit("INF must install the PriorityBooster kernel driver service")

if "WindowsKernelModeDriver10.0" not in project or "<DriverType>WDM</DriverType>" not in project:
    raise SystemExit("project must target WDM kernel mode")

if "does not bypass Windows kernel security" not in readme:
    raise SystemExit("README must document security boundary")

match = re.search(
    r"#define IOCTL_PRIORITY_BOOSTER_SET_PRIORITY\s+CTL_CODE\(PRIORITY_BOOSTER_DEVICE, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS\)",
    header,
)
if not match:
    raise SystemExit("set-priority IOCTL must preserve requested CTL_CODE contract")

print("kernel scaffold validation passed")
