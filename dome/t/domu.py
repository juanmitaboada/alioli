#!/usr/bin/env python

# Libraries
import os
import sys
import time
import datetime

import gtk
import gtk.glade

import matplotlib
from matplotlib.figure import *
from matplotlib.axes import *
from matplotlib.backends.backend_gtk import *
from pylab import *

matplotlib.use('GTK')


class DomuGTK:
    """Cologic Analizer"""
    
    def __init__(self):
        
        #Set the Glade file
        self.gladefile = "domu/domu.glade"  
        self.wTree = gtk.glade.XML(self.gladefile) 
        
        # Get widgets that we will use
        self.about=self.wTree.get_widget("about")
        self.calendar=self.wTree.get_widget("calendar")
        self.btncontroller=self.wTree.get_widget("btnPlot")
        self.entry_start=self.wTree.get_widget("entry_start")
        self.entry_end=self.wTree.get_widget("entry_end")
        self.entry_resolution=self.wTree.get_widget("entry_resolution")
        self.entry_system=self.wTree.get_widget("entry_system")
        self.error_dialog=self.wTree.get_widget("error_dialog")
        
        # Set start/end date
        now=time.time()
        fechatuple=datetime.datetime.fromtimestamp(now).timetuple()
        (day,month,year)=(fechatuple[2],fechatuple[1],fechatuple[0])
        self.entry_start.set_text("%02d/%02d/%s" % (day,month,year))
        self.entry_end.set_text("%02d/%02d/%s" % (day,month,year))
        
        # Set resolution
        self.entry_resolution.set_active(3)
        # Update the date_range variable
        self.update_date_range()
        
        # Draw canvas
        self.figure = Figure(figsize=(6,4), dpi=72)
        self.axis = self.figure.add_subplot(111)
        
        self.axis.set_xlabel('Time')
        self.axis.set_ylabel('%')
        self.axis.set_title('Usage')
        self.axis.grid(True)
        
        self.canvas = FigureCanvasGTK(self.figure)
        self.canvas.show()
        self.graphview = self.wTree.get_widget("vbox3")
        self.graphview.pack_start(self.canvas, True, True)  
        
        # Selected box
        self.__selected_entry_calendar=None
        
        # Connect to the database
        
        # List of systems
        # query="SELECT name FROM systems_system"
        # total=self.__DB.query(query)
        # if total:
        #     rows=self.__DB.fetchall()
        #     for (name,) in rows:
        #         self.entry_system.append_text(name)
        self.entry_system.set_active(0)
        
        #Create our dictionay and connect it
        dic = {
            "on_btnPlot_released" : self.btnPlot_released,
            "on_entry_start_icon_press" : self.entry_start_icon_press,
            "on_entry_end_icon_press" : self.entry_end_icon_press,
            "on_calendar_day_selected_double_click" : self.calendar_day_selected_double_click,
            "on_calendar_focus_out_event": self.calendar_close,
            "on_entry_resolution_changed" : self.entry_resolution_changed,
            "on_entry_system_changed": self.entry_system_changed,
            "on_about_activate" : self.about_open,
            "on_error_dialog_action_area_button_press_event" : self.error_dialog_action_area_button_press_event,
#            "on_about_close" : self.gtk_widget_destroy,
            "on_exit_activate" : gtk.main_quit,
            "on_MainWindow_destroy" : gtk.main_quit }
        self.wTree.signal_autoconnect(dic)
    
    def __del__(self):
        self.__DB.disconnect()
    
    def __todate(self,epoch):
        if type(epoch)==type([]):
            a=[]
            for e in epoch:
                a.append(self.__todate(e))
        else:
            a=datetime.datetime.fromtimestamp(epoch)
        return a
    
    def about_open(self, widget):
        self.about.show()
    
    def about_close(self, widget):
        print("Destroy about")
        widget.destroy()
        self.about.destroy()
    
    def btnPlot_released(self, widget):
        self.plot()
        
    def plot(self):
        # Get limits
        start_date=self.entry_start.get_text()
        end_date=self.entry_end.get_text()
        
        # Split the limits (start)
        start_split=start_date.split("/")
        if len(start_split)!=3:
            self.error_dialog.text="ABC"
            self.error_dialog.show()
            return
        else:
            start_date=time.mktime(datetime.datetime(int(start_split[2]),int(start_split[1]),int(start_split[0])).timetuple())-1
        # Split the limits (end)
        end_split=end_date.split("/")
        if len(end_split)!=3:
            self.error_dialog.text="ABC"
            self.error_dialog.show()
            return
        else:
            end_date=time.mktime(datetime.datetime(int(end_split[2]),int(end_split[1]),int(end_split[0])).timetuple())+24*60*60+1
        # Control dates
        if end_date<=start_date:
            self.error_dialog.text="XYZ"
            self.error_dialog.show()
            return
        
        # Prepare the graph
        self.figure = Figure(figsize=(6,4), dpi=72)
        self.axis = self.figure.add_subplot(111)
        
        self.axis.set_xlabel('Time')
        self.axis.set_ylabel('%')
        self.axis.set_title('Usage')
        self.axis.grid(True)
        
        # Get the selected row
        query="SELECT id FROM systems_system WHERE name='%s'" % (self.entry_system.get_active_text())
        # total=self.__DB.query(query)
        # if total!=1:
        #     self.error_dialog.text="MNP"
        #     self.error_dialog.show()
        #     return
        # system_id=self.__DB.fetchall()[0][0]
        
        # Prepare the request
