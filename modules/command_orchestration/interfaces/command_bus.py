from typing import Protocol

from modules.command_orchestration.models.command_request import CommandRequest


class CommandBus(Protocol):
    def execute(self, request: CommandRequest) -> int:
        """Execute a command request and return exit code."""
