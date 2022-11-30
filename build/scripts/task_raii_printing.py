class TaskRaiiPrint(object):
  def __init__(self, msg):
    self._msg = msg
    self._finished_msg = "done"

    print(msg)
  
  def failed(self):
    self._finished_msg = "failed"

  def __del__(self):
    print(f"{self._msg} - {self._finished_msg}")