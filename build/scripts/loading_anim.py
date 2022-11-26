import itertools
import threading
import time
import sys

animation_thread : threading.Thread
done = False
#here is the animation
def animate():
    for c in itertools.cycle(['|', '/', '-', '\\']):
        if done:
            break
        sys.stdout.write('\rloading ' + c)
        sys.stdout.flush()
        time.sleep(0.1)
    sys.stdout.write('\rDone!     ')

def stop_animation():
  global done
  done = True
  global animation_thread
  animation_thread.join()

def start_animation():
  global animation_thread
  animation_thread = threading.Thread(target=animate)
  animation_thread.start()

if __name__ == "__main__":
  start_animation()