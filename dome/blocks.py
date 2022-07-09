import threading
import cv2

from codenerix_lib.debugger import Debugger

from gi_import import gi
from gi.repository import Gtk, GObject, GdkPixbuf, WebKit2

from matplotlib.figure import Figure
from math import pi
import matplotlib.cm as cm
from matplotlib.backends.backend_gtk3cairo import FigureCanvasGTK3Cairo as FigureCanvas

class Block(Debugger):

    def __init__(self, config, telemetry_init, command_init):

        # Prepare debugger
        self.set_debug()

        # Save config
        self.config = config
        self.real_feel = None

        # Memory
        self.__telemetry = telemetry_init
        self.__telemetry_lock = threading.Lock()
        self.__telemetry_linked = []
        self.__command = command_init
        self.__command_lock = threading.Lock()
        self.__command_linked = []

        # Build gtk element
        self.build()

    @property
    def telemetry(self):
        return self.__telemetry

    @telemetry.setter
    def telemetry(self, value):
        return self.telemetry_set(value)

    def telemetry_set(self, value):
        self.__telemetry = value
        self.tick_telemetry()
        for (client, feedback) in self.__telemetry_linked:
            client.event_telemetry(feedback, value)
        return value

    @property
    def command(self):
        return self.__command

    @command.setter
    def command(self, value):
        return self.command_set(value)

    def command_set(self, value):
        self.__command = self.real_feel_filter(value)
        self.tick_command()
        for (client, feedback) in self.__command_linked:
            client.event_command(feedback, value)
        return value

    def link_telemetry(self, client, feedback):
        self.__telemetry_linked.append((client, feedback))

    def link_command(self, client, feedback):
        self.__command_linked.append((client, feedback))

    def real_feel_filter(self, value):
        return value

    def set_element(self, element):
        # Remember it
        self.element = element

        # Verify that element exists
        if self.element:

            # Get border
            border = self.config.get('gtk_show_borders', False)

            # Set border
            if border:
                lab = Gtk.Frame()
                lab.set_border_width(border)
                lab.add(self.element)
                self.element = lab

    def tick(self):
        self.tick_telemetry()
        self.tick_command()

    def tick_telemetry(self):
        if self.__telemetry_lock.acquire(0):
            self.update_telemetry()
            self.__telemetry_lock.release()

    def tick_command(self):
        if self.__command_lock.acquire(0):
            self.update_command()
            self.__command_lock.release()

    def update_telemetry(self):
        self.warning("update_telemetry() not implemented")

    def update_command(self):
        self.warning("update_command() not implemented")

    def gtk(self):
        return self.element

    def rads(self, grads):
        if grads:
            return grads * pi / 180
        else:
            return 0

class Info(Block):
    dim = (460, 100)
    width_item = 130
    width_description = 330

    def __init__(self, config):
        super().__init__(config, None, None)

    def build(self):

        # Create element
        liststore = Gtk.ListStore(str, str)
        liststore.append(["Amperage", "7.3A (81%)"])
        liststore.append(["Consumption", "1321mA"])
        liststore.append(["Voltage", "12V"])

        element = Gtk.TreeView()
        element.set_model(liststore)

        cell = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("System", cell, text=0)
        column.set_min_width(self.width_item)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)
        column.set_reorderable(True)
        #column.set_fixed_width(50)
        element.append_column(column)

        cell = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Information", cell, text=1)
        column.set_min_width(self.width_description)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)
        column.set_reorderable(True)
        #column.set_fixed_width(50)
        element.append_column(column)

        element.set_size_request(*self.dim)

        self.set_element(element)

