from datetime import datetime

from modules.command_orchestration.models.task_record import TaskRecord
from modules.command_orchestration.services.task_scheduler import TaskScheduler


def test_task_scheduler_enqueue_dequeue() -> None:
    scheduler = TaskScheduler()
    task = TaskRecord(task_id="a", description="demo", created_at=datetime.utcnow())
    scheduler.enqueue(task)

    assert scheduler.pending_count() == 1
    popped = scheduler.dequeue()
    assert popped is not None and popped.task_id == "a"
    assert scheduler.pending_count() == 0