#        query = "SELECT UNIX_TIMESTAMP(`date`),FROM_UNIXTIME( UNIX_TIMESTAMP( `date` ) - UNIX_TIMESTAMP( `date` ) %% %s ) AS checkpoint, AVG(`usage`) AS usage_avg FROM systems_statistic WHERE `date`>=FROM_UNIXTIME(%s) AND `date`<FROM_UNIXTIME(%s) AND system_id=%s AND kind='<kind>' GROUP BY checkpoint" % (self.__date_range,start_date,end_date,system_id)
        query = "(SELECT UNIX_TIMESTAMP(`date`) AS `moment`,FROM_UNIXTIME( UNIX_TIMESTAMP( `date` ) - UNIX_TIMESTAMP( `date` ) %% %s ) AS checkpoint, AVG(`usage`) AS value FROM systems_statistic WHERE `date`>=FROM_UNIXTIME(%s) AND `date`<FROM_UNIXTIME(%s) AND system_id=%s AND kind='<kind>' GROUP BY checkpoint) UNION (SELECT UNIX_TIMESTAMP(`date`) AS moment,NULL,0 FROM systems_statistic WHERE `date`>=FROM_UNIXTIME(%s) AND `date`<FROM_UNIXTIME(%s) AND system_id=%s AND kind='<kind>' AND `usage` IS NULL) ORDER BY moment ASC" % (self.__date_range,start_date,end_date,system_id,start_date,end_date,system_id)
        legend=[]
        total={}
        total[start_date]=None
        total[end_date]=None
        
        # Get data from the database
        # for (kind,name,color) in [('MOUSE','Mouse','r'),('KEY','Keyboard','b'),('USER','User','c'),('CPU','CPU','y')]:
        for (kind, name, color) in [('USER','User','r'),('CPU','CPU','b')]:
            # if kind=='USER':
            #     results=self.__DB.query(query.replace("kind='<kind>'","( kind='MOUSE' OR kind='KEY' )"))
            # else:
            #     results=self.__DB.query(query.replace("<kind>",kind))
            # if results:
            #     rows=self.__DB.fetchall()
            # else:
            #     continue
            rows = []
            e=[start_date]
            d=[None]
            for (tdate_int,gar,usage) in rows:
                e.append(tdate_int)
                d.append(usage)
                tdate=str(tdate_int)
                if tdate in total:
                    total[tdate].append(usage)
                else:
                    total[tdate]=[usage]
            e.append(end_date)
            d.append(None)
            
            # Plot the result
            self.axis.plot(self.__todate(e),d,color=color)
            # Remember the legend
            legend.append(name)
        
        # Plot the legend
        if legend:
            self.axis.legend(legend, "best")
        
        # Canvas limits
        self.axis.set_xlim(self.__todate(start_date-0.15),self.__todate(end_date+0.15))
        self.axis.set_ylim(0,100)
        
        # destroy graph if it already exists
        while True:
            try:
                self.canvas.destroy()
                break
            except:
                break
        self.canvas = FigureCanvasGTK(self.figure) # a gtk.DrawingArea
        self.canvas.show()
        self.graphview = self.wTree.get_widget("vbox3")
        for child in self.graphview.get_children():
            self.graphview.remove(child)
        self.graphview.pack_start(self.canvas, True, True)
    
    def entry_start_icon_press(self,entry_object,entry_icon,button_press):
        self.__selected_entry_calendar=entry_object
        self.calendar.show()
    
    def entry_end_icon_press(self,entry_object,entry_icon,button_press):
        self.__selected_entry_calendar=entry_object
        self.calendar.show()
    
    def calendar_day_selected_double_click(self,widget):
        # Get the selected date
        (year,month,day)=widget.get_date()
        # Set the date in the entry box
        self.__selected_entry_calendar.set_text("%02d/%02d/%s" % (day,month+1,year))
        # Close the widget
        self.calendar_close(widget)
        
    def calendar_close(self, widget, event=None):
        self.calendar.hide()
    
    def error_dialog_action_area_button_press_event(self,widget):
        print(widget)
    
    def entry_resolution_changed(self,widget):
        # Update the date_range variable
        self.update_date_range()
        # Plot
        self.plot()
    
    def entry_system_changed(self,widget):
        # Plot
        self.plot()
    
    def update_date_range(self):
        # Get the selected row
        selected=self.entry_resolution.get_active_text()
        # Split the data
        (size,unit)=selected.split(" ")
        # Process the unit
        if unit in ['second','seconds']:
            multiplier=1
        elif unit in ['minute','minutes']:
            multiplier=60
        elif unit in ['hour','hours']:
            multiplier=60*60
        elif unit in ['day','days']:
            multiplier=60*60*24
        elif unit in ['week','weeks']:
            multiplier=60*60*24*7
        elif unit in ['month','months']:
            multiplier=60*60*24*30
        elif unit in ['year','years']:
            multiplier=60*60*24*365
        # Save the size
        self.__date_range=int(size)*multiplier

if __name__ == "__main__":
    hwg = DomuGTK()
    gtk.main()
