from dataclasses import dataclass
from datetime import datetime


@dataclass(frozen=True)
class TaskRecord:
    task_id: str
    description: str
    created_at: datetime
    priority: int = 5
