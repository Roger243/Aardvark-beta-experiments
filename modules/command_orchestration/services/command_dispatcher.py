import subprocess
from typing import Optional

from modules.command_orchestration.models.command_request import CommandRequest
from modules.command_orchestration.utils.validation import validate_command_request


class CommandDispatcher:
    def execute(self, request: CommandRequest, timeout_seconds: Optional[int] = 30) -> int:
        validate_command_request(request)
        completed = subprocess.run(
            [request.executable, *request.arguments],
            timeout=timeout_seconds,
            check=False,
            capture_output=True,
            text=True,
        )
        return completed.returncode
