from math import atan2
from typing import List
import cv2

pts , rectPts= [[]], [[]]

# 点的顺时针排序
# 计算该点与重心连线的斜率角,输入为2个点
def KAngle(a, center):
    return atan2((a.y - center.y), (a.x - center.x))
# 对点进行顺时针排序,pts为点集
def ClockWiseSortPoints(pts):
    center = 0
    x, y = 0, 0
    ptsSize = len(pts)
    for pt in pts:
        x += pt.x
        y += pt.y
    # 防止出现除0错误
    if ptsSize:
        center.x = x / ptsSize
        center.y = y / ptsSize
    else:
        center.x = 2000
        center.y = 1500
    
    # 冒泡排序
    for i in range(0, ptsSize - 1):
        for j in range(0, i - 1):
            if KAngle(pts[j], center) > KAngle(pts[j + 1], center):
                tmp = pts[j]
                pts[j] = pts[j + 1]
                pts[j + 1] = tmp
