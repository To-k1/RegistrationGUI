from math import atan2
from pathlib import Path
import cv2
import os
import numpy as np
import math

# 存储识别到的点
# pts , rectPts= [[]], [[]]

def MkdirAndImwrite(filename, img):
    pwd_f = Path(filename).parent
    if not os.path.exists(pwd_f):
        os.makedirs(pwd_f)
    cv2.imwrite(filename, img)


# 点的顺时针排序
# 计算该点与重心连线的斜率角,输入为2个点
def KAngle(a, center):
    return atan2((a[1] - center[1]), (a[0] - center[0]))
# 对点进行顺时针排序,pts为点集
def ClockwiseSortPoints(pts):
    center = np.array([0, 0], dtype=np.int32)
    x, y = 0, 0
    ptsSize = len(pts)
    for pt in pts:
        x += pt[0]
        y += pt[1]
    # 防止出现除0错误
    if ptsSize > 0:
        center[0] = int(x / ptsSize)
        center[1] = int(y / ptsSize)
    else:
        center[0] = 2000
        center[1] = 1500
    
    # 冒泡排序
    for i in range(0, ptsSize - 1):
        for j in range(0, ptsSize - i - 1):
            a=KAngle(pts[j], center)
            b=KAngle(pts[j + 1], center)
            if KAngle(pts[j], center) > KAngle(pts[j + 1], center):
                tmp = pts[j]
                pts[j] = pts[j + 1]
                pts[j + 1] = tmp

# 输入一个数组,返回他是否有4个不同顶点,th_x表示x轴距离多远之内被视为一个点
def Has4Points(pts, th_x, th_y) -> bool:
    pointNum = len(pts)
    ClockwiseSortPoints(pts)
    for i in range(0, pointNum):
        # 两点挨在一起时pointNum-1
        if abs(pts[i - 1][0] - pts[i][0]) < th_x and abs(pts[i - 1][1] - pts[i][1]) < th_y:
            pointNum -= 1
    if pointNum >= 4:
        return True
    return False

