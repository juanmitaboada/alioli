import time
import threading

from codenerix_lib.debugger import Debugger

from gi_import import gi
from gi.repository import GLib

import random

class BuoyLink(threading.Thread, Debugger):

    # Thread event, stops the thread if it is set.
    stopthread = threading.Event()

    def setup(self, config, dome):

        # Autoconfig
        self.dome = dome
        self.set_debug()
        self.config = config
        self.ts = 0
        self.simulator = config['telemetry_simulator']

        # Map telemetry
        self.dome.compass.telemetry=0.0
        self.dome.tilt.telemetry=0.0
        self.dome.depth.telemetry=0.0

    def where(self, moving, fixed):
        '''
        moving=4; fixed=2; minus=(moving-fixed)%360; plus=(fixed-moving)%360; print("{}->{} :: +:{} -:{}".format(moving, fixed, plus, minus))
        '''
        minus=(moving-fixed)%360
        plus=(fixed-moving)%360
        if minus<plus:
            return -minus
        else:
            return plus

    def run(self):
        while not self.stopthread.isSet():

            if not self.simulator:
                # === NORMAL COMMUNICATION === ================================
                pass
            else:

                # === SIMULATOR === ===========================================

                # Compass
                direction = self.where(self.dome.compass.telemetry, self.dome.compass.command)
                if direction>0:
                    self.dome.compass.telemetry = (self.dome.compass.telemetry + 5) % 360
                elif direction<0:
                    self.dome.compass.telemetry = (self.dome.compass.telemetry - 5) % 360
                else:
                    self.dome.compass.telemetry += (random.randint(0, 2) * -1**random.randint(0,1)) % 360
                time.sleep(0.1)

                # Tilt
                direction = self.where(self.dome.tilt.telemetry, self.dome.tilt.command)
                if direction>0:
                    self.dome.tilt.telemetry = (self.dome.tilt.telemetry + 5) % 360
                elif direction<0:
                    self.dome.tilt.telemetry = (self.dome.tilt.telemetry - 5) % 360
                else:
                    self.dome.tilt.telemetry += (random.randint(0, 2) * -1**random.randint(0,1)) % 360
                time.sleep(0.1)

                # Depth
                direction = self.where(self.dome.depth.telemetry, self.dome.depth.command)
                if direction>0:
                    self.dome.depth.telemetry += 1/4
                elif direction<0:
                    self.dome.depth.telemetry -= 1/4
                else:
                    self.dome.depth.telemetry += (random.randint(0, 2) / 4) * -1**random.randint(0,1)
                time.sleep(0.1)

                # Sleep a cycle
                time.sleep(0.7)

    def stop(self):
        '''Sets the event to terminante the thread's main loop'''
        self.stopthread.set()
