# cn466Project DOORMAN </br> [sensor ตรวจจับการเปิดปิดประตูในเเต่ละครั้ง เเละรายงานผ่านไลน์]
## ผู้จัดทำ : เอกนรินทร์ เลิศนันวัฒน์ 6110490023 
### Motivation : 
การตรวจจับการเปิด-ปิดประตูในเเต่ละวันเเละสามารถกลับมาย้อนดูในภายหลังได้ว่าตอนนี้ประตูปิดหรือเปิดอยู่ พร้อมทั้งจะเเจ้งเตือนเมื่อมีการเปิดประตู เนื่องจากห้อง/บ้าน นั้นเป็นพื้นที่ส่วนตัวไม่ต้องการให้ใครเข้ามาโดยไม่ได้รับอนุญาติ หรือเเอบเข้ามาโดยไม่รู้ตัว การเเจ้งเตือนผ่านไลน์จะทำให้มั่นใจได้ว่าเราลืมปิดประตูหรือไม่ มีการเปิดประตูเข้ามาในพื้นที่ส่วนตัวของเราโดยพลการ นอกจากนี้ยังรับรู้ได้ว่าเปิดประตูตั้งเเต่เมื่อไหร่ปิดเมื่อไรเเละเปิดปิดประตูไปเเล้วกี่ครั้ง 
### Requirement :
- บอร์ด Cucumber RS: ใช้เซนเซอร์วัดความเร่งเชิงมุม เซนเซอร์วัดความเร่งเชิงมุมเเละวิเคราะห์การเคลื่อนไหว เเละรายงานค่าทุกครั้งที่มีการเปิดหรือปิดประตู
- HiveMQ: topic ของเซนเซอร์ในการรายงานการเปิดปิดประตูคือ "cn466/door/cucumber_nut/#"
- LINE bot: รองรับ text message
- edge Impulse: ทำการสร้าง ML ในการตรวจสอบการเคลื่อนไหวของข้อมูล
- LIFF: UI component เป็นการแสดงหน้าเว็บไซต์แบบ tall
- Web API: URL API ใช้ในการส่งข้อมูลจาก server มาให้clientโดยข้อมมูลเป็นเเบบjson
- Cloud service:ใช้ Firebase ใข้เป็นที่เก็บ database
- hosting: ใช้ herogu ในการเป็น web server
### Design :
1. สร้าง Machine Learning ด้วย Edge Impulse เพือใช้ในการวิเคราะห์ข้อมูลจาก sensor ว่ามีการเปิดหรือปิดประตูหรือไม่ 
2. นำ Machine Learning มาอัพลง Cucumber RS โดยให้มีการเเจ้งเตือนผ่าน MQTT HiveMQ ในtopic "cn466/door/cucumber_nut/#"
3. สร้าง progarm ไว้สำหรับติดต่อกับ  LINE bot ใน LINE Developers เพื่อรับค่าจาก MQTT HiveMQ เมื่อมีการเปิด/ปิดประตู เเละทำการเเจ้งเตือนผ่านไลน์
4. line bot กำหนดให้มีการตอบกลับ text message ดังนี้
      * สถานะ :จะทำการตอบกลับสถานะปัจจุบัน
      * วว-ดด-ปปปป ดูประวัติ : บอกประวัติ ทั้งหมดของวันนั้น เวลาที่เปิดปิด เเละสถานะว่าเปิดหรือปิด 
      * วว-ดด-ปปปป จำนวนครั้ง : บอกจำนวนครั้งของการเปิดเเละปิดประตู
5. สร้าง Firebase สำหรับเก็บข้อมูล การเปิดปิดโดยมี ข้อมูล [ วัน-เดือน-ปี เวลา เเละ สถานะเปิดหรือปิด ]  
6. สร้างหน้าเว็บสำหรับให้userเรียกใช้ผ่านLINE LIFF โดยให้สามรถเลือกวันที่ เเละสามรถเลือกได้ว่าจะ ดูจำนวนครั้ง หรือ ประวัติ
7. สร้าง LINE LIFF ใน LINE Developers เพื่อทำการเชื่อมไปยังweb ที่เราได้เตรียมไว้
8. อัพโหลดข้อมูลขึ้น web server โดย ใช้ heroku 
### Test :
* รับ text message "สถานะ" จะตอบกลับเป็นข้อความ "สถานะประตู : <เปิด/ปิด>"
* รับ text message "วว-ดด-ปปปป จำนวนครั้ง" จะตอบกลับเป็นข้อความ "จำนวนการเปิด-ปิดประตู เท่ากับ <จำนวนครั้ง>"
* รับ text message "วว-ดด-ปปปป ดูประวัติ" จะตอบกลับเป็นข้อความ 
    * ประวัติของวันที่  วว-ดด-ปปปป
    * <เปิด/ปิด> ประตูเมื่อเวลา ชช:นน:วว
    * <เปิด/ปิด> ประตูเมื่อเวลา ชช:นน:วว
* กด LINE LIFF จากนั้นเลือก วันที่เเละคำสั่ง จะสามารถส่งข้อความ ไปยัง LINE bot ได้ 
### สิ่งที่ใช้ 
* HiveMQ
          * topic : cn466/natthaporn/backache/#
          * payload: การเคลื่อนที่(x,y,z) และสถานะการเคลื่อนไหว
* Edge Impulse
* LINE Bot& LIFF
* Cucumber RS
* Cloud service
     * Heroku เป็น webserver
     * Firebase เป็นdatabase
Web API
     *Fetch API
     