class Analisys(Block):
    dim = (460, 100)
    width_item = 130
    width_description = 330

    def __init__(self, config):
        super().__init__(config, None, None)

    def build(self):

        # Create element
        liststore = Gtk.ListStore(str, str)
        liststore.append(["Conductivity", "13.4u"])
        liststore.append(["ORP", "1.1 m2/u"])
        liststore.append(["pH", "6.4 cma"])
        liststore.append(["Salinity", "4.1mpus"])

        element = Gtk.TreeView()
        element.set_model(liststore)

        cell = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Water", cell, text=0)
        column.set_min_width(self.width_item)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)
        column.set_reorderable(True)
        #column.set_fixed_width(50)
        element.append_column(column)

        cell = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Analisys", cell, text=1)
        column.set_min_width(self.width_description)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)
        column.set_reorderable(True)
        #column.set_fixed_width(50)
        element.append_column(column)

        element.set_size_request(*self.dim)

        self.set_element(element)

class Telemetry(Block):
    dim = (460, 100)
    width_sensor = 130
    width_telemetry = 165
    width_command = 165
    elements = [
        ('compass', 'Compass'),
        ('tilt', 'Tilt'),
        ('depth', 'Depth')
    ]

    def __init__(self, config):
        super().__init__(config, [], [])

    def build(self):

        # Create rows

        # Create element
        idx=0
        self.rows={}
        self.liststore = Gtk.ListStore(str, float, float)
        for (key, name) in self.elements:
            self.liststore.append([name, 0.0, 0.0])
            self.rows[key] = (idx, name)
            idx += 1

        element = Gtk.TreeView()
        element.set_model(self.liststore)

        cell = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Sensor", cell, text=0)
        column.set_min_width(self.width_sensor)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)
        column.set_reorderable(True)
        #column.set_fixed_width(50)
        element.append_column(column)

        cell = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Telemetry", cell, text=1)
        column.set_min_width(self.width_telemetry)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)
        column.set_reorderable(True)
        #column.set_fixed_width(50)
        element.append_column(column)

        cell = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Command", cell, text=2)
        column.set_min_width(self.width_command)
        column.set_sizing(Gtk.TreeViewColumnSizing.AUTOSIZE)
        column.set_resizable(True)
        column.set_reorderable(True)
        #column.set_fixed_width(50)
        element.append_column(column)

        element.set_size_request(*self.dim)

        self.set_element(element)

    def add(self, key, block):
        block.link_telemetry(self, key)
        block.link_command(self, key)

    def event_telemetry(self, key, value):
        (idx, name) = self.rows[key]
        self.liststore[idx][1] = value

    def event_command(self, key, value):
        (idx, name) = self.rows[key]
        self.liststore[idx][2] = value

class Compass(Block):
    dim = (360, 600)
    telemetry_color = "blue"
    command_color = "red"

    def __init__(self, config):
        # Initialize
        real_feel = config['ps4']['real_feel']['compass']
        # Prepare line height
        if real_feel:
            self.command_height=10
        else:
            self.command_height=80
        super().__init__(config, 0.0, 0.0)
        self.real_feel = real_feel

    def build(self):

        # Prepare graph
        self.fig = Figure(figsize=(5,5), dpi=100)
        self.ax = self.fig.add_subplot(111, polar=True)
        self.ax.set_theta_zero_location("N")
        self.telemetry_bar = self.ax.bar(0, height=100, width=pi/50, bottom=0, alpha=1, color=[self.telemetry_color], zorder=1)
        self.command_bar = self.ax.bar(0, height=self.command_height, width=pi/50, bottom=0, alpha=1, color=[self.command_color], zorder=2)
        self.ax.plot()

        # Set Canvas
        canvas = FigureCanvas(self.fig)
        canvas.set_size_request(*self.dim)
        element = Gtk.ScrolledWindow()
        element.add_with_viewport(canvas)
        element.set_size_request(*self.dim)

        self.set_element(element)

    @property
    def command(self):
        return super().command

    @command.setter
    def command(self, value):

        # Read feel filter
        if self.real_feel:
            minus=(self.telemetry-value)%360
            plus=(value-self.telemetry)%360
            if minus<plus:
                diff = -minus
            else:
                diff = plus
            if diff>0:
                value = (self.telemetry + self.real_feel) % 360
            elif diff<0:
                value = (self.telemetry - self.real_feel) % 360

        return super().command_set(value)

    def update_telemetry(self):
        # Redraw
        self.telemetry_bar.remove()
        self.telemetry_bar = self.ax.bar(self.rads(self.telemetry), height=100, width=pi/50, bottom=0, alpha=1, color=[self.telemetry_color], zorder=1)
        self.fig.canvas.draw()

    def update_command(self):
        # Redraw
        self.command_bar.remove()
        self.command_bar = self.ax.bar(self.rads(self.command), height=self.command_height, width=pi/50, bottom=0, alpha=1, color=[self.command_color], zorder=2)
        self.fig.canvas.draw()

class Tilt(Block):
    dim = (360, 360)
    telemetry_color = "blue"
    command_color = "red"

    def __init__(self, config):
        # Prepare line height
        real_feel = config['ps4']['real_feel']['tilt']
        if real_feel:
            self.command_height=0.1
        else:
            self.command_height=0.8
        super().__init__(config, 0.0, 0.0)
        self.real_feel = real_feel

    def build(self):

        # Prepare graph
        self.fig = Figure(figsize=(5,5), dpi=100)
        self.ax = self.fig.add_subplot(111, polar=True)
        self.telemetry_bar = self.ax.bar(0, height=1, width=pi/50, bottom=0, alpha=1, color=[self.telemetry_color], zorder=1)
        self.command_bar = self.ax.bar(0, height=self.command_height, width=pi/50, bottom=0, alpha=1, color=[self.command_color], zorder=2)
        self.ax.set_thetamin(-90)
        self.ax.set_thetamax(90)
        self.ax.plot()

        # Set Canvas
        canvas = FigureCanvas(self.fig)
        canvas.set_size_request(*self.dim)
        element = Gtk.ScrolledWindow()
        element.add_with_viewport(canvas)
        element.set_size_request(*self.dim)

        self.set_element(element)

    @property
    def telemetry(self):
        return super().telemetry

    @telemetry.setter
    def telemetry(self, value):

        # Lock limits
        if value>90 and value<180:
            value = 90
        elif value>=180 and value<270:
            value = -90
        elif value>=270:
            value = -(360 - (value%360))

        return super().telemetry_set(value)

    @property
    def command(self):
        return super().command

    @command.setter
    def command(self, value):

        # Real feel filter
        value = value % 360
        if self.real_feel:
            minus=(self.telemetry-value)%360
            plus=(value-self.telemetry)%360
            if minus<plus:
                diff = -minus
            else:
                diff = plus
            if diff>0:
                value = (self.telemetry + self.real_feel) % 360
            elif diff<0:
                value = (self.telemetry - self.real_feel) % 360

        # Lock limits
        if value>90 and value<180:
            value = 90
        elif value>=180 and value<270:
            value = -90
        elif value>=270:
            value = -(360 - (value%360))

        return super().command_set(value)

    def update_telemetry(self):
        # Redraw
        value = self.telemetry
        self.telemetry_bar.remove()
        self.telemetry_bar = self.ax.bar(self.rads(value), height=1, width=pi/50, bottom=0, alpha=1, color=[self.telemetry_color], zorder=1)
        self.fig.canvas.draw()

    def update_command(self):
        # Redraw
        self.command_bar.remove()
        self.command_bar = self.ax.bar(self.rads(self.command), height=self.command_height, width=pi/50, bottom=0, alpha=1, color=[self.command_color], zorder=2)
        self.fig.canvas.draw()

