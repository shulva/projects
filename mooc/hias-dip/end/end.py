import cv2
import sys
import numpy
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

        self.bt1 = self.window.findChild(QPushButton, "pushButton")
        self.bt1.clicked.connect(self.bt1_click)

        self.bt2 = self.window.findChild(QPushButton, "pushButton_2")
        self.bt2.clicked.connect(self.bt2_click)

    @Slot()
    def bt1_click(self):  # 打开/更换图片
        self.file_path, _ = QFileDialog.getOpenFileName(self.window, "选择图片")

        num = cv2.imread(self.file_path, 0)

        self.pix = QtGui.QPixmap(self.file_path)
        self.pix.scaledToWidth(631)
        self.label_1.resize(631, 631 / num.shape[1] * num.shape[0])
        self.label_1.setPixmap(self.pix)
        self.label_1.setScaledContents(True)

    @Slot()
    def bt2_click(self):  # 识别

        num = cv2.imread(self.file_path)
        num_color = num.copy()
        num = cv2.cvtColor(num, cv2.COLOR_BGR2GRAY)

        rect_kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (15, 4))
        sq_kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))

        top_hat = cv2.morphologyEx(num, cv2.MORPH_TOPHAT, rect_kernel)  # 外黑内白识别较佳

        edge = cv2.Sobel(top_hat, ddepth=cv2.CV_32F, dx=1, dy=0, ksize=-1)
        edge = np.absolute(edge)
        (min, max) = (np.min(edge), np.max(edge))
        edge = (255 * ((edge - min) / (max - min)))
        edge = edge.astype('uint8')  # 图 edge

        cv2.imwrite("edge.png", edge)
        # ----------------------
        edge = cv2.morphologyEx(edge, cv2.MORPH_CLOSE, rect_kernel)
        edge = cv2.threshold(edge, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)[1]

        edge = cv2.morphologyEx(edge, cv2.MORPH_CLOSE, sq_kernel)
        # ----------------------

        thresh, hierarchy = cv2.findContours(edge.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        cv2.drawContours(num_color, thresh, -1, (0, 255, 0), 3)  # 图 num_color

        number = []
        for i, c in enumerate(thresh):
            (x, y, w, h) = cv2.boundingRect(c)
            factor = w / float(h)

            if factor > 7  :
                if (w > 300 and w <400) and h > 40:
                    number.append((x, y, w, h))

        number = sorted(number, key=lambda x: x[0])
        print(number)

        outputs = []
        for (i, (ex, ey, ew, eh)) in enumerate(number):

            groupOutput = []
            group = num[ey - 5:ey + eh + 5, ex - 5:ex + ew + 5]
            group = cv2.threshold(group, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)[1]
            digit_cns, hier = cv2.findContours(group.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            digit_cns = sort_outlet(digit_cns, method='left2right')[0]

            for c in digit_cns:
                (x, y, w, h) = cv2.boundingRect(c)
                box = group[y:y + h, x:x + w]
                box = cv2.resize(box, (60, 90))

                scores = []
                for (digit, digit_box) in template().items():
                    result = cv2.matchTemplate(box, digit_box, cv2.TM_CCOEFF)
                    (_, score, _, _) = cv2.minMaxLoc(result)
                    scores.append(score)

                groupOutput.append(str(np.argmax(scores)))

            cv2.rectangle(num_color, (ex - 5, ey - 5), (ex + ew + 5, ey + eh + 5), (255, 0, 0), 1)
            cv2.putText(num_color, "".join(groupOutput), (ex, ey - 15), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
            outputs.extend(groupOutput)

        cv2.imwrite("end.png", num_color)

        self.pix = QtGui.QPixmap("end.png")
        self.pix.scaledToWidth(631)
        self.label_2.resize(631, 631 / num.shape[1] * num.shape[0])
        self.label_2.setPixmap(self.pix)
        self.label_2.setScaledContents(True)


def sort_outlet(outlet, method='left2right'):
    boxs = [cv2.boundingRect(o) for o in outlet]
    reverse = False
    i = 0
    if method == 'right2left' or method == 'bottom2top':
        reverse = True
    if method == 'top2bottom' or method == 'bottom2top':
        i = 1

    (outlet, boxs) = zip(*sorted(zip(outlet, boxs), key=lambda x: x[1][i], reverse=reverse))

    return outlet, boxs


def template():
    img = cv2.imread('template.png')
    template_to_find = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    template_to_find = cv2.threshold(template_to_find, 10, 255, cv2.THRESH_BINARY_INV)[1]
    cv2.imwrite('template_to_find.png', template_to_find)

    outlet, attribute = cv2.findContours(template_to_find.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # cv2.drawContours(img, outlet, -1, (0, 0, 255), 3)
    # cv2.imshow("img", img)
    # cv2.waitKey(0)

    sorted_outlet = sort_outlet(outlet)[0]

    digit2outlet = {}

    for i, o in enumerate(sorted_outlet):
        (x, y, w, h) = cv2.boundingRect(o)
        box = template_to_find[y:y + h, x:x + w]
        box = cv2.resize(box, (60, 90))
        digit2outlet[i] = box

    return digit2outlet


if __name__ == "__main__":
    template()

    loader = QUiLoader()

    app = QApplication([])
    app.setStyle("Fusion")

    w = main_window(loader)
    w.window.show()

    app.exec()
