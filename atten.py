import cv2
import face_recognition
import numpy as np
import os
from datetime import datetime

# Path to the folder containing images
path = '/home/rayen/camsec/imagesAtten'
images = []
clasNmaes = []
myList = os.listdir(path)
print(myList)

# Load images and corresponding names
for cls in myList:
    curImg = cv2.imread(f'{path}/{cls}')
    images.append(curImg)
    clasNmaes.append(os.path.splitext(cls)[0])

print(clasNmaes)

def findEncodings(images):
    encodeList = []
    for img in images:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        encode = face_recognition.face_encodings(img)[0]
        encodeList.append(encode)
    return encodeList

def markAttendance(name):
    # Read the existing data and store names in a list
    with open('attdata.csv', 'r+') as f:
        myDataList = f.readlines()
        nameList = [line.split(',')[0] for line in myDataList]

        # Write the name only if it is not already present in the file
        if name not in nameList:
            now = datetime.now()
            dString = now.strftime('%H:%M:%S')
            f.writelines(f'\n{name},{dString}')

# Get encodings for known images
encodeListKnown = findEncodings(images)
print(len(encodeListKnown))
print('Encoding Complete')

# Open the webcam
cap = cv2.VideoCapture(0)

while True:
    success, img = cap.read()
    if not success:
        break

    imgS = cv2.resize(img, (0, 0), None, 0.25, 0.25)
    imgS = cv2.cvtColor(imgS, cv2.COLOR_BGR2RGB)

    faceCurFrame = face_recognition.face_locations(imgS)
    encodeCurFrame = face_recognition.face_encodings(imgS, faceCurFrame)

    for encodeFace, faceLoc in zip(encodeCurFrame, faceCurFrame):
        matches = face_recognition.compare_faces(encodeListKnown, encodeFace)
        faceDis = face_recognition.face_distance(encodeListKnown, encodeFace)
        matchIndex = np.argmin(faceDis)

        if matches[matchIndex]:
            name = clasNmaes[matchIndex].upper()
            y1, x2, y2, x1 = faceLoc
            y1, x2, y2, x1 = y1 * 4, x2 * 4, y2 * 4, x1 * 4

            cv2.rectangle(img, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.rectangle(img, (x1, y2 - 35), (x2, y2), (0, 255, 0), cv2.FILLED)
            cv2.putText(img, name, (x1 + 6, y2 - 6), cv2.FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2)

            # Mark attendance for the recognized face
            markAttendance(name)

    cv2.imshow('Webcam', img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
#192.168.1.17