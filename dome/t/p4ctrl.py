from pyPS4Controller.controller import Controller


class MyController(Controller):

    def on_x_press(self):
        print("on_x_press")

    def on_x_release(self):
        print("on_x_release")

    def on_triangle_press(self):
        print("on_triangle_press")

    def on_triangle_release(self):
        print("on_triangle_release")

    def on_circle_press(self):
        print("on_circle_press")

    def on_circle_release(self):
        print("on_circle_release")

    def on_square_press(self):
        print("on_square_press")

    def on_square_release(self):
        print("on_square_release")

    def on_L1_press(self):
        print("on_L1_press")

    def on_L1_release(self):
        print("on_L1_release")

    def on_L2_press(self, value):
        print("on_L2_press: ", value)

    def on_L2_release(self):
        print("on_L2_release")

    def on_R1_press(self):
        print("on_R1_press")

    def on_R1_release(self):
        print("on_R1_release")

    def on_R2_press(self, value):
        print("on_R2_press: ", value)

    def on_R2_release(self):
        print("on_R2_release")

    def on_up_arrow_press(self):
        print("on_up_arrow_press")

    def on_up_down_arrow_release(self):
        print("on_up_down_arrow_release")

    def on_down_arrow_press(self):
        print("on_down_arrow_press")

    def on_left_arrow_press(self):
        print("on_left_arrow_press")

    def on_left_right_arrow_release(self):
        print("on_left_right_arrow_release")

    def on_right_arrow_press(self):
        print("on_right_arrow_press")

    def on_L3_up(self, value):
        print("on_L3_up: ", value)

    def on_L3_down(self, value):
        print("on_L3_down: ", value)

    def on_L3_left(self, value):
        print("on_L3_left: ", value)

    def on_L3_right(self, value):
        print("on_L3_right: ", value)

    def on_L3_release(self):
        print("on_L3_release")

    def on_R3_up(self, value):
        print("on_R3_up: ", value)

    def on_R3_down(self, value):
        print("on_R3_down: ", value)

    def on_R3_left(self, value):
        print("on_R3_left: ", value)

    def on_R3_right(self, value):
        print("on_R3_right: ", value)

    def on_R3_release(self):
        print("on_R3_release")

    def on_options_press(self):
        print("on_options_press")

    def on_options_release(self):
        print("on_options_release")

    # Next events are only detected when connecting without ds4drv
    def on_share_press(self):
        print("on_share_press")

    def on_share_release(self):
        print("on_share_release")

    def on_playstation_button_press(self):
        print("on_playstation_button_press")

    def on_playstation_button_release(self):
        print("on_playstation_button_release")

# now make sure the controller is paired over the Bluetooth and turn on the listener
MyController(interface="/dev/input/js0").listen()   # , connecting_using_ds4drv=False
