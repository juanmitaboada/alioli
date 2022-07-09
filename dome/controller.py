import time
import math
import threading

from codenerix_lib.debugger import Debugger

from pyPS4Controller.controller import Controller

from gi_import import gi
from gi.repository import GLib, Gtk

# class PS4Cmds(Controller, Debugger):
#     def setup(self, config, dome):
#         pass

class PS4Cmds(Controller, Debugger):

    keys_used = (
        ('r3', 'left', 32767*5),
        ('r3', 'right', 32767*5),
        ('r3', 'up', 32767*5),
        ('r3', 'down', 32767*5),
        ('up', None, 1),
        ('down', None, 1)
    )

    def setup(self, config, dome):

        # Autoconfig
        self.dome = dome
        self.set_debug()
        self.cdxdebug=super().debug
        self.config = config
        self.ts = 0
        self.ps4_longpress = config['ps4']['longpress']

        # Map controller
        self.ctrl = dome.get_controller_mapper()
        self.ctrl['updated'] = False

        # Prepare longpress structure
        self.__longpress = {}
        for (kind, target, step) in self.keys_used:
            if target:
                key = "{}_{}".format(kind, target)
            else:
                key = kind
            self.__longpress[key] = 0

        # === MAP CONTROLLER === ========================================
        # Joysticks
        setattr(self, "on_L3_up", self.joystick('l3', 'up', 'down'))
        setattr(self, "on_L3_down", self.joystick('l3', 'down', 'up'))
        setattr(self, "on_L3_left", self.joystick('l3', 'left', 'right'))
        setattr(self, "on_L3_right", self.joystick('l3', 'right', 'left'))
        setattr(self, "on_L3_x_at_rest", self.joystick('l3', 'rest_x', None))
        setattr(self, "on_L3_y_at_rest", self.joystick('l3', 'rest_y', None))
        setattr(self, "on_L3_press", self.joystick('l3','press', None))
        setattr(self, "on_L3_release", self.joystick('l3', 'release', None))
        setattr(self, "on_R3_up", self.joystick('r3', 'up', 'down'))
        setattr(self, "on_R3_down", self.joystick('r3', 'down', 'up'))
        setattr(self, "on_R3_left", self.joystick('r3', 'left', 'right'))
        setattr(self, "on_R3_right", self.joystick('r3', 'right', 'left'))
        setattr(self, "on_R3_x_at_rest", self.joystick('r3', 'rest_x', None))
        setattr(self, "on_R3_y_at_rest", self.joystick('r3', 'rest_y', None))
        setattr(self, "on_R3_press", self.joystick('r3', 'press', None))
        setattr(self, "on_R3_release", self.joystick('r3','release', None))
        # Triggers
        setattr(self, "on_L1_press", self.click('l1', None, 'press'))
        setattr(self, "on_L1_release", self.click('l1', None, 'release'))
        setattr(self, "on_R1_press", self.click('r1', None, 'press'))
        setattr(self, "on_R1_release", self.click('r1', None, 'release'))
        setattr(self, "on_L2_press", self.trigger('l2', 'press'))
        setattr(self, "on_L2_release", self.trigger('l2', 'release'))
        setattr(self, "on_R2_press", self.trigger('r2', 'press'))
        setattr(self, "on_R2_release", self.trigger('r2', 'release'))
        # Symbols
        setattr(self, "on_x_press", self.click('x', None, 'press'))
        setattr(self, "on_x_release", self.click('x', None, 'release'))
        setattr(self, "on_triangle_press", self.click('t', None, 'press'))
        setattr(self, "on_triangle_release", self.click('t', None, 'release'))
        setattr(self, "on_circle_press", self.click('c', None, 'press'))
        setattr(self, "on_circle_release", self.click('c', None, 'release'))
        setattr(self, "on_square_press", self.click('s', None, 'press'))
        setattr(self, "on_square_release", self.click('s', None, 'release'))
        # Arrows
        setattr(self, "on_up_arrow_press", self.click('up', 'down', 'press'))
        setattr(self, "on_down_arrow_press", self.click('down', 'up', 'press'))
        setattr(self, "on_up_down_arrow_release", self.click('up', 'down', 'release'))
        setattr(self, "on_left_arrow_press", self.click('left', 'right', 'press'))
        setattr(self, "on_right_arrow_press", self.click('right', 'left', 'press'))
        setattr(self, "on_left_right_arrow_release", self.click('left', 'right', 'release'))
        # Extra
        setattr(self, "on_options_press", self.click('options', None, 'press'))
        setattr(self, "on_options_release", self.click('options', None, 'release'))
        setattr(self, "on_share_press", self.click('share', None, 'press'))
        setattr(self, "on_share_release", self.click('share', None, 'release'))
        setattr(self, "on_playstation_button_press", self.click('playstation', None, 'press'))
        setattr(self, "on_playstation_button_release", self.click('playstation', None, 'release'))

    def joystick(self, kind, target, opposite):

        if not self.ctrl.get(kind, None):
            self.ctrl[kind] = {}

        if target:
            key = "{}_{}".format(kind, target)
            self.ctrl[kind][target] = 0.0
            self.ctrl[kind]['grads'] = 0.0
        else:
            key = kind
            self.ctrl[kind]['button'] = False
            self.ctrl[kind]['button_clicks'] = 0

        def press():
            if self.config['ps4']['debug']:
                self.cdxdebug("Joystick {} {} <> {}".format(kind, target, opposite), color="cyan")
            self.ctrl[kind]['button'] = True
            self.ctrl[kind]['button_clicks'] += 1
            self.__longpress[key] = time.time()
            self.update(kind, [target])

        def release():
            if self.config['ps4']['debug']:
                self.cdxdebug("Joystick {} {} <> {}".format(kind, target, opposite), color="cyan")
            self.ctrl[kind]['button'] = False
            self.__longpress[key] = time.time()
            self.update(kind, [target, opposite])

        def direction_up_left(value):
            if self.config['ps4']['debug']:
                self.cdxdebug("Joystick {} {} <> {}: {}".format(kind, target, opposite, value), color="cyan")
            self.ctrl[kind][target] = -value
            self.ctrl[kind][opposite] = 0.0
            self.grads(self.ctrl[kind])
            self.__longpress[key] = time.time()
            self.update(kind, [target, opposite])

        def direction_down_right(value):
            if self.config['ps4']['debug']:
                self.cdxdebug("Joystick {} {} <> {}: {}".format(kind, target, opposite, value), color="cyan")
            self.ctrl[kind][target] = value
            self.ctrl[kind][opposite] = 0.0
            self.grads(self.ctrl[kind])
            self.__longpress[key] = time.time()
            self.update(kind, [target, opposite])

        def rest_x():
            if self.config['ps4']['debug']:
                self.cdxdebug("Joystick {} {} <> {}".format(kind, target, opposite), color="cyan")
            self.ctrl[kind]['left'] = 0.0
            self.ctrl[kind]['right'] = 0.0
            self.grads(self.ctrl[kind])
            self.__longpress[key] = time.time()
            self.update(kind, target)

        def rest_y():
            if self.config['ps4']['debug']:
                self.cdxdebug("Joystick {} {} <> {}".format(kind, target, opposite), color="cyan")
            self.ctrl[kind]['up'] = 0.0
            self.ctrl[kind]['down'] = 0.0
            self.grads(self.ctrl[kind])
            self.__longpress[key] = time.time()
            self.update(kind, target)

        if target=='up' or target=='left':
            return direction_up_left
        elif target=='down' or target=='right':
            return direction_down_right
        elif target=='rest_x':
            return rest_x
        elif target=='rest_y':
            return rest_y
        elif target=='press':
            return press
        elif target=='release':
            return release
        else:
            raise IOError("Wrong target '{}' valid targets are: up, down, left, right, rest_x, rest_y, press & release".format(target))

    def click(self, kind, opposite, action):

        self.ctrl[kind] = False
        self.ctrl["{}_clicks".format(kind)] = 0

        def press():
            if self.config['ps4']['debug']:
                self.cdxdebug("Click {} {} <> {}: PRESS".format(kind, action, opposite), color="cyan")
            self.ctrl[kind] = True
            self.ctrl['{}_clicks'.format(kind)] += 1
            if opposite:
                self.ctrl[opposite] = False
            self.__longpress[kind] = time.time()
            self.update(kind)
        def release():
            if self.config['ps4']['debug']:
                self.cdxdebug("Click {} {} <> {}: RELEASE".format(kind, action, opposite), color="cyan")
            self.ctrl[kind] = False
            if opposite:
                self.ctrl[opposite] = False
            self.__longpress[kind] = time.time()
            self.update(kind)
        if action=='press':
            return press
        else:
            return release

    def trigger(self, kind, action):

        self.ctrl[kind] = 0.0

        def press(value):
            self.ctrl[kind] = value
            self.__longpress[kind] = time.time()
            self.update(kind)
        def release():
            self.ctrl[kind] = 0.0
            self.__longpress[kind] = time.time()
            self.update(kind)
        if self.config['ps4']['debug']:
            self.cdxdebug("Trigger {} {}".format(kind, action), color="cyan")
        if action=='press':
            return press
        else:
            return release

    def bypass(self):
        def fn():
            pass
        if self.config['ps4']['debug']:
            self.cdxdebug("Pass".format(kind, action), color="cyan")
        return fn

    def grads(self, button):

        # Get axis
        up = button['up']
        down = button['down']
        left = button['left']
        right = button['right']

        # Calculate
        if up > 0:
            if left > 0:
                # UP - LEFT
                value = 180.0 - math.degrees(math.atan2(up, left))
            else:
                # UP - RIGHT
                value = math.degrees(math.atan2(up, right))
        else:
            if left > 0:
                # DOWN - LEFT
                value = 180.0 + math.degrees(math.atan2(down, left))
            else:
                # DOWN - RIGHT
                value = 360.0 - math.degrees(math.atan2(down, right))

        # Set grads
        button['grads'] = value

    def update(self, key, targets=None):
        GLib.idle_add(self.dome.command_update, key, targets)

    def tick(self):
        for (kind, target, step) in self.keys_used:
            if target:
                key = "{}_{}".format(kind, target)
                if self.ctrl[kind][target] and self.__longpress[key]+self.ps4_longpress < time.time():
                    self.ctrl[kind][target]=step
                    self.update(kind, [target])
            else:
                if self.ctrl[kind] and self.__longpress[kind]+self.ps4_longpress < time.time():
                    self.ctrl['{}_clicks'.format(kind)] += step
                    self.update(kind)

class PS4Controller(threading.Thread, Debugger):

    def setup(self, config, dome):
        self.set_debug()
        self.dome = dome
        self.config = config
        self.ps4cmds = PS4Cmds(interface=self.config['ps4']['dev'], connecting_using_ds4drv=False)
        self.ps4cmds.setup(config, dome)

    def get_setup(self):
        return (self.config, self.dome)

    def run(self):
        self.warning("Connect your PS4 device, please!")
        self.stopped = False
        self.ps4cmds.listen(timeout=5)
        self.stopped = True

    def stopping(self):
        return self.ps4cmds.stop

    def stop(self):
        '''Sets the event to terminate the thread's main loop'''
        self.debug("Stop PS4, hit some button in your controller or power it off!", color="yellow")
        self.ps4cmds.stop = True

    def tick(self):
        self.ps4cmds.tick()

    def wait(self):
        while not self.stopped:
            self.debug("Stop PS4, hit some button in your controller or power it off!", color="yellow")
            try:
                time.sleep(1)
            except KeyboardInterrupt:
                break
        return self.stopped

