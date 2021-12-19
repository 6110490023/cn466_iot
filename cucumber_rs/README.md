# Cucumber_RS 
### ส่วนที่ใช้ในการ สร้าง Machine Learning 
จะเป็นไฟล์ impulse_data_forwarder.ino 
* งานทำงานคือส่งข้อมูลออกไปยัง edge impulse
* รับ data ไปเทรนMachine Learning 
* นำไฟล์ ที่ได้นำมาใช้เเละอัพลงบอร์ด
### ส่วนที่ใช้ในการ ติดต่อผ่านMQTT HiveMQ 
จะเป็นไฟล์ predict_mqtt.ino
* รับข้อมูลผ่าน sensor ความเร่งเชิงมุมที่บอร์ด 
* ทำการวิเคราะห์ข้อมูลเเละทำนายผลของข้อมูล
* เมื่อมีการขยับตัว จะทำการส่งข้อมูลไปให้ subscriber
