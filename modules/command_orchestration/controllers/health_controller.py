from modules.command_orchestration.services.task_scheduler import TaskScheduler


class HealthController:
    def __init__(self, scheduler: TaskScheduler) -> None:
        self._scheduler = scheduler

    def status(self) -> dict[str, int | str]:
        return {
            "module": "command_orchestration",
            "pending_tasks": self._scheduler.pending_count(),
            "state": "healthy",
        }
