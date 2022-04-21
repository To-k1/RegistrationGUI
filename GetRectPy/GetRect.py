def boundingBox(idx,c):
    if len(c) < Config.min_contours: 
        return None
    epsilon = Config.epsilon_start
    while True:
        approxBox = cv2.approxPolyDP(c,epsilon,True)
        #求出拟合得到的多边形的面积
        theArea = math.fabs(cv2.contourArea(approxBox))
        #输出拟合信息
        print("contour idx: %d ,contour_len: %d ,epsilon: %d ,approx_len: %d ,approx_area: %s"%(idx,len(c),epsilon,len(approxBox),theArea))
        if (len(approxBox) < 4):
            return None
        if theArea > Config.min_area:
            if (len(approxBox) > 4):
                # epsilon 增长一个步长值
                epsilon += Config.epsilon_step               
                continue
            else: #approx的长度为4，表明已经拟合成矩形了                
                #转换成4*2的数组
                approxBox = approxBox.reshape((4, 2))                
                return approxBox                
        else:
            print("failed to find boundingBox,idx = %d area=%f"%(idx, theArea))
            return None

#针对每个轮廓，拟合外接四边形,如果成功，则将记录该四个点
for idx,c in enumerate(contours):
    approxBox = boundingBox(idx,c)
    if approxBox is None: 
#        print("\n")
        continue
    
     # 获取最小矩形包络
    rect = cv2.minAreaRect(approxBox)
    box = cv2.boxPoints(rect)
    box = np.int0(box)
    box = box.reshape(4,2)
    box = order_points(box)
    print("boundingBox：\n",box)   