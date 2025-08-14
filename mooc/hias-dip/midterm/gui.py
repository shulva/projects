#!/usr/bin/python
import sys
import matplotlib.pyplot as pyplot
import cv2
import numpy as np

from PySide6.QtWidgets import QApplication, QLabel, QMainWindow, QPushButton, QFileDialog, QSlider, QComboBox
from PySide6.QtCore import QFile, QIODevice, Slot
from PySide6.QtUiTools import QUiLoader
from PySide6 import QtGui


class main_window(QMainWindow):
    def __init__(self, loader):
        super(main_window, self).__init__()

        self.loader = loader

        ui_file = QFile("gui.ui")

        if not ui_file.open(QIODevice.ReadOnly):
            print("无法打开ui文件")
            sys.exit(-1)

        self.window = self.loader.load(ui_file)

        ui_file.close()

        self.label_1 = self.window.findChild(QLabel, "label")
        self.label_2 = self.window.findChild(QLabel, "label_2")
        self.label_3 = self.window.findChild(QLabel, "label_3")

        self.bt1 = self.window.findChild(QPushButton, "pushButton")
        self.bt1.clicked.connect(self.bt1_click)

        self.bt2 = self.window.findChild(QPushButton, "pushButton_2")
        self.bt2.clicked.connect(self.bt2_click)

        self.bt3 = self.window.findChild(QPushButton, "pushButton_3")
        self.bt3.clicked.connect(self.bt3_click)

        self.bt4 = self.window.findChild(QPushButton, "pushButton_4")
        self.bt4.clicked.connect(self.bt4_click)

        self.bt5 = self.window.findChild(QPushButton, "pushButton_5")  # combobox button
        self.bt5.clicked.connect(self.bt5_click)

        self.slider1 = self.window.findChild(QSlider, "horizontalSlider")
        self.slider1.valueChanged.connect(self.slider1_change_value)

        self.slider2 = self.window.findChild(QSlider, "horizontalSlider_2")
        self.slider2.valueChanged.connect(self.slider2_change_value)

        self.combobox = self.window.findChild(QComboBox, "comboBox")
        


    @Slot()
    def bt1_click(self):  # 打开/更换图片
        self.file_path, _ = QFileDialog.getOpenFileName(self.window, "选择图片")

        self.pix = QtGui.QPixmap(self.file_path)
        self.label_1.setPixmap(self.pix)
        self.label_1.setScaledContents(True)

    @Slot()
    def bt2_click(self):  # 绘制直方图
        input = cv2.imread(self.file_path, 0)
        img_hist = cv2.calcHist([input], [0], None, [256], [0, 256])
        pyplot.plot(img_hist, color="b")
        pyplot.title("hist")
        pyplot.xlabel("gray scale")
        pyplot.ylabel("num")
        pyplot.savefig("hist.png")

        self.pix = QtGui.QPixmap("hist.png")
        self.label_3.setPixmap(self.pix)
        self.label_3.setScaledContents(True)

    @Slot()
    def bt3_click(self):  # 直方图均衡化
        input = cv2.imread(self.file_path, 0)
        dst = cv2.equalizeHist(input)
        cv2.imwrite("img_after_equal.png", dst)

        self.pix = QtGui.QPixmap("img_after_equal.png")
        self.label_2.setPixmap(self.pix)
        self.label_2.setScaledContents(True)

        pyplot.plot(dst, color="b")
        pyplot.title("hist")
        pyplot.xlabel("gray scale")
        pyplot.ylabel("num")
        pyplot.savefig("hist.png")

        self.pix = QtGui.QPixmap("hist.png")
        self.label_3.setPixmap(self.pix)
        self.label_3.setScaledContents(True)

    @Slot()
    def bt4_click(self):  # 直方图正规化增强
        input = cv2.imread(self.file_path, 0)
        img_bright = cv2.normalize(input, dst=None, alpha=350, beta=10, norm_type=cv2.NORM_MINMAX)
        cv2.imwrite("img_bright.png", img_bright)

        self.pix = QtGui.QPixmap("img_bright.png")
        self.label_2.setPixmap(self.pix)
        self.label_2.setScaledContents(True)

        pyplot.plot(input, color="b")
        pyplot.title("hist")
        pyplot.xlabel("gray scale")
        pyplot.ylabel("num")
        pyplot.savefig("bright_hist.png")

        self.pix = QtGui.QPixmap("bright_hist.png")
        self.label_3.setPixmap(self.pix)
        self.label_3.setScaledContents(True)

    @Slot()
    def slider1_change_value(self):  # x不变，只变y
        input = cv2.imread(self.file_path, 0)
        rows, cols = input.shape

        factor = rows / 100 * self.slider1.value()
        img_before = np.float32([[0, 0], [cols, 0], [0, rows], [cols, rows]])
        img_after = np.float32([[0, factor], [cols, factor], [0, rows - factor], [cols, rows - factor]])
        array = cv2.getPerspectiveTransform(img_before, img_after)
        img_horizontal = cv2.warpPerspective(input, array, (cols, rows))

        cv2.imwrite("img_horizontal.png", img_horizontal)

        self.pix = QtGui.QPixmap("img_horizontal.png")
        self.label_2.setPixmap(self.pix)
        self.label_2.setScaledContents(True)

    @Slot()
    def slider2_change_value(self):
        input = cv2.imread(self.file_path, 0)
        rows, cols = input.shape

        factor = cols / 100 * self.slider2.value()
        img_before = np.float32([[0, 0], [cols, 0], [0, rows], [cols, rows]])
        img_after = np.float32([[factor, 0], [cols - factor, 0], [factor, rows], [cols - factor, rows]])
        array = cv2.getPerspectiveTransform(img_before, img_after)
        img_vertical = cv2.warpPerspective(input, array, (cols, rows))

        cv2.imwrite("img_vertical.png", img_vertical)

        self.pix = QtGui.QPixmap("img_vertical.png")
        self.label_2.setPixmap(self.pix)
        self.label_2.setScaledContents(True)

    @Slot()
    def bt5_click(self):

        factor = self.combobox.currentIndex()
        input = cv2.imread(self.file_path, 0)

        if factor == 0:
            mean = 0
            sigma = 25
            gauss = np.random.normal(mean, sigma, (input.shape[0], input.shape[1]))

            noisy_image = input + gauss
            noisy_image = np.clip(noisy_image, a_min=0, a_max=255)
        elif factor == 1:
            vals = len(np.unique(input))
            vals = 2 ** np.ceil(np.log2(vals))

            noisy_image = np.random.poisson(input * vals) / float(vals)
        elif factor == 2:
            probability = 0.4
            amount = 0.04

            noisy_image = np.copy(input)
            num = np.ceil(amount * input.size * probability)
            coordinate = [np.random.randint(0, i - 1, int(num)) for i in input.shape]
            noisy_image[coordinate[0], coordinate[1]] = 255

            num_pepper = np.ceil(amount * input.size * probability)
            coordinate = [np.random.randint(0, i - 1, int(num_pepper)) for i in input.shape]
            noisy_image[coordinate[0], coordinate[1]] = 0

        cv2.imwrite("noisy_image.png", noisy_image)
        self.pix = QtGui.QPixmap("noisy_image.png")
        self.label_2.setPixmap(self.pix)
        self.label_2.setScaledContents(True)


if __name__ == "__main__":
    loader = QUiLoader()

    app = QApplication([])
    app.setStyle("Fusion")

    w = main_window(loader)
    w.window.show()

    app.exec()
