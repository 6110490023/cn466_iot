import line from '@line/bot-sdk';
import express from 'express';
import ngrok from 'ngrok';
import axios from 'axios';
import mqtt from 'mqtt';
import dotenv from 'dotenv';
import { initializeApp } from "firebase/app";
import { getFirestore, collection, getDocs, doc, setDoc } from "firebase/firestore";
// varible
const port = process.env.PORT || 3000;
let currentData = { "status": "undefine" };
dotenv.config()

var mqttClient = mqtt.connect(process.env.HIVEMQ_BROKER);
//mqttClient
mqttClient.on('connect', () => {
     console.log('HIVEMQ connected');
     mqttClient.subscribe(["cn466/door/cucumber_nut/#"], () => {
          console.log("Topic subscribed");
     });
});

mqttClient.on('message', (topic, payload) => {
     console.log('Received Message:', topic, payload.toString())
     currentData = JSON.parse(payload.toString());
     notify(currentData.status)


});
//firebase

const firebaseConfig = {
     apiKey: "AIzaSyAjOywgHdA-wJP-ynu9zJvmQMJ62NBR9io",
     authDomain: "cn466doorman.firebaseapp.com",
     projectId: "cn466doorman",
     storageBucket: "cn466doorman.appspot.com",
     messagingSenderId: "698214544675",
     appId: "1:698214544675:web:1a667f4ce0d8bae4898896",
     measurementId: "G-J50BJPD46E"
};
const firebaseApp = initializeApp(firebaseConfig);
const db = getFirestore();
async function set(nameCollection, nameDocument, data) {
     try {
          await setDoc(doc(db, nameCollection, nameDocument), data);

          console.log("Document written with ID: ", nameDocument);
     } catch (e) {
          console.error("Error adding document: ", e);
     }
}
async function get(nameCollection) {
     const querySnapshot = await getDocs(collection(db, "users"));

}

const config = {
     channelAccessToken: process.env.CHANNEL_ACCESS_TOKEN,
     channelSecret: process.env.CHANNEL_SECRET,
};

const lineClient = new line.Client(config);
const app = express();

app.post('/callback', line.middleware(config), (req, res) => {
     Promise
          .all(req.body.events.map(handleEvent))
          .then((result) => res.json(result))
          .catch((err) => {
               console.error(err);
               res.status(500).end();
          });
});
app.get('/getdate', async (req, res) => {
     //เซตวันที่ส่งไป 12-12-2021 ว-ด-ปี
     const querySnapshot = await getDocs(collection(db, "listDate"));
     let listDate = []
     querySnapshot.forEach((doc) => {
          console.log(doc.id);
          listDate.push(doc.id);
     }
     );
     res.json({ data: listDate });
});
async function handleEvent(event) {
     if (event.type !== 'message' || event.message.type !== 'text') {
          // ignore non-text-message event
          return Promise.resolve(null);
     }
     let text = event.message.text
     if (text == 'สถานะ') {
          let echo = [{ type: 'text', text: "สถานะประตู :"+currentData.status }];
          return lineClient.replyMessage(event.replyToken, echo);
     }
     let arraytext = text.split(" ");
     if (arraytext[1] == 'ดูประวัติ') {
          const querySnapshot = await getDocs(collection(db,arraytext[0] ));
          let dataDoc = "ประวัติของวันที่  " +arraytext[0]+"\n=======================";
          querySnapshot.forEach((doc) => {
               dataDoc = dataDoc+"\n" +doc.data().status+" ประตูเมื่อเวลา "+doc.data().Time ;
          });
          let echo = [{ type: 'text', text: dataDoc }];
          return lineClient.replyMessage(event.replyToken, echo);
     }
     if (arraytext[1] == 'จำนวนครั้ง') {
          const querySnapshot = await getDocs(collection(db,arraytext[0] ));
          let count =0;
          querySnapshot.forEach((doc) => {
               count= count+1;
          });
          let dataDoc = "จำนวนการเปิด-ปิดประตู" +" เท่ากับ "+ count;
          let echo = [{ type: 'text', text: dataDoc }];
          return lineClient.replyMessage(event.replyToken, echo);
     }

     let echo = [];
     echo.push({ type: 'text', text: text });
     return lineClient.replyMessage(event.replyToken, echo);
}

function notify(status) {
     var today = new Date();
     var date = today.getDate() + '-' + (today.getMonth() + 1) + '-' + today.getFullYear();
     var time = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
     set(date, time, { "status": status, "Date": date, "Time": time });
     set("listDate", date, {});
     lineClient.broadcast({ type: 'text', text: "สถานะประตู: " + status });
}

async function initServices() {
     //const baseURL = await ngrok.connect(port);
     const baseURL = process.env.BASE_URL;
     await lineClient.setWebhookEndpointUrl(baseURL + '/callback');
     console.log('Set LINE webhook at ' + baseURL);
}

app.use(express.static('static'));
initServices();
app.listen(port, () => {
     console.log(`listening on ${port}`);
});