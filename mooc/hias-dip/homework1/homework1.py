#! /usr/bin/env python3
import matplotlib.pyplot as pyplot
import cv2
import numpy as np

def calculate_hist(img): # img --> hist
    hist = []
    array = np.array(img)
    for i in range(256):
        hist.append(len(array[array == i]))
    # print(hist)
    # print(array)
    return hist

def get_cdf(hist): # hist --> cdf (cumulative distribution function)
    hist = np.array(hist)
    sum_hist = sum(hist)

    Pr = hist/sum_hist

    S = []
    cdf = 0
    for i in Pr:
        cdf = cdf + i
        S.append(cdf)
    S = np.array(S)
    # print(S)
    return S

def show_img_hist(img,title):

    img_hist = cv2.calcHist([img],[0],None,[256],[0,256])
    pyplot.subplot(1,4,1)
    pyplot.plot(img_hist,color="b")
    pyplot.title(title)

if __name__ == "__main__":
    input = cv2.imread('pout.tif',0)
    show_img_hist(input,"img_input")

    target_hist = []
    for i in range(256): # 在此可手动更改 hist
        target_hist.append(i)

    pyplot.subplot(1,4,2)
    pyplot.plot(target_hist,color="b")
    pyplot.title("target_hist")

    src_map = np.interp(get_cdf(calculate_hist(input)),get_cdf(target_hist),range(256))
    # 更改 hist即可

    pyplot.subplot(1,4,3)
    pyplot.plot(src_map,color="b")
    pyplot.title("map")

    print(np.array(input))
    output = np.array(input)
    for i in range(len(output)):
        for j in range(len(output[i])):
            output[i][j] = src_map[output[i][j]]

    pyplot.subplot(1,4,4)
    pyplot.plot(calculate_hist(output),color="b")
    pyplot.title("output_img")
    pyplot.show()

    cv2.imshow('img_input',input)
    cv2.imshow('img_output',output)
    cv2.imwrite('img_output_1.png',output)
    cv2.waitKey()
    cv2.destroyAllWindows()
   
