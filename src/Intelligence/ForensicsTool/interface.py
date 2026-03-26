from typing import Protocol

class ModuleProtocol(Protocol):
    def initialize(self) -> str: ...
