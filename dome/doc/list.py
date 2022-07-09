import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

class ListStore(Gtk.Window):
    def __init__(self):
        Gtk.Window.__init__(self)
        self.connect("destroy", Gtk.main_quit)

        liststore = Gtk.ListStore(str, int)
        liststore.append(["Oranges", 5])
        liststore.append(["Apples", 3])
        liststore.append(["Bananas", 2])
        liststore.append(["Tomatoes", 4])
        liststore.append(["Cucumber", 1])

        treeview = Gtk.TreeView()
        treeview.set_model(liststore)
        self.add(treeview)

        cellrenderertext = Gtk.CellRendererText()

        treeviewcolumn = Gtk.TreeViewColumn("Item")
        treeview.append_column(treeviewcolumn)
        treeviewcolumn.pack_start(cellrenderertext, True)
        treeviewcolumn.add_attribute(cellrenderertext, "text", 0)

        treeviewcolumn = Gtk.TreeViewColumn("Quantity")
        treeview.append_column(treeviewcolumn)
        treeviewcolumn.pack_start(cellrenderertext, True)
        treeviewcolumn.add_attribute(cellrenderertext, "text", 1)

window = ListStore()
window.show_all()

Gtk.main()
