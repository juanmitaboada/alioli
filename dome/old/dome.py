import gi
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk


class Handler(Gtk.Window):

    def onDestroy(self, *args):
        Gtk.main_quit()

    def compass_slider(self, *args):
        print("Hello World")


builder = Gtk.Builder()
builder.add_from_file("dome.glade")
builder.connect_signals(Handler())

compass = builder.get_object("compass")
depth = builder.get_object("depth")
for pos in range(0, 100, 5):
    print(pos)
    depth.add_mark(pos, Gtk.PositionType.RIGHT)

window = builder.get_object("Dome")
window.show_all()

Gtk.main()