class Depth(Block):
    dim = (180, 360)
    telemetry_color = "blue"
    command_color = "red"

    def __init__(self, config):
        super().__init__(config, 0.0, 0.0)
        self.real_feel = config['ps4']['real_feel']['depth']

    def build(self):

        # Prepare gradient
        xmin, xmax = xlim = 0, 1
        ymin, ymax = ylim = self.config['depth']['bottom']-1, 0
        gradient = [[.6, .6], [.7, .7]]

        # Prepare graph
        # self.fig = Figure(figsize=(1, 10), dpi=100)
        self.fig = Figure()
        self.ax = self.fig.add_subplot(111, xlim=xlim, ylim=ylim, autoscale_on=False)
        self.ax.imshow(gradient, interpolation='bicubic', cmap=cm.cool, extent=(xmin, xmax, ymin, ymax), alpha=0.3)

        # -1 is from -30 1 more
        self.telemetry_bar = self.ax.bar(0.3, height=-1, width=0.2, bottom=0, alpha=1, color=[self.telemetry_color])
        self.command_bar = self.ax.bar(0.3, height=-1, width=0.2, bottom=0, alpha=1, color=[self.command_color])

        self.ax.set_aspect('auto')
        # self.fig.set_size(5, 5)
        self.ax.plot()

        # Set Canvas
        canvas = FigureCanvas(self.fig)
        canvas.set_size_request(*self.dim)
        element = Gtk.ScrolledWindow()
        element.add_with_viewport(canvas)
        element.set_size_request(*self.dim)

        self.set_element(element)

    @property
    def command(self):
        return super().command

    @command.setter
    def command(self, value):

        # Real feel filter
        if self.real_feel and abs(self.telemetry-value)>self.real_feel:
            if self.telemetry>value:
                value = self.telemetry - self.real_feel
            elif self.telemetry<value:
                value = self.telemetry + self.real_feel

        return super().command_set(value)

    def update_telemetry(self):
        # Redraw
        self.telemetry_bar.remove()
        self.telemetry_bar = self.ax.bar(0.6, height=-1, width=0.2, bottom=self.telemetry, alpha=1, color=[self.telemetry_color])
        self.fig.canvas.draw()

    def update_command(self):
        # Redraw
        self.command_bar.remove()
        self.command_bar = self.ax.bar(0.3, height=-1, width=0.2, bottom=self.command, alpha=1, color=[self.command_color])
        self.fig.canvas.draw()

class FPV(Block):
    dim = (600, 480)

    def __init__(self, config):
        super().__init__(config, None, None)

    def build(self):
        # Get basic config
        self.mode = self.config["fpv_mode"]

        # Open Video Capture
        if self.mode == "opencv":
            self.__video = cv2.VideoCapture(self.config["fpv_opencv_adapter"])
        elif self.mode == "video":
            self.__video = cv2.VideoCapture(self.config["fpv_video"])

        # Build FPV object
        if self.mode == 'opencv' or self.mode == 'image' or self.mode == 'video':
            self.__fpv = Gtk.Image()
        elif self.mode == 'webrtc':
            self.__fpv = WebKit2.WebView()
            self.__fpv.load_uri(self.config["fpv_webrtc"])

        self.__fpv.set_size_request(*self.dim)

        self.set_element(self.__fpv)

    def update(self):

        # Redraw
        if self.mode == 'opencv' or self.mode == 'video':

            # Get a frame
            ret, frame = self.__video.read()

            # Rewind if no frame and we are in a video
            if not ret and self.mode == 'video':
                self.__video.set(cv2.CAP_PROP_POS_FRAMES, 0)
                ret, frame = self.__video.read()

            # Check if we got a frame
            if ret:

                # Resize
                frame = cv2.resize(frame, (600, 480), interpolation=cv2.INTER_CUBIC)

                # Set to Gray Scale if requested
                if self.config["greyscale"]:
                    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
                    frame = cv2.cvtColor(frame, cv2.COLOR_GRAY2RGB)
                else:
                    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

                # Channel color correction for Video Adapters
                if self.mode == 'opencv':
                    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

                # Build and dump image to widget
                pb = GdkPixbuf.Pixbuf.new_from_data(frame.tostring(), GdkPixbuf.Colorspace.RGB, False, 8, frame.shape[1], frame.shape[0], frame.shape[2] * frame.shape[1])
                self.__fpv.set_from_pixbuf(pb.copy())

        elif self.mode == 'image':
            self.__fpv.set_from_file(self.config['fpv_image'])

        else:
            pass
