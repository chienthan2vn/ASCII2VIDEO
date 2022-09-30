import cv2
import numpy as np
import os

link = input("Nhap link video:")

cap = cv2.VideoCapture(link)
i = 0
while(cap.isOpened()):
    ret, frame = cap.read()

    # gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    cv2.imshow('frame', frame)
    # Tạo thư mục nếu chưa có
    if not os.path.exists('./data'):
        os.mkdir('./data')

    cv2.imwrite('./data/' + str(i) + ".jpg", frame)

    print(str(i) + ".jpg")
    i += 1

    if cv2.waitKey(100) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()