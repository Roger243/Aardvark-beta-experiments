from collections import deque
from typing import Deque, Optional

from modules.command_orchestration.models.task_record import TaskRecord


class TaskScheduler:
    def __init__(self) -> None:
        self._queue: Deque[TaskRecord] = deque()

    def enqueue(self, task: TaskRecord) -> None:
        self._queue.append(task)

    def dequeue(self) -> Optional[TaskRecord]:
        if not self._queue:
            return None
        return self._queue.popleft()

    def pending_count(self) -> int:
        return len(self._queue)
