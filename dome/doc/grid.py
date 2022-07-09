import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

# How it looks:
#
#   B1   B2    B3
#   B1
#   B1   B4 B4 B4


class Grid(Gtk.Window):
    def __init__(self):
        Gtk.Window.__init__(self)
        self.set_title("Grid")
        self.connect("destroy", Gtk.main_quit)

        grid = Gtk.Grid()
        grid.set_row_spacing(5)
        grid.set_column_spacing(5)
        self.add(grid)

        # left:int, top:int, width:int, height:int
        button = Gtk.Button(label="Button 1")
        grid.attach(button, 0, 0, 1, 2)   # izq:0  top:0   ancho:1    alto: 2
        button = Gtk.Button(label="Button 2")
        grid.attach(button, 1, 0, 1, 1)
        button = Gtk.Button(label="Button 3")
        grid.attach(button, 2, 0, 1, 1)
        button = Gtk.Button(label="Button 4")
        grid.attach(button, 1, 1, 2, 1)

window = Grid()
window.show_all()

Gtk.main()
