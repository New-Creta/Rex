import logging

class Logger(object):
  def __init__(self, name = "logger", level = logging.DEBUG):
    self.logger = logging.getLogger(name)
    self.logger.setLevel(level)

  def debug(self, msg):
    self.logger.debug(f"[Debug] {msg}")

  def info(self, msg):
    self.logger.info(f"[Info] {msg}")

  def warning(self, msg):
    self.logger.warning(f"[Warning] {msg}")

  def error(self, msg):
    self.logger.error(f"[Error] {msg}")

  def critical(self, msg):
    self.logger.error(f"-- Critical Error Occurred --")
    raise Exception(msg)

class StreamLogger(Logger):
  def __init__(self, name = "logger", level = logging.DEBUG):
    super().__init__(name, level)

    sh = logging.StreamHandler()
    self.logger.addHandler(sh)

class FileLogger(Logger):
  def __init__(self, name = "logger", level = logging.DEBUG):
    super().__init__(name, level)

    fh = logging.FileHandler(f"{name}.log", "w")
    self.logger.addHandler(fh)

def logging_level_from_string(level):
  if level == "debug":
    return logging.DEBUG
  elif level == "info":
    return logging.INFO
  elif level == "warning":
    return logging.WARNING
  elif level == "error":
    return logging.ERROR
  elif level == "critical":
    return logging.CRITICAL

  return logging.NOTSET