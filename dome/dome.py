#!/usr/bin/env python3

import sys
import time
import threading

from codenerix_lib.debugger import Debugger

from controller import PS4Controller
from blocks import Info, Analisys, Telemetry, Compass, Tilt, Depth, FPV
from communications import BuoyLink

from gi_import import gi
from gi.repository import GLib, Gtk

# Configuration
config = {}
config['telemetry_simulator'] = True
config['compass'] = {
    'tick': 1/32000
}
config['tilt'] = {
    'tick': 1/32000
}
config['depth'] = {
    'top': 0,
    'bottom': -50,
    'tick': 0.5
}
config['main_cycle_seg'] = 1
config['telemetry_cycle_seg'] = 1
config['gtk_show_borders'] = True
config['greyscale'] = False
config['frames_per_second'] = 25
config['fpv_opencv_adapter'] = 0
config['fpv_image'] = "media/buceo01.png"  # 1+2+3
config['fpv_video'] = "media/alioli_submarine_osd1.mp4"  # 1+2
config['fpv_webrtc'] = "http://localhost"
config['fpv_mode'] = "video"
# config['fpv_mode'] = "image"
# config['fpv_mode'] = "webrtc"
# config['fpv_mode'] = "opencv"

config['ps4'] = {
    'debug': False,
    'real_feel': {
        'compass': None,
        'tilt': None,
        'depth': None,
        # 'compass': 2,
        # 'tilt': 2,
        # 'depth': 0.5,
    },
    'dev' : '/dev/input/js0',
    'refresh' : 0.1,
    'longpress': 0.5,
    'mapping' : {
        'compass': ('r3_left', 'r3_right'),
        'tilt': ('r3_up', 'r3_down'),
        'depth': ('up', 'down'),
    }
}

def main_quit(obj):
    '''It stops the thread and the gtk's main loop'''
    global checker
    global fpv
    global ps4
    global buoy
    fpv.stop()
    checker.stop()
    ps4.stop()
    buoy.stop()
    Gtk.main_quit()

class Dome(Gtk.Window, Debugger):

    # dim = (1920, 1080)    # 16:9
    # dim = (1680, 1050)    # 16:10
    # dim = (1440, 1080)    # 4:3
    # dim = (1440, 1050)    # 4:3
    dim = (1600, 900)       # 16:9
    # dim = (1440, 900)     # 16:10
    # dim = (1280, 960)     # 4:3
    # dim = (1368, 768)     # 16:9
    # dim = (1024, 768)

    def __init__(self, config):
        '''Build window'''

        # Prepare debugger
        self.set_debug()

        # Set configuration
        self.config = config
        self.real_feel = config['ps4']['real_feel']

        # Prepare mapping controller
        self.ctrl = {}

        # Prepare elements
        self.info = Info(config)
        self.analisys = Analisys(config)
        self.compass = Compass(config)
        self.tilt = Tilt(config)
        self.depth = Depth(config)
        self.fpv = FPV(config)
        self.telemetry = Telemetry(config)
        self.telemetry.add("compass", self.compass)
        self.telemetry.add("tilt", self.tilt)
        self.telemetry.add("depth", self.depth)

        # Locker
        self.__fpv_lock = threading.Lock()
        self.__command_update_lock = {
            'compass': threading.Lock(),
            'tilt': threading.Lock(),
            'depth': threading.Lock(),
        }

        # Prepare an empty label to fill gaps
        empty = Gtk.Label(label="")
        empty.set_width_chars(2)
        empty.set_max_width_chars(2)
        if self.config.get("gtk_show_borders", False):
            empty = self.add_border(empty)

        # === WINDOWS ===

        # Set Window
        Gtk.Window.__init__(self)
        self.set_title("Alioli Dome")
        self.set_default_size(*(self.dim))
        self.connect("destroy", main_quit)

        # === GRIDS ===
        # Grid.attach(obj, left, top, width=1, height=1)

        # Main elements
        main_top = Gtk.Label(label="Top margin - Tool bar") #, 0, 0, 1 ,1)
        main_top.set_hexpand(True)
        main_body = Gtk.Grid()
        main_body.set_hexpand(True)
        main_body.set_vexpand(True)
        main_bottom = Gtk.Label(label="Bottom margin - Status")
        main_bottom.set_hexpand(True)

        # Setup Main
        main_grid = Gtk.Grid()
        self.add(main_grid)
        main_grid.set_row_spacing(3)
        main_grid.set_column_spacing(1)
        main_grid.attach(self.add_border(main_top), 0, 0, 1, 1)
        main_grid.attach(main_body, 0, 1, 1, 1)
        main_grid.attach(self.add_border(main_bottom), 0, 2, 1, 1)

        # Setup body
        left_grid = Gtk.Grid()
        center_grid = Gtk.Grid()
        right_grid = Gtk.Grid()
        main_body.set_row_spacing(1)
        main_body.set_column_spacing(3)
        main_body.attach(left_grid, 0, 0, 1, 1)
        main_body.attach(center_grid, 1, 0, 1, 1)
        main_body.attach(right_grid, 2, 0, 1, 1)

        # Setup left
        tilt_depth_grid = Gtk.Grid()
        tilt_depth_grid.set_row_spacing(1)
        tilt_depth_grid.set_column_spacing(2)
        left_grid.set_row_spacing(2)
        left_grid.set_column_spacing(1)
        left_grid.attach(self.compass.gtk(), 0, 0, 1, 1)
        left_grid.attach(tilt_depth_grid, 0, 1, 1, 1)
        tilt_depth_grid.attach(self.tilt.gtk(), 0, 0, 1, 1)
        tilt_depth_grid.attach(self.depth.gtk(), 1, 0, 1, 1)

        # Setup center
        center_grid.set_row_spacing(1)
        center_grid.set_column_spacing(1)
        center_grid.attach(self.fpv.gtk(), 0, 0, 1, 1)

        # Setup right
        right_grid.set_row_spacing(4)
        right_grid.set_column_spacing(1)
        right_grid.attach(self.info.gtk(), 0, 0, 1, 1)
        right_grid.attach(self.analisys.gtk(), 0, 1, 1, 1)
        right_grid.attach(self.telemetry.gtk(), 0, 2, 1, 1)

    def add_border(self, obj, border=None):

        if border is None:
            border = self.config.get('gtk_show_borders', False)

        if border:
            lab = Gtk.Frame()
            lab.set_border_width(border)
            lab.add(obj)
        else:
            lab = obj
        return lab

    # === Mappers === ====================================================

    def get_controller_mapper(self):
        '''Return the mapping object for the controller'''
        return self.ctrl

    # === Updaters === ===================================================

    def command_update(self, group, targets):

        # Choose device
        device = None
        if group=='r3':
            if targets[0]=='left' or targets[0]=='right':
                device = 'compass'
            else:
                device = 'tilt'
        elif group=='up' or group=='down':
            device = 'depth'

        # Decide
        if device in ['compass', 'tilt', 'depth']:

            # Get Lock
            if self.__command_update_lock[device].acquire(0):

                # Compass
                if device=='compass' and (self.ctrl['r3']['left'] or self.ctrl['r3']['right']):
                    value = self.compass.command + (self.ctrl['r3']['left'] - self.ctrl['r3']['right']) * self.config['compass']['tick']
                    self.compass.command = value % 360

                # Tilt
                elif device=='tilt' and (self.ctrl['r3']['up'] or self.ctrl['r3']['down']):
                    value = self.tilt.command + (self.ctrl['r3']['up'] - self.ctrl['r3']['down']) * self.config['tilt']['tick']
                    if value>90 and value<180:
                        self.tilt.command = 90
                    elif value>=180 and value<270:
                        self.tilt.command = -90
                    elif value<0:
                        self.tilt.command = - (360 - (value%360))
                    else:
                        self.tilt.command = value % 360

                # Depth
                elif device=='depth' and (self.ctrl['up_clicks'] or self.ctrl['down_clicks']):
                    result = (self.ctrl['up_clicks'] - self.ctrl['down_clicks']) * self.config['depth']['tick'] + self.depth.command
                    self.ctrl['up_clicks'] = 0
                    self.ctrl['down_clicks'] = 0
                    if result>config['depth']['top']:
                        self.depth.command = config['depth']['top']
                    elif result<config['depth']['bottom']:
                        self.depth.command = config['depth']['bottom']
                    else:
                        self.depth.command = result 

                # Release lock
                self.__command_update_lock[device].release()

    def update_fpv(self):
        if self.__fpv_lock.acquire(0):
            self.fpv.update()
            self.__fpv_lock.release()

