#! /usr/bin/env python3
import matplotlib.pyplot as pyplot
import cv2
import numpy as np


# 1----------------------------------------------------
def hough(file_name):
    img1 = cv2.imread(file_name, 0)
    edges = cv2.Canny(img1, 30, 70)  # canny 边缘检测
    drawing = np.zeros(img1.shape[:], dtype=np.uint8)
    lines = cv2.HoughLinesP(edges, 0.8, np.pi / 180, 90)

    t1, t2, temp_x, temp_y = lines[0][0]

    for line in lines:
        x1, y1, x2, y2 = line[0]
        cv2.line(img1, (x1, y1), (x2, y2), (255, 255, 255), 1, lineType=cv2.LINE_AA)
    cv2.imwrite('canny.png', img1)


# 1----------------------------------------------------

# 2----------------------------------------------------
def hough_circle(file_name):
    img1 = cv2.imread(file_name, 0)
    img1 = cv2.medianBlur(img1, 5)

    rows = img1.shape[0]
    chart_x = []

    # param2 has big imapct
    circles = cv2.HoughCircles(img1, cv2.HOUGH_GRADIENT, 1, rows / 8, param1=100, param2=10, minRadius=1, maxRadius=40)
    circles = np.uint16(np.around(circles))

    if circles is not None:
        circles = np.uint16(np.around(circles))

    for i in circles[0, :]:
        center = (i[0], i[1])
        # circle center
        cv2.circle(img1, center, 1, (0, 100, 100), 3)

        # circle outline
        radius = i[2]
        chart_x.append(radius)

        cv2.circle(img1, center, radius, (255, 0, 255), 3)

    cv2.imwrite('circles.png', img1)

    # line chart

    chart_x.sort(key=None)
    chart_y = []
    for i in range(len(chart_x)):
        chart_y.append(chart_x[i] * chart_x[i] * np.pi)

    pyplot.plot(chart_x, chart_y, 'bs-')
    pyplot.savefig('line_chart.png')


# 2----------------------------------------------------


if __name__ == '__main__':
    hough('west.png')
    hough_circle('0941.tif')
