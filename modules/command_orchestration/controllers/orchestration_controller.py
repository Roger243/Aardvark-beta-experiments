from modules.command_orchestration.models.command_request import CommandRequest
from modules.command_orchestration.services.command_dispatcher import CommandDispatcher


class OrchestrationController:
    def __init__(self, dispatcher: CommandDispatcher) -> None:
        self._dispatcher = dispatcher

    def run_command(self, command_id: str, executable: str, args: list[str]) -> int:
        request = CommandRequest(command_id=command_id, executable=executable, arguments=args)
        return self._dispatcher.execute(request)