class FPVUpdater(threading.Thread, Debugger):
    '''This class keeps FPV updated'''

    # Thread event, stops the thread if it is set.
    stopthread = threading.Event()

    def setup(self, config, dome):
        self.set_debug()
        self.dome = dome
        self.config = config
        self.cycle_fpv = 1.0 / float(config["frames_per_second"])

    def update_fpv(self):
        self.dome.update_fpv()
        return False

    def run(self):
        while not self.stopthread.isSet():

            GLib.idle_add(self.update_fpv)

            # Sleep a cycle
            time.sleep(self.cycle_fpv)

    def stop(self):
        '''Sets the event to terminate the thread's main loop'''
        self.stopthread.set()


class PS4Checker(threading.Thread, Debugger):
    '''This class sets the fraction of the progressbar'''

    # Thread event, stops the thread if it is set.
    stopthread = threading.Event()

    def setup(self, config, ps4):
        self.set_debug()
        self.config = config
        self.ps4 = ps4

    def run(self):
        refresh = self.config['ps4']['refresh']
        while not self.stopthread.isSet():

            # Ticks
            self.ps4.tick()

            # Check ps4 and restart if stopped
            if not self.ps4.stopping() and not self.ps4.is_alive():
                ps4 = PS4Controller()
                ps4.setup(*self.ps4.get_setup())
                ps4.start()
                self.ps4 = ps4

            # Sleep a cycle
            time.sleep(refresh)

    def stop(self):
        '''Sets the event to terminate the thread's main loop'''
        self.stopthread.set()


# Prepare dome
dome = Dome(config)
dome.show_all()

# PS4Controller
ps4 = PS4Controller()
ps4.setup(config, dome)
ps4.start()

# Communications
buoy = BuoyLink()
buoy.setup(config, dome)
buoy.start()

# Prepare checker
checker = PS4Checker()
checker.setup(config, ps4)
checker.start()

# Prepare FPV updater
fpv = FPVUpdater()
fpv.setup(config, dome)
fpv.start()

# Work
try:
    Gtk.main()
except KeyboardInterrupt:
    main_quit(None)
if ps4.wait():
    ps4.join()
    ps4.debug("Program finished!", color="green")
else:
    ps4.debug("Hit CTRL+C to finish this program!", color="yellow")
