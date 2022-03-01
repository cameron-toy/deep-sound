from PyQt6.QtWidgets import QApplication, QMainWindow, QSlider, QLabel, QPushButton
from PyQt6.QtCore import Qt
import os

# Only needed for access to command line arguments
import sys

F1 = 2700
F2 = 4800

class Demo(QMainWindow):
    slider_labels = []

    def __init__(self):
        super().__init__()
        self.initUI()
    
    def initUI(self):
        self.setWindowTitle("Deep Sound Demo")

        s1 = self.add_slider(F1, 30, 40, 200, 30, 0, 7500)
        s2 = self.add_slider(F2, 30, 90, 200, 30, 0, 7500)
        button = QPushButton("Release!", self)
        button.setGeometry(115, 150, 100, 30)
        button.clicked.connect(lambda _: self.do_release(s1, s2))

        self.setGeometry(600, 600, 350, 250)
        self.show()

    def do_release(self, s1, s2):
        os.system(f"python gen_knock.py {s1.value()} {s2.value()} 0.15 0.15 8")
    
    def add_slider(self, initial, ax, ay, aw, ah, low, high):
        slider = QSlider(Qt.Orientation.Horizontal, self)
        slider.setFocusPolicy(Qt.FocusPolicy.NoFocus)
        slider.setGeometry(ax, ay, aw, ah)
        slider.setMinimum(low)
        slider.setMaximum(high)
        slider.setValue(initial)


        label = QLabel(self)
        label.setText(str(initial))
        label.setGeometry(ax + 220, ay, 80, 30)
        slider.valueChanged[int].connect(lambda v: label.setText(str(v)))
        
        self.slider_labels.append(label)
        return slider
    


def main():
    # You need one (and only one) QApplication instance per application.
    # Pass in sys.argv to allow command line arguments for your app.
    # If you know you won't use command line arguments QApplication([]) works too.

    app = QApplication(sys.argv)

    # Create a Qt widget, which will be our window.
    window = Demo()

    # Start the event loop.
    app.exec()


if __name__=="__main__":
    main()