# 画多边形过程
# 去除小连通区域,输入为两张图
def RemoveSmall(src):
    # 将二值图转化为黑白存储到dst中
    dst= cv2.inRange(src, (0, 100, 0), (255, 255, 255))
    MkdirAndImwrite("original.png", dst)
    # 提取连通区域，并剔除小面积联通区域
    contours, hierarchy= cv2.findContours(dst, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    # 图片总面积
    areaSize = len(src[0]) * len(src)
    # 小于图片面积四分之一的视为小区域
    tmp = [i for i in contours if cv2.contourArea(i) >= areaSize / 4]
    # tmp = np.array([])
    # for i in range(len(contours)):
    #     area = cv2.contourArea(contours[i])
    #     if area >= areaSize / 4:
    #         tmp.append(contours[i])
    contours = tmp

    # 将所有像素置零
    dst[:, :] = 0
    cv2.drawContours(dst, contours, -1, [255], cv2.FILLED)
    MkdirAndImwrite("remove.png", dst)
    return dst

# 给二元一次方程组求解(带过程){ax+by+c=0 dx+ey+f=0}
def SolvEqu(a, b, c, d, e, f):
    y = (-f * 1.0 + d * c * 1.0 / a) * 1.0 / (-d * b * 1.0 / a + e * 1.0)
    x = (-c * 1.0 - b * y * 1.0) * 1.0 / a
    return x, y

# 获得直线交点，第4,5,6个参数是HoughLines的rho精度，theta精度和长度阈值
def CntPoint(srcImage,
             rho_h = 1, theta_h = math.pi/360, 
             th_h = 450, srn = 0, stn = 0):
    th_h = int(th_h)
    midImage = cv2.Canny(srcImage, 50, 200, 3)
    dstImage = cv2.cvtColor(midImage, cv2.COLOR_GRAY2BGR)
    MkdirAndImwrite("cannyEdge.png", midImage)
    # 进行霍夫线变换
    lines = cv2.HoughLines(midImage, rho_h, theta_h, th_h)
    pts = [[]]

    # 依次在图中绘制出每条线段,并且两两求交点
    for i in range(len(lines)):
        rho, theta = lines[i][0][0], lines[i][0][1]
        a, b= math.cos(theta), math.sin(theta)
        x0, y0 = a * rho, b * rho
        pt1 = (int(x0 + 10000 * (-b)), int(y0 + 10000 * (a)))
        pt2 = (int(x0 - 10000 * (-b)), int(y0 - 10000 * (a)))
        # 画线
        cv2.line(dstImage, pt1, pt2, (255,255,255))
        # 两两求交点
        for j in range(i+1, len(lines)):
            rho2, theta2 = lines[j][0][0], lines[j][0][1]
            c, d= math.cos(theta2), math.sin(theta2)
            x, y = SolvEqu(a, b, -rho, c, d, -rho2)
            if ((0 <= x and x <= len(srcImage[0]) and 0 <= y and y <= len(srcImage)) 
                 and not ((x > (len(srcImage[0]) / 4) and x < (len(srcImage[0]) / 6 * 5))
                 or (y > (len(srcImage) / 3) and y < (len(srcImage) / 3 * 2)))):
                pts[0].append((int(x), int(y)))

    return pts, dstImage

def IntoPoly(dst, pts):
    # 将交点排序并连成多边形并填充
    ClockwiseSortPoints(pts[0])
    dstImage = np.zeros(shape=(len(dst), len(dst[0]), 3), dtype=np.uint8)
    pts = np.array(pts)
    cv2.fillPoly(dstImage, pts, (255, 255, 255))
    MkdirAndImwrite("srcPloy.png", dstImage)
    dst = dstImage
    return dstImage

# 从前到后分别为变换矩阵，原图，二值图，结果图，中间图1,2，参照图，
# flagM表示是否直接用传进来的变换矩阵为1用,succeed表示是否失败过，为1表示失败过要变更参数
def Registrating1Pic(M, fnSrc, fnBinImg, fnFi, 
                    flagM = False, failed = False,
                    FnMid1 = "srcLines.png", 
                    FnMid2 = "refLines.png", 
                    fnRefImg = "refImg.png") -> bool:
    # 用于记录失败的图片
    failedImg = open("failedImg.txt", "a")
    # IMG1:二值图，refimg：参考二值图，src：原图，dst：结果图
    binImg = cv2.imread(fnBinImg)
    refImg = cv2.imread(fnRefImg)
    src = cv2.imread(fnSrc)
    dst = np.zeros(shape=(len(src), len(src[0]), 3), dtype=np.uint8)
    # 没有二值图(png)返回失败
    if(len(binImg) == 0):
        return True
    refImg = cv2.resize(refImg, (len(src[0]), len(src)))
    # 若有矩阵且使用则直接变换
    if flagM and len(M) != 0:
        dst = cv2.warpPerspective(M, (len(src[0]), len(src)))
    else:
        midImg = RemoveSmall(binImg)
        try:
            if failed:
                pts, binImg = CntPoint(midImg, 12, math.pi / 180, len(src[0])*1000/4096)
            else:
                pts, binImg = CntPoint(midImg, 1, math.pi / 360, len(src[0])*450/4096)
            print(pts[0])
            #由顶点填充多边形
            binImg = IntoPoly(binImg, pts)
            MkdirAndImwrite(fnFi, binImg)
        except Exception:
            # 若已经失败过，记录文件名到txt文件方便以后处理
            if failed:
                failedImg.writelines(str(fnSrc))
            print("该图失败")
            failedImg.close()
            # 删除失败的结果图，方便下一步处理
            if os.path.exists(str(fnFi)):
                os.remove(str(fnFi))
            return False
        
        # 获得填充好的矩形的顶点
        pts, midImg = CntPoint(binImg, 1, math.pi / 360, len(src[0])*450/4096)
        if not Has4Points(pts[0], len(src[0])/4, len(src)/4):
            pts, midImg = CntPoint(binImg, 1, math.pi / 360, len(src[0])*350/4096)
        if not Has4Points(pts[0], len(src[0])/4, len(src)/4):
            pts, midImg = CntPoint(binImg, 1, math.pi / 360, len(src[0])*250/4096)
        if not Has4Points(pts[0], len(src[0])/4, len(src)/4):
            pts, midImg = CntPoint(binImg, 7, math.pi / 180, len(src[0])*700/4096)
        print(pts[0])
        MkdirAndImwrite(FnMid1, midImg)
        if not Has4Points(pts[0], len(src[0])/4, len(src)/4) or len(pts[0])>100:
            print("识别不到四个顶点")
            MkdirAndImwrite(fnFi, midImg)
            return False

        # 获取基准图顶点
        rectPts, midImg = CntPoint(refImg)
        print(rectPts[0])
        MkdirAndImwrite(FnMid2, midImg)

        # 顶点排序
        ClockwiseSortPoints(pts[0])
        ClockwiseSortPoints(rectPts[0])
        # 删除多余顶点,只保留4个
        pts2F , rectPts2F = [(0, 0) for i in range(4)], rectPts[0]
        pts2F , rectPts2F = np.array(pts2F, dtype=np.float32) , np.array(rectPts2F ,dtype=np.float32)
        j = 1
        for i in range(1, len(pts[0])):
            # 不为多余顶点
            if abs(pts[0][i - 1][0] - pts[0][i][0]) > (len(src[0]) / 4) or abs(pts[0][i - 1][1] - pts[0][i][1]) > (len(src) / 4):
                pts2F[j] = pts[0][i]
                j += 1
        pts2F[0] = pts[0][0]
        ClockwiseSortPoints(pts2F)
        for i in range(len(pts2F)):
            print(pts2F[i])
            # 如果有两个点太接近或不足4个点则失败
            if (pts2F[i][0] == 0 and pts2F[i][1] == 0) or ((i > 0) and (abs(pts2F[i - 1][0] - pts2F[i][0]) < (len(src[0]) / 4) and abs(pts2F[i - 1][1] - pts2F[i][1]) < (len(src) / 4))):
                # 若已经失败过，记录文件名到txt文件方便以后处理
                if failed:
                    failedImg.writelines(str(fnSrc))
                print("该图失败")
                failedImg.close()
                # 删除失败的结果图，方便下一步处理
                if os.path.exists(str(fnFi)):
                    os.remove(str(fnFi))
                return False
        
        M = cv2.getPerspectiveTransform(pts2F, rectPts2F)
        dst = cv2.warpPerspective(src, M, (len(src[0]), len(src)))
        # 抠图
        refImg = cv2.bitwise_not(refImg)
        maskedDst = cv2.subtract(dst, refImg)
        MkdirAndImwrite(fnFi, maskedDst)
        failedImg.close()

        return True
            
Registrating1Pic([], 
                r"F:\shixi\CS\registration\registration\srcImg\test\001\Right\R0.jpg",
                r"F:\shixi\CS\registration\registration\binImg\test\001\Right\R0.png",
                r"F:\shixi\CS\registration\registration\dstImg\test\001\Right\R0.png")

    
