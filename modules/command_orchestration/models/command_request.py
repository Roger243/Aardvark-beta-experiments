from dataclasses import dataclass, field
from typing import Dict, List


@dataclass(frozen=True)
class CommandRequest:
    command_id: str
    executable: str
    arguments: List[str] = field(default_factory=list)
    metadata: Dict[str, str] = field(default_factory=dict)
