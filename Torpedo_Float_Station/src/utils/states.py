from enum import Enum

class WiFiState(Enum):
    WAITING = 0
    FOUND = 1
    ERROR = 2
    RECEIVING = 